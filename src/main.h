#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <dht.h>
#include <Seeed_BMP280.h>
#include <LIS3DHTR.h>
#include <Adafruit_SleepyDog.h>
#include <U8x8lib.h>
#define BAUD 9600
#define DHT11_PIN 3
#define LEDPIN 4
#define RESETPIN A1
#define BUZZERPIN 5
#define LIGHTSENSORPIN A6
#define SOUNDSENSORPIN A2
#define VOLUMEPIN 0
#define BUTTONPIN 6
#define bg96_rx  10
#define bg96_tx  11
SoftwareSerial bg96_serial(bg96_rx, bg96_tx);
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#define U8X8_ENABLE_180_DEGREE_ROTATION 1
LIS3DHTR<TwoWire> LIS;
BMP280 bmp280;
dht DHT;
//parameter
bool dispflg =true,movex=false,movey=false,movez=false;
float tem,x, y, z;   
unsigned int uptime,upcount=1,soundSamplingCount=5,accelSamplingCount=5;   
int light,sound,hum,temperature,humidity,volume;
long press;
int sendbyte=0x0000;
/******define software Serial port*******/

/******define the udp parameter const*******/
const char UDP_server[] = "uni.soracom.io";
const int UDP_port = 23080;
/******define the tcp parameter const*******/
const char TCP_server[] = "uni.soracom.io";
const int TCP_port = 23080;

static char return_dat[256] = {"\0"};   //save the UE return all data
void bg96_initial();
void bg96_serial_read();
void bg96_serial_clearbuf();
void tcp_config();
void start_tcp();
void tcp_protocal();
void tcp_send_dat();
void tcp_receive_dat();
void tcp_close();
void snprintf_fun1();
void bg96_at_cmd();
void bg96_ate_cmd();/* Echo mode*/
void bg96_at_cimi();/* International Mobile ID and Serial Number*/
void bg96_at_csq();/* Signal Quality*/
void bg96_at_cereg();/*EPS Network Registration Status*/
void bg96_at_cgpaddr();/*IP Address*/
void udp_protocal();
void snprintf_fun1();
void start_udp();
void udp_hardware();
void udp_send();
void udp_recieve();
void udp_close();
void oled_write(int i);



