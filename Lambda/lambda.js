// load modules
var https = require('https');       //LINE APIを利用するためのHTTPSクライアント
var aws = require('aws-sdk');       //S3、Pollyを利用するためのAWS SDK
var endpoint = '******************.iot.ap-northeast-1.amazonaws.com'; //AWS IoTのエンドポイントAWS IoTの画面から確認して置く

exports.handler = function(event, context) {
    console.log('EVENT:', JSON.stringify(event, null, 2));
    
    //AWS IoTの接続準備
    var iotdata = new aws.IotData( { region: 'ap-northeast-1', endpoint: endpoint } );
    var params = {
        topic: 'soracom/cmd/1', // required
        payload: new Buffer('{\"cmd\":1}') || 'STRING_VALUE',
        qos: 0
    }
     var params_off = {
        topic: 'soracom/cmd/1', // required        
        payload: new Buffer('{\"cmd\":0}') || 'STRING_VALUE',
        qos: 0
    }
    var params_timer = {
        topic: '/soracom/cmd/1', // required
        payload: new Buffer('{\"cmd\":2}') || 'STRING_VALUE',
        qos: 0
    }
    //LINE メッセージを変数へ格納
    var event_data = JSON.parse(event.body);
    var reply_token = event_data.events[0].replyToken;
    var message_type = event_data.events[0].message.type;
    var message_text = event_data.events[0].message.text;
    var message_body;
    
    if (message_text == "on") {
        message_body =
            [{
                "type":"text",
                "text": "LED ON"
            }];
        //AWS IoTを介してsoracom deviceがSubしているTopicへMQTT Publishする
        iotdata.publish(params, function(err, data) {
            if (err) console.log("AWS IoT Error:",err, err.stack); // an error occurred
            else     console.log("AWS IoT Success:",data);           // successful response
        });
    } else if (message_text == "off") {
        message_body =
            [{
                "type":"text",
                "text": "LED OFF"
            }];
        //AWS IoTを介してsoracom deviceがSubしているTopicへMQTT Publishする
        iotdata.publish(params_off, function(err, data) {
            if (err) console.log("AWS IoT Error:",err, err.stack); // an error occurred
            else     console.log("AWS IoT Success:",data);           // successful response
        });
    } else if (message_text == "timer") {
        message_body =
            [{
                "type":"text",
                //"text": "タイマー運転中"
            }];
            iotdata.publish(params_timer, function(err, data) {
                if (err) console.log("AWS IoT Error:",err, err.stack); // an error occurred
                else     console.log("AWS IoT Success:",data);           // successful response             
            });
    } else if (message_text == "monitor") {
        message_body =
            [{
                "type":"text",
                //"text":"グラフをみる",

                
            }];         
    } else {
        message_body =
            [{
                "type": "template",
                "altText": "this is a confirm template",
                "template": {
                    "type": "buttons",
                    "text": "点灯させますか？",
                    "actions": [
                        {
                            "type": "message",
                            "label": "点灯",
                            "text": "on"
                        },
                        {
                            "type": "message",
                            "label": "消灯",
                            "text": "off"
                        },   
                        {
                            "type": "message",
                            "label": "タイマー",
                            "text": "timer"
                        },
                        {
                            "type": "uri",
                            "label": "グラフ表示",
                            "uri": "https://jp.lagoon.soracom.io/"
                        },
                    ]
                }
            }]
    }
            
    // LINE への受け付けましたメッセージRequest Body
    var request_body = JSON.stringify({
        replyToken: reply_token,
        messages: message_body
    });
    // LINE への受け付けましたメッセージRequest Headers
    var send_options = {
        host: 'api.line.me',
        path: '/v2/bot/message/reply',
        headers: {
            "Content-type": "application/json; charset=UTF-8",
            "Authorization": " Bearer " + "*********************************" //ご自身のLINE APIアクセストークンを入力
        },
        method: 'POST'
    };
    // LINE への受け付けましたメッセージAPIリクエスト
    var req = https.request(send_options, function(res){
        res.on('request_body', function (chunk) {
            console.log('OK: ' + res.statusCode + chunk.toString());
        });
        req.on('error', function(err) {
            console.log('ERROR: ' + err.message);
        });
    });
    req.write(request_body);
    req.end();

};
