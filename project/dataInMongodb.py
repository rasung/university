import pymongo
import datetime
# date += relativedelta(months=1). month 연산기능은 datetime에 없다.
from dateutil.relativedelta import relativedelta

######################################  functions  ######################################


# 특정 시간, 특정 위치의 모든 장비들의 body, r, s, t 값들 중에서 최대값만 걸러내기. 
# db 컬렉션(특정위치이름)에 저장되는 값은 장비이름, 날짜, 최대온도값. 
# rstMaxValue.장소 컬렉션에 저장
# 관측 단위 시간 : 1시간 간격
# 관측 단위 장소 : 해당 장소 하나
# 관측 단위 장비 : 장비 정보 있음. ex) HEAD, TR ...
# 관측 단위 세부장비(rst) : rst들의 max값만 저장함으로 rst 정보 없음. ex) body, r, s, t
# 컬렉션 효용 : 특정 장소에 있는 장비들의 온도값을 알 수 있음. save_max_value_for_hour_graph(), 를 위한 단계.
def save_max_value_among_rst(location, start_date, finish_date):
    global db
    equip_list = get_equip_list(location)
    # date type change
    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')
    for i in equip_list:
        date = start_date
        rst_list = get_rst_list(location, i)

        # ex) 컬렉션명 : maxRST.외천초
        collection_name = "rstMaxValue" + '.' + location

        while date < finish_date:
            max_sensorvalue = 0;
            for j in rst_list:            
                result = db[location].find_one({"EQUIPNAME" : i, "DATETIME" : date, "RST" : j})
                if result is None:
                    continue;
                else:
                    if max_sensorvalue < result['MAX_SENSORVALUE']:
                        max_sensorvalue = result['MAX_SENSORVALUE']
            
            db[collection_name].insert({"EQUIPNAME" : i, "DATETIME" : date, "maxSensorValue" : max_sensorvalue})

            # console mornitoring
            saved = db[collection_name].find_one({"EQUIPNAME" : i, "DATETIME" : date})
            EQUIPNAME = saved["EQUIPNAME"]
            DATETIME = datetime.datetime.strftime(saved["DATETIME"], '%Y-%m-%d %H:%M:%S')
            MAXSENSORVALUE = str(saved["maxSensorValue"])
            print(location, EQUIPNAME, DATETIME, MAXSENSORVALUE)

            date += datetime.timedelta(hours=1)





def get_equip_list(location):
    global db
    collection = db["장비목록"]
    equip_list = collection.find({"LOCATION" : location})[0]["EQUIP_LIST"]
    return equip_list

        
def get_rst_list(location, equipname):
    global db
    collection = db["RST목록"]
    rst_list = collection.find({"LOCATION" : location, "EQUIPNAME" : equipname})[0]["RST"]
    return rst_list
    

# equipMaxValue.장소 컬렉션에 저장
# 관측 단위 시간 : 1시간 간격
# 관측 단위 장소 : 해당 장소 하나
# 관측 단위 장비 : 장비들의 max값만 저장함으로 장비 정보 없음 . ex) HEAD, TR ...
# 관측 단위 세부장비(rst) : rst 정보 없음. ex) body, r, s, t
# 컬렉션 효용 : save_max_value_for_hour_graph()를 위한 단계.
def save_max_value_among_equip(location, start_date, finish_date):
    global db
    collection_rstMaxValue = 'rstMaxValue.' + location
    collection_equipMaxValue = 'equipMaxValue.' + location

    # date type change
    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')
    date = start_date

    equip_list = get_equip_list(location)
    while date < finish_date:
        max_sensorvalue = 0;
        for i in equip_list:
            result = db[collection_rstMaxValue].find_one({"EQUIPNAME" : i, "DATETIME" : date})
            if max_sensorvalue < result['maxSensorValue']:
                max_sensorvalue = result['maxSensorValue']

        db[collection_equipMaxValue].insert({"DATETIME" : date, "maxSensorValue" : max_sensorvalue})

        # console mornitoring
        saved = db[collection_equipMaxValue].find_one({"DATETIME" : date})
        DATETIME = datetime.datetime.strftime(saved["DATETIME"], '%Y-%m-%d %H:%M:%S')
        MAXSENSORVALUE = str(saved["maxSensorValue"])
        print(location, DATETIME, MAXSENSORVALUE)

        date += datetime.timedelta(hours=1)


