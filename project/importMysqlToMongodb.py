import pymysql
import pymongo
import datetime


######################################  functions  ######################################



# mongodb collection 이름은 공백문자가 올 수 없기 때문에 공백문자를 '_'로 교체한다.
# ex) 현대백화점 쇼핑몰동 -> 현대백화점_쇼핑몰동
def location_name_change_for_mongodb_collection(location):
    location = location.replace(' ','_')
    return location


# body, r, s, t 정보의 '-','_' 값들을 제거해서 리스트로 분리시킨다.
def split_rst(unsplited_rst):
    if unsplited_rst is None:
        return None
    else:
        unsplited_rst = unsplited_rst.split('-')
        splited_rst = list(map(lambda a : a.split('_'), unsplited_rst))
        return splited_rst


# mysql select 쿼리.
def get_sensvalue_from_mysql(nth_sensor, equipid, sensid, from_date, until_date):
    from_date = from_date.strftime('%Y-%m-%d %H:%M:%S')
    until_date = until_date.strftime('%Y-%m-%d %H:%M:%S')
    sql = "select SENSVALUE" + nth_sensor + " from pems.equipment_sensorvalue where EQUIPID='" + equipid + "' AND SENSID='" + sensid + "' AND SENSDATETIME >= '" + from_date + "' AND SENSDATETIME < '" + until_date + "' limit 1;"
    curs.execute(sql)
    return curs.fetchone()


# mongodb insert 쿼리.
def set_sensorvalue_to_mongodb(location, equipname, rst_name, date, max_sensorvalue):
    global db
    location = location_name_change_for_mongodb_collection(location)
    collection = db[location]

    collection.insert({"EQUIPNAME" : equipname, "RST" : rst_name, "DATETIME" : date, "MAX_SENSORVALUE" : max_sensorvalue})


# 해당 지역의 장비ID 가져오기.    
# ex) (입력) location : 외천초 ->  (출력) equipid : "CCB0000001"
def get_equipid(location):
    # 장소에 해당하는 EQUIPID 검색
    global curs
    sql = "select * from pems.equipment where EQUIPNAME='" + location + "';"
    curs.execute(sql)
    row = curs.fetchone()
    equipid = row[2]
    return equipid


# 해당 지역의 장비목록 가져오기.    
# ex) (입력) equipid : "CCB0000001" ->  (출력) equip_list : ['HEAD','TR',...]
def get_equip_list(equipid):
    # 장비 종류 출력 ex) HEAD
    global curs
    sql = "select * from pems.equipment_rst_assign where EQUIPID='" + equipid + "';"
    curs.execute(sql)
    row = curs.fetchall()

    equip_list = []
    for i in range(len(row)):
        # row[][2] : 장비 종류 이름
        equip_list.append(row[i][2])
        
    return equip_list
    

# 장비목록에서 해당 장비의 BODY, R, S, T 정보 가져온다. [         BODY             ,              R           ,  S,T ]
# ex) (입력) equipid : "HEAD" ->  (출력) equip_list : ['T01_23-T01_23-T01_23-...','T01_23-T01_23-T01_23-...', . . .]
def get_rst_mapping_list(equipid, equipname):
    # 선택한 장비에 해당하는 카메라 픽셀 출력
    global curs
    sql = "select * from pems.equipment_rst_assign where EQUIPID='" + equipid + "' AND EQUIP_NAME='" + equipname + "';"
    curs.execute(sql)
    row = curs.fetchone()

    rst_list = []
    rst_list.append(row[4]) # BODY
    rst_list.append(row[5]) # R
    rst_list.append(row[6]) # S
    rst_list.append(row[7]) # T

    return rst_list
        

# 특정 위치의 특정 장비의 BODY or R or S or T 에서 특정 시간대의 최대온도 가져오기.    
# ex) (입력) "특정 장비의 BODY RST 리스트", "CCB0000001"(특정 위치), "2016-09-22 00:00:00", "2017-03-09 00:00:00" ->  (출력)  최대온도값
def get_max_sensorvalue(splited_rst, equipid, from_date, until_date):
    sensorvalue_list = []
    for i in splited_rst:
        sensid = i[0]
        # ex) sensorvalue06 (x) -> sensorvalue6(o) 
        nth_sensor = str(int(i[1]))
        sensorvalue = get_sensvalue_from_mysql(nth_sensor, equipid, sensid, from_date, until_date)
        if sensorvalue is None:
            return None
        else:
            sensorvalue_list.append(sensorvalue[0])

    return max(sensorvalue_list)


