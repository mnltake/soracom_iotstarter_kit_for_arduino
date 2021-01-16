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

soracom バイナリーパーサー
https://dev.soracom.io/jp/docs/binary_parser/

## 出力例

AT+QISENDEX=0,"06a41c2003c8000102e101229f01a7"

(send ATcomand to modemUART)

temp:0:int:16:/100 hum:2:int:16:/100 press:4:int:16:7:+100000/100 uptime:6:uint:16 light:8:int:16 sound:10:int:16 dispflg:12:bool:7 movex:12:bool:6 movey:13:bool:5 movez:12:bool:4 volume:13:int:16

(binary-parser)

[
  {
    "temp": 17,
    "hum": 72,
    "press": 1009.68,
    "uptime": 1,
    "light": 737,
    "sound": 290,
    "dispflg": true,
    "movex": false,
    "movey": false,
    "movez": true,
    "volume": 423
  }
]
