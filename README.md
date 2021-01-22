## soracom_iotstarter_kit_for_arduino 

sample 

IoT スターターキット for Arduino
https://soracom.jp/store/5243/

LTE-M Shield for Arduino
https://soracom.jp/store/5303/

IoT スターターキット for Arduino をセットアップする
https://dev.soracom.io/jp/start/iotstarter_kit_for_arduino/

Grove Beginner Kit For Arduino
https://wiki.seeedstudio.com/Grove-Beginner-Kit-For-Arduino/

NB-IoT Shield wiki
http://wiki.dragino.com/index.php?title=NB-IoT_Shield#BG96_example

dragino/NB-IoT github
https://github.com/dragino/NB-IoT

BG96 AT Commands Manual
https://www.quectel.com/UploadImage/Downlad/Quectel_BG96_AT_Commands_Manual_V2.1.pdf

BG96 TCP/IP AT Commands Manual
https://www.quectel.com/UploadImage/Downlad/Quectel_BG96_TCP(IP)_AT_Commands_Manual_V1.0.pdf

BG96_MQTT_Application_Note_V1.1
https://www.quectel.com/UploadImage/Downlad/Quectel_BG96_MQTT_Application_Note_V1.1.pdf

soracom バイナリーパーサー
https://dev.soracom.io/jp/docs/binary_parser/

## 出力例

AT+QISENDEX=0,"06a4132405cc001a02e300ac87002a"

(send ATcomand to modemUART)

temp:0:int:16:/100 hum:2:int:16:/100 press:4:int:16:7:+100000/100 upcount:6:uint:16 light:8:int:16 sound:10:int:16 dispflg:12:bool:7 movex:12:bool:6 movey:13:bool:5 movez:12:bool:4 ledflg:12:bool:3 volume:13:int:16

(binary-parser)

[[
  {
    "temp": 17,
    "hum": 49,
    "press": 1014.84,
    "upcount": 26,
    "light": 739,
    "sound": 172,
    "dispflg": true,
    "movex": false,
    "movey": false,
    "movez": false,
    "ledflg": false,
    "volume": 42
  }
]

soracom device ->(binary data) -> soracom Unified Endpoint->(bianary parser)

　　　　　　　　-> soracom Harvest -> soracom Lagoon -> LINE alart

　　　　　　　　-> soracom funnnel -> AWS IoT core(MQTT Publish) -> AWS SNS ,AWS DynamoDB...

soracom device(MQTT Subscribe) <- soracom beam <- AWS IoT core <- AWS Lambda <- Amazone API Gateway <- LINE Messaging API 