# date_and_sensorvalue 컬렉션에 저장
# 컬렉션 구조 : ["DATETIME" : 날짜, "date_and_sensorvalue" : [날짜,외천초값,길자원값,...,미래제주값]]
# 컬렉션 효용 : 모든 장소의 온도 값을 1시간 간격으로 비교하는 그래프에 넣는 데이터로 사용됨. + save_max_value_for_day_graph()를 위한 중간 데이터 생성
def save_max_value_for_hour_graph(start_date, finish_date, location_list):
    global db
    # date type change
    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')
    date = start_date
    date_and_sensorvalue_info = ["DATETIME"] + location_list
    # date_and_sensorvalue_list 스키마 정보 저장
    db["date_and_sensorvalue"].insert({"DATETIME" : "ISODate('2000-00-00 T00:00:00Z')", "date_and_sensorvalue" : date_and_sensorvalue_info })

    while date < finish_date:
        date_and_sensorvalue_list = [date]
        for i in location_list:
            collection_equipMaxValue = "equipMaxValue." + i;
            result = db[collection_equipMaxValue].find_one({"DATETIME" : date})
            sensorvalue = result["maxSensorValue"]
            date_and_sensorvalue_list.append(sensorvalue)
            
        db["date_and_sensorvalue"].insert({"DATETIME" : date, "date_and_sensorvalue" : date_and_sensorvalue_list})

        # console mornitoring
        print(date, date_and_sensorvalue_list)

        date += datetime.timedelta(hours=1)
    

# date_and_sensorvalue_day 컬렉션에 저장
# 컬렉션 구조 : ["DATETIME" : 날짜, "date_and_sensorvalue" : [날짜,외천초값,길자원값,...,미래제주값]]
# 컬렉션 효용 : 모든 장소의 온도 값을 1일 간격으로 비교하는 그래프에 넣는 데이터로 사용됨.
def save_max_value_for_day_graph(start_date, finish_date, location_list):
    global db
    # date type change
    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')
    date = start_date
    date_and_sensorvalue_info = ["DATETIME"] + location_list

    # date_and_sensorvalue_list 스키마 정보 저장
    db["date_and_sensorvalue_days"].insert({"DATETIME" : "ISODate('2000-00-00 T00:00:00Z')", "date_and_sensorvalue" : date_and_sensorvalue_info })

    while date < finish_date:
        date_and_sensorvalue_list = [date]
        for i in location_list:
            temp_date = date
            temp_sensorvalue = 0;
            collection_equipMaxValue = 'equipMaxValue.' + i
            for j in range(0, 24):
                result = db[collection_equipMaxValue].find_one({"DATETIME" : temp_date})
                sensorvalue = result["maxSensorValue"]
                if temp_sensorvalue < sensorvalue:
                    temp_sensorvalue = sensorvalue
                temp_date += datetime.timedelta(hours=1)

            date_and_sensorvalue_list.append(temp_sensorvalue)
            
        db["date_and_sensorvalue_days"].insert({"DATETIME" : date, "date_and_sensorvalue" : date_and_sensorvalue_list})

        # console mornitoring
        print(date, date_and_sensorvalue_list)

        date += datetime.timedelta(days=1)
    