# 특정 시간대로 나눠서 import하기
# 
def save_sensorvalue_by_datetime(location, equipname, rst_mapping_list, rst_name, equipid, start_date, finish_date):
    splited_rst = split_rst(rst_mapping_list)

    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')
    until_date = start_date + datetime.timedelta(hours=1)
    while start_date < finish_date:
        # mysql read
        max_sensorvalue = get_max_sensorvalue(splited_rst, equipid, start_date, until_date)

        if max_sensorvalue is None:
            start_date += datetime.timedelta(days=1)
            until_date = start_date + datetime.timedelta(hours=1)
            continue

        date = start_date + datetime.timedelta(minutes=30)

        # mongodb write
        set_sensorvalue_to_mongodb(location, equipname, rst_name, date, max_sensorvalue)

        # console mornitoring
        date = date.strftime('%Y-%m-%d %H:%M:%S')
        print(location + ' ' + equipname + ' ' + rst_name + ' ' + date)

        # next date
        start_date = until_date
        until_date += datetime.timedelta(hours=1)


# 특정 위치, 특정 장비의 rst, 시작날짜, 마지막날짜 정하기
# 
def save_sensorvalue(location, equip_rst_list, equipid, start_date, finish_date):
    global rst_name
    for i in equip_rst_list:
        equipname = i[0]
        rst_list = i[1]
        for j in range(0, len(rst_list)):
            if rst_list[j] is None:
                continue

            # rst_list[j] : rst_mapping_list ex) ['T01_21-T01_22-T02_23...']
            # start date가 "2016-09-22 00:00:00" 인 이유 : 테이블의 시작 날짜.
            else:
                save_sensorvalue_by_datetime(location, equipname, rst_list[j], rst_name[j], equipid, start_date, finish_date)


# importMysqlToMongodb.py 의 시작 함수
# 특정 위치, 검색할 날짜범위를 인자로 데이터를 검색 후 저장.
# 
def save_by_location_and_date(location, start_date, finish_date):
    equipid = get_equipid(location)
    equip_list = get_equip_list(equipid)
    equip_rst_list = []
    for i in equip_list:
        rst = get_rst_mapping_list(equipid, i)
        equip_rst_list.append([i,rst])

    save_sensorvalue(location, equip_rst_list, equipid, start_date, finish_date)


# 해당 위치의 장비목록만 따로 mongodb에 저장
def save_equip_list_to_mongodb(location):
    equipid = get_equipid(location)
    equip_list = get_equip_list(equipid)

    # mongodb collection 이름은 공백문자가 올 수 없기 때문에 공백문자를 '_'로 교체한다.
    # ex) 현대백화점 쇼핑몰동 -> 현대백화점_쇼핑몰동
    location = location_name_change_for_mongodb_collection(location)

    global db
    collection = db["장비목록"]
    collection.insert({"LOCATION" : location, "EQUIP_LIST" : equip_list})


def save_rst_list_to_mongodb(location):
    global db
    global rst_name


    collection = db["RST목록"]
    equipid = get_equipid(location)
    equip_list = get_equip_list(equipid)

    # mongodb collection 이름은 공백문자가 올 수 없기 때문에 공백문자를 '_'로 교체한다.
    # ex) 현대백화점 쇼핑몰동 -> 현대백화점_쇼핑몰동
    location = location_name_change_for_mongodb_collection(location)

    for i in equip_list:
        rst_list = []
        rst = get_rst_mapping_list(equipid, i)
        for j in range(0, len(rst)):
            if rst[j] is None:
                continue
            else:
                rst_list.append(rst_name[j])

    
        collection.insert({"LOCATION" : location, "EQUIPNAME" : i, "RST" : rst_list})


######################################  start  ######################################


# connect mysql
conn_mysql = pymysql.connect(host='125.140.142.56', port=30001, user='pems_cbnu', password='cbnu', db='pems', charset='utf8')
curs = conn_mysql.cursor()

# connect mongodb
conn_mongo = pymongo.MongoClient("localhost", 27017)
db = conn_mongo.db

# rst equip 
rst_name = ["BODY", "R", "S", "T"]



# mongodb collection은 공백문자가 포함되면 안되므로 ' ' -> '_' 로 변경된 이름을 collection명으로 저장해야 한다.
location_list_in_mysql = ["외천초", "길자원", "창무산업", "대성DA", "동청주지사", "한국JCC 2공장", "현대백화점 쇼핑몰동", "현대백화점동", "미래제주"]
location_list_in_mongodb = ["외천초", "길자원", "창무산업", "대성DA", "동청주지사", "한국JCC_2공장", "현대백화점_쇼핑몰동", "현대백화점동", "미래제주"]


# 장비목록 , RST목록 collection
# ex) collection명 : 장비목록 , RST목록
for i in location_list_in_mysql:
    save_equip_list_to_mongodb(i)
    save_rst_list_to_mongodb(i)


# 장비위치 collection
# ex) collection명 : 외천초 , 길자원 , ... 
start_date = "2016-09-22 00:30:00"
finish_date = "2017-03-09 00:30:00"
for i in location_list_in_mysql:
    save_by_location_and_date(i, start_date, finish_date)




conn_mysql.close()
