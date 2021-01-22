/*
soracom device ->(binary data) -> soracom Unified Endpoint

->(bianary parser)
temp:0:int:16:/100 hum:2:int:16:/100 press:4:int:16:7:+100000/100 upcount:6:uint:16 light:8:int:16 
sound:10:int:16 dispflg:12:bool:7 movex:12:bool:6 movey:13:bool:5 movez:12:bool:4 ledflg:12:bool:3 volume:13:int:16

-> soracom Harvest -> soracom Lagoon -> LINE alart

-> soracom funnnel -> AWS IoT core(MQTT Publish) -> AWS SNS ,AWS DynamoDB...

soracom device(MQTT Subscribe) <- soracom beam <- AWS IoT core <- AWS Lambda <- Amazone API Gateway <- LINE Messaging API 

*/
#include <Arduino.h>

#define BG96_BAUD 9600
#define USB_BAUD 9600
#define VOLUMEPIN A0
#define BG96_RESETPIN A1
#define SOUNDSENSORPIN A2
#define LIGHTSENSORPIN A6
#define DHT11_PIN 3
#define LEDPIN 4
#define BUZZERPIN 5
#define BUTTONPIN 6
#define LED_BUILTIN 13

//BG96 UART
#include <SoftwareSerial.h>
#define BG96_RX  10
#define BG96_TX  11
SoftwareSerial bg96_serial(BG96_RX, BG96_TX);

//OLED
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1
/*
//3-axis Acceleration
#include <LIS3DHTR.h>
LIS3DHTR<TwoWire> LIS;
int accelSamplingCount=5;float x,y,z;
*/
//Temprerature Humidity
#include <dht.h>
dht DHT;
float tem; int hum;  

//Air Pressure
#include <Seeed_BMP280.h>
BMP280 bmp280;
long press;

//MQTT subscribe command
#include <ArduinoJson.h>
StaticJsonDocument<20> doc;
int subcmd=0;


//sllep watchdog timer
#include <Adafruit_SleepyDog.h>

//analog and bit parameter
bool dispflg =true,movex=false,movey=false,movez=false,ledflg=false;
unsigned int upcount=1;
int light,sound,volume,soundSamplingCount=5;

/******define the tcp parameter const*******/
const char TCP_server[] = "uni.soracom.io";//soracom Unified Endpoint
const int TCP_port = 23080;
static char return_dat[256] = {"\0"};   //save  return all data

void bg96_initial();
void bg96_serial_read();
void bg96_serial_clearbuf();
void tcp_config();
void start_tcp();
void tcp_protocal();
void tcp_send_dat();
void tcp_receive_dat();
void tcp_close();
void mqtt_config();
void mqtt_close();
void topic_read();
void bg96_at_cmd();
void bg96_ate_cmd();/* Echo mode*/
void bg96_at_cimi();/* International Mobile ID and Serial Number*/
void bg96_at_csq();/* Signal Quality*/
void bg96_at_cereg();/*EPS Network Registration Status*/
void bg96_at_cgpaddr();/*IP Address*/
void oled_write(int i);