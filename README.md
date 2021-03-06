## soracom_iotstarter_kit_for_arduino 

sample 

IoT スターターキット for Arduino
https://soracom.jp/store/5243/

IoTオンライン講座 | Arduinoとセンサーで屋内環境モニタリングシステムを作ろう
https://youtu.be/T_AnmgM_1t4

soracom公式サンプル集
https://github.com/soracom-labs/arduino-dragino-unified

LTE-M Shield for Arduino
https://soracom.jp/store/5303/

IoT スターターキット for Arduino をセットアップする
https://dev.soracom.io/jp/start/iotstarter_kit_for_arduino/

Grove Beginner Kit For Arduino
https://wiki.seeedstudio.com/Grove-Beginner-Kit-For-Arduino/

Arduino_SensorKit library
https://github.com/arduino-libraries/Arduino_SensorKit

NB-IoT Shield wiki
http://wiki.dragino.com/index.php?title=NB-IoT_Shield#BG96_example

dragino/NB-IoT github
https://github.com/dragino/NB-IoT

回路図
https://github.com/dragino/NB-IoT/tree/master/NB-IoT%20Shield/hardware/IoT%20Shield/v1.2

BG96 AT Commands Manual
https://www.quectel.com/UploadImage/Downlad/Quectel_BG96_AT_Commands_Manual_V2.1.pdf

BG96 TCP/IP AT Commands Manual
https://www.quectel.com/UploadImage/Downlad/Quectel_BG96_TCP(IP)_AT_Commands_Manual_V1.0.pdf

BG96_MQTT_Application_Note_V1.1
https://www.quectel.com/UploadImage/Downlad/Quectel_BG96_MQTT_Application_Note_V1.1.pdf

soracom バイナリーパーサー
https://dev.soracom.io/jp/docs/binary_parser/

## プレゼン資料

https://www.slideshare.net/secret/fsarRga89yyD0b

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
## Stream

![steam](https://github.com/mnltake/soracom_iotstarter_kit_for_arduino/blob/main/steam.jpg)
soracom device ->TCP(binary data) -> soracom Unified Endpoint->(bianary parser)

　　　　　　　　-> soracom HarvestData(database) -> soracom Lagoon(dashboard) -> ALART (LINE ,Slack,Email...)

　　　　　　　　-> soracom funnnel(MQTT Publish)  -> AWS IoT core-> AWS SNS ,AWS DynamoDB...

soracom device(MQTT Subscribe) <- soracom Beam <- AWS IoT core <- AWS Lambda <- Amazone API Gateway <- LINE Messaging API 