# date_and_sensorvalue_month 컬렉션에 저장
# 컬렉션 구조 : ["DATETIME" : 날짜, "date_and_sensorvalue" : [날짜,외천초값,길자원값,...,미래제주값]]
# 컬렉션 효용 : 모든 장소의 온도 값을 1달 간격으로 비교하는 그래프에 넣는 데이터로 사용됨.
def save_max_value_for_month_graph(start_date, finish_date, location_list):
    global db
    # date type change
    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')
    date = start_date
    date_and_sensorvalue_info = ["DATETIME"] + location_list

    # date_and_sensorvalue_list 스키마 정보 저장
    db["date_and_sensorvalue_months"].insert({"DATETIME" : "ISODate('2000-00-00 T00:00:00Z')", "date_and_sensorvalue" : date_and_sensorvalue_info })

    while date < finish_date:
        date_and_sensorvalue_list = [date]
        for i in location_list:
            temp_date = date
            temp_sensorvalue = 0;
            collection_equipMaxValue = 'equipMaxValue.' + i
            month = temp_date.month
            k=0
            while temp_date.month is month:
                k += 1
                result = db[collection_equipMaxValue].find_one({"DATETIME" : temp_date})
                if result is not None:
                    sensorvalue = result["maxSensorValue"]
                    if temp_sensorvalue < sensorvalue:
                        temp_sensorvalue = sensorvalue
                temp_date += datetime.timedelta(hours=1)

            date_and_sensorvalue_list.append(temp_sensorvalue)
            
        db["date_and_sensorvalue_months"].insert({"DATETIME" : date, "date_and_sensorvalue" : date_and_sensorvalue_list})

        # console mornitoring
        print(date, date_and_sensorvalue_list)

        date += relativedelta(months=1)
    
def save_max_value_for_oneEquip(location, start_date, finish_date):
    global db

    # date type change
    start_date = datetime.datetime.strptime(start_date, '%Y-%m-%d %H:%M:%S')
    finish_date = datetime.datetime.strptime(finish_date, '%Y-%m-%d %H:%M:%S')

    for i in location:
        equip_list = get_equip_list(i)
        date = start_date
        collection_rstMaxValue = "rstMaxValue" + '.' + i
        collection_name = "date_and_sensorvalue_for_one_equip." + i
        while date < finish_date:
            temp_date_sensorvalue = [date];
            for j in equip_list:
                result = db[collection_rstMaxValue].find_one({"DATETIME" : date, "EQUIPNAME" : j})
                temp_date_sensorvalue.append(result['maxSensorValue'])
            
            db[collection_name].insert({"DATETIME" : date, "date_and_sensorvalue" : temp_date_sensorvalue})

            # console mornitoring
            saved = db[collection_name].find_one({"DATETIME" : date})
            DATETIME = datetime.datetime.strftime(saved["DATETIME"], '%Y-%m-%d %H:%M:%S')
            DATE_AND_SENSORVALUE = str(saved["date_and_sensorvalue"])
            print(i, DATETIME, DATE_AND_SENSORVALUE)

            date += datetime.timedelta(hours=1)
    


######################################  start  ######################################



# connect mongodb
conn_mongo = pymongo.MongoClient("localhost", 27017)
db = conn_mongo.db

# mongodb collection은 공백문자가 포함되면 안되므로 ' ' -> '_' 로 변경된 이름으로 저장하기로 약속하였기 때문에.
# 공백문자가 포함된 장비위치는 없다. 장비위치(location)에 공백문자 대신 '_' 값이 들어간 이름을 입력해야 한다.
# ex) 한국JCC 2공장 -> 한국JCC_2공장
location_list_in_mongodb = ["외천초", "길자원", "창무산업", "대성DA", "동청주지사", "한국JCC_2공장", "현대백화점_쇼핑몰동", "현대백화점동", "미래제주"]


# rst equip 
rst_name = ["BODY", "R", "S", "T"]

start_date = "2016-09-22 00:00:00"
finish_date = "2017-03-09 00:00:00"

for i in location_list_in_mongodb:
    save_max_value_among_rst(i, start_date, finish_date)

for i in location_list_in_mongodb:
    save_max_value_among_equip(i, start_date, finish_date)

save_max_value_for_hour_graph(start_date, finish_date, location_list_in_mongodb)

save_max_value_for_day_graph(start_date, finish_date, location_list_in_mongodb)

# 월간 단위 라서 01일로 지정해줘야 한다.
save_max_value_for_month_graph("2016-09-01 00:00:00", "2017-04-01 00:00:00", location_list_in_mongodb)

save_max_value_for_oneEquip(location_list_in_mongodb, start_date, finish_date)