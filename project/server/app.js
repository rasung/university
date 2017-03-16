var express = require('express');
var app = express();
var bodyParser = require('body-parser');
var MongoClient = require('mongodb').MongoClient;
var url = 'mongodb://localhost:27017/db';

var location_list = ["외천초", "길자원", "창무산업", "대성DA", "동청주지사", "한국JCC_2공장", "현대백화점_쇼핑몰동", "현대백화점동", "미래제주"]
var all_equip_data_structure = ["DATETIME", "외천초", "길자원", "창무산업", "대성DA", "동청주지사", "한국JCC_2공장", "현대백화점_쇼핑몰동", "현대백화점동", "미래제주"];
var equip_list = {  
                    "외천초" : ["HEAD", "ASS", "TR", "MOF"],
                    "길자원" : ["TR", "MOF", "ASS"],
                    "창무산업" : ["LA", "MOF", "TR", "TR2"],
                    "대성DA" : ["TR", "ASS", "MOF"], 
                    "동청주지사" : ["ASS", "LA", "COS/PF", "MOF", "TR"],
                    "한국JCC_2공장" : ["TR", "TR2", "TR3", "MOF", "HEAD"],
                    "현대백화점_쇼핑몰동" : ["LA", "LBS", "HEAD", "VCB", "MOF", "HEAD2", "HEAD3"],
                    "현대백화점동" : ["LBS", "LA", "VCB", "MOF", "HEAD"],
                    "미래제주" : ["MOF", "TR", "ASS"]
                }

// Todo   mongodb 에서 특정 collection에서 특정 날짜 범위의 값 가져오는 함수 생성하기
// 입력 : collection 이름, 시작날짜, 마지막날짜

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
    extended : true
}));


app.get('/', function (req, res) {
  res.sendFile('C:/Users/rlxotjr/Desktop/server/index.html');
});



app.post('/all_equip', function(req, res){
    var select = req.body.select;
    var year = req.body.year;
    var month = req.body.month;
    var day = req.body.day;

    if(select == 'hour')
    {
        var start_date = new Date(year, month, day, 0, 0, 0);
        var finish_date = new Date(year, month, day, 23, 0, 0);

        MongoClient.connect(url, function(err, db){
            var collection = db.collection("date_and_sensorvalue");
            collection.find({"DATETIME" : { "$gte" : start_date, "$lte" : finish_date}}).sort({"DATETIME" : 1}).toArray(function (err, result) {
                var date_and_sensorvalue = [];
                date_and_sensorvalue.push(all_equip_data_structure);
                for(var i=0; i<24; i++)
                {
                    if (result[0] == undefined)
                    {
                        break;
                    }
                    else
                    {
                        date_and_sensorvalue.push(result[i]["date_and_sensorvalue"]);
                    }
                }
                res.send(date_and_sensorvalue);
            });
            db.close();
        });
    }
    else if(select == 'day')
    {
        var start_date = new Date(year, month, day, 0, 0, 0);
        var finish_date = new Date(year, month, day, 0, 0, 0);

        // 특정일로부터 전후3일 총 1주일 범위를 그래프로 보여준다.
        start_date.setDate(start_date.getDate() - 4);
        finish_date.setDate(finish_date.getDate() + 3);

        MongoClient.connect(url, function(err, db){
            var collection = db.collection("date_and_sensorvalue_days");
            collection.find({"DATETIME" : { "$gte" : start_date, "$lte" : finish_date}}).sort({"DATETIME" : 1}).toArray(function (err, result) {
                var date_and_sensorvalue = [];
                date_and_sensorvalue.push(all_equip_data_structure);
                for(var i=0; i<7; i++)
                {
                    if (result[0] == undefined)
                    {
                        break;
                    }
                    else
                    {
                        date_and_sensorvalue.push(result[i]["date_and_sensorvalue"]);
                    }
                }
                res.send(date_and_sensorvalue);
            });
            db.close();
        });
    }
    else if(select == 'month')
    {
        // 특정일이 포함된 년도의 전체 월간 정보를 그래프로 보여준다.
        var start_date = new Date(year, 1, 1, 0, 0, 0);
        var finish_date = new Date(year, 12, 1, 0, 0, 0);

        MongoClient.connect(url, function(err, db){
            var collection = db.collection("date_and_sensorvalue_months");
            collection.find({"DATETIME" : { "$gte" : start_date, "$lte" : finish_date}}).sort({"DATETIME" : 1}).toArray(function (err, result) {
                var date_and_sensorvalue = [];
                date_and_sensorvalue.push(all_equip_data_structure);
                for(var i=0; i<result.length; i++)
                {
                    if (result[0] == undefined)
                    {
                        break;
                    }
                    else
                    {

                        date_and_sensorvalue.push(result[i]["date_and_sensorvalue"]);
                    }
                }
                res.send(date_and_sensorvalue);
            });
            db.close();
        });
    }
});

app.post('/one_equip', function(req, res){
    var select = req.body.select;
    var year = req.body.year;
    var month = req.body.month;
    var day = req.body.day;

    var start_date = new Date(year, month, day, 0, 0, 0);
    var finish_date = new Date(year, month, day, 23, 0, 0);
    
    MongoClient.connect(url, function(err, db){
        var collection = db.collection("date_and_sensorvalue_for_one_equip." + select);
        collection.find({"DATETIME" : { "$gte" : start_date, "$lte" : finish_date}}).sort({"DATETIME" : 1}).toArray(function (err, result) {
            var date_and_sensorvalue = [];
            var format = ["DATETIME"];
            var equip = equip_list[select];
            for (var i=0; i<equip.length; i++)
            {
                format.push(equip[i]);
            }

            date_and_sensorvalue.push(format);
            console.log(format);
            for(var i=0; i<result.length; i++)
            {
                if (result[0] == undefined)
                {
                    break;
                }
                else
                {
                    date_and_sensorvalue.push(result[i]["date_and_sensorvalue"]);
                }
            }
            console.log(date_and_sensorvalue)
            res.send(date_and_sensorvalue);
        });
        db.close();

    });

});
       


        
    

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});

