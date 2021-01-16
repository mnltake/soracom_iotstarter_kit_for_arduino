
#include "main.h"

void setup() {
  pinMode(LIGHTSENSORPIN, INPUT);
  pinMode(SOUNDSENSORPIN, INPUT);
  pinMode(VOLUMEPIN, INPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(RESETPIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH);//modem reset
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.println("Start:");
  u8x8.println("Please wait");
  delay(100);
  digitalWrite(RESETPIN, LOW);
  pinMode(LEDPIN, OUTPUT);//LED ON
  digitalWrite(LEDPIN, HIGH);
  pinMode(13, OUTPUT);//board LED
  tone(BUZZERPIN ,440);//beep
  bg96_initial();
  bmp280.init();
  oled_write(0);
  LIS.begin(Wire, 0x19);
  delay(100);
  LIS.setFullScaleRange(LIS3DHTR_RANGE_8G);
  LIS.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);
  LIS.setHighSolution(true); 
}

void loop() {
  Watchdog.enable(8000);
  noTone(BUZZERPIN);
  digitalWrite(LEDPIN, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  /* read sensor*/
  light= analogRead(LIGHTSENSORPIN);
  Serial.println("light sensor is:");
  Serial.println(light);                

  long sum = 0;
  for (int i = 0; i < soundSamplingCount; i++) {
    sum += analogRead(SOUNDSENSORPIN);
    delay(5);
  }
  sound = sum / soundSamplingCount;
  Serial.println("Sound sensor is:");
  Serial.println(sound);                 

  temperature = DHT.read11(DHT11_PIN); 
  tem = DHT.temperature*1.0;     //float
  Serial.println("the Tmperature is:");
  Serial.println(tem);               
  
  humidity = DHT.read11(DHT11_PIN);      //Read humidity data
  hum = DHT.humidity* 1.0;       //int
  Serial.println("Humidity is:");
  Serial.println(hum);      

  press = bmp280.getPressure() ;
  Serial.println("AirPress is:");
  Serial.println(press);             //Pa

  float sumx=0,sumy=0,sumz=0;
  for (int i = 0; i < accelSamplingCount; i++) {
    sumx += LIS.getAccelerationX();
    sumy += LIS.getAccelerationY();
    sumz += LIS.getAccelerationZ();
    delay(5);
  }
  x = sumx / accelSamplingCount;
  y = sumy / accelSamplingCount;
  z = sumz / accelSamplingCount;
  Serial.println("Acceleratin is:");
  Serial.print("x:"); Serial.print(x); Serial.print("  ");
  Serial.print("y:"); Serial.print(y); Serial.print("  ");
  Serial.print("z:"); Serial.println(z);
  movex=(abs(x) >0.5)?true:false;
  movey=(abs(y) >0.5)?true:false;
  movez=(abs(z) >0.5)?true:false;

  volume=analogRead(VOLUMEPIN);
  Serial.println("Volume is:");
  Serial.println(volume);     
    
  Serial.println("upcount is:");
  Serial.println(upcount);                 
  
  //start send data
  digitalWrite(LEDPIN, HIGH);
  sendbyte=bitWrite(sendbyte,7,dispflg);
  sendbyte=bitWrite(sendbyte,6,movex);
  sendbyte=bitWrite(sendbyte,5,movey);
  sendbyte=bitWrite(sendbyte,4,movez);
  sendbyte=bitWrite(sendbyte,3,1);
  sendbyte=bitWrite(sendbyte,2,1);
  sendbyte=bitWrite(sendbyte,1,1);
  sendbyte=bitWrite(sendbyte,0,1);
  tcp_protocal();
  //udp_protocal(); /*NOT MOVE AT+QISENDEX Send Hex String command cannot be applied for “UDP SERVICE”*/
  if(strstr(return_dat,"OK") == NULL && strstr(return_dat,"ok") == NULL){ 
     tone(BUZZERPIN ,440);//ERROR beep
     delay(8000);} //WDT restart
  digitalWrite(LEDPIN, LOW);

  //sleep
  digitalWrite(LED_BUILTIN, LOW);
  for(int i=0 ;i<14;i++){
   
    if (digitalRead(BUTTONPIN)==HIGH){
      dispflg = !dispflg;
    }
    if (dispflg){
      oled_write(i);
    }else{
      u8x8.clear();
    }
    
    Watchdog.reset();
    Watchdog.sleep(4000);
  }
  upcount++;
}
void oled_write(int i){
  u8x8.clear();
  u8x8.setCursor(0, 1);
  switch (i%7){
    case 0:
    if(strstr(return_dat,"OK")!=NULL){
      u8x8.println("return:");
      u8x8.println();
      u8x8.println("OK");
    }else if(strstr(return_dat,"ERROR")!=NULL){
      u8x8.println("return:");
      u8x8.println();
      Serial.println("ERROR");
    }
      return;
    case 1:
      u8x8.println("Temperture:");
      u8x8.println();
      u8x8.println(tem);
      return;
    case 2:
      u8x8.println("Humidity:");
      u8x8.println();
      u8x8.println(hum);
      return;
    case 3:
      u8x8.println("Air Pressure:");
      u8x8.println();
      u8x8.println(press);
      return;
    case 4:
      u8x8.println("Light:");
      u8x8.println();
      u8x8.println(light);
      return;
    case 5:
      u8x8.println("Sound:");
      u8x8.println();
      u8x8.println(sound);
      return;
    case 6:
      u8x8.println("Up Count:");
      u8x8.println(upcount);
      u8x8.print(int(i/7)+1);
      u8x8.println("/2");
      return;
  }
return;
}
void tcp_send_dat(){
  int tem_x100,hum_x100;
  tem_x100 = (int)(tem*100);
  hum_x100 = hum*100;
  int press_c = (int)(press-100000);
  char cmd[64]={"0"};
  snprintf(cmd,sizeof(cmd),"AT+QISENDEX=0,\"%04x%04x%04x%04x%04x%04x%02x04x\"",tem_x100,hum_x100,press_c,upcount,light,sound,sendbyte,volume);
  bg96_serial.println(cmd);
  Serial.println("println the tcp send data");
  Serial.println(cmd);
}

void bg96_initial(){
  delay(1000);
  Serial.begin(BAUD);            
  bg96_serial.begin(BAUD);                 //open the Serial ,BAUD is 9600bps
  Serial.println("start the serial port,BAUD is 9600bps.");
  delay(500);
  Watchdog.reset();

  bg96_at_cmd();
  oled_write(0);
  /*
  delay(500);
  bg96_ate_cmd();
  delay(500);
  bg96_at_cimi();
  bg96_at_csq();
  delay(500);
  bg96_at_cereg();
  delay(500);
  */
  bg96_at_cgpaddr();
  delay(500);
  Watchdog.reset();
  //tcp_config();
  tcp_close();
  delay(500);
  tcp_config();
  }


void bg96_serial_read()
{
  int i = 0;
  memset(return_dat, 0, sizeof(return_dat));
  bg96_serial.flush();
  delay(2000);
  while (bg96_serial.available() && i < (int)sizeof(return_dat) - 1)
  {
    return_dat[i] = bg96_serial.read();
    i = i + 1;
  }
  return_dat[i] = '\0';
  Serial.println(return_dat);
}

void bg96_serial_clearbuf()
{
  bg96_serial.flush();

  while (bg96_serial.available()) {
    bg96_serial.read();
  }
}
/********tcp config*********/
void tcp_config(){
  start_tcp();
  }
void start_tcp(){
  char cmd[64]={"0"};
  snprintf(cmd,sizeof(cmd),"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0",TCP_server,TCP_port);//TCP cliant Buffer access mode
  while(1){
  bg96_serial_clearbuf();
  Serial.println(cmd);
  bg96_serial.println(cmd);
  bg96_serial_read();
  if(strstr(return_dat,"OK")!=NULL){
    Serial.println("the tcp already connect");
    break;}
  }
  }
/********tcp protocol*********/
void tcp_protocal()
{
  tcp_send_dat();
  delay(3000);
  tcp_receive_dat();
  }
void tcp_close()
{
  Serial.println("close tcp server.");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QICLOSE=0");
  bg96_serial_read();
  }

void tcp_receive_dat()
{
  Serial.println("receive TCP data:");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QIRD=0,1500");
  bg96_serial_read();
  }

 void bg96_at_cmd(){
  Serial.println("\n");
  Serial.println("send the at command,wait for return \"OK\"");
  while(1){
    bg96_serial_clearbuf();//clear the return data buffer
    bg96_serial.println("AT");
    bg96_serial_read();
    if(strstr(return_dat,"OK") != NULL){
      Serial.println("<----------------------->");
      break;}
    else 
      delay(1000);
    }}

 void bg96_ate_cmd(){
  Serial.println("close the return display");
  while(1){
    bg96_serial_clearbuf();//clear the return data buffer
    bg96_serial.println("ATE0");
    bg96_serial_read();
    if(strstr(return_dat,"OK") != NULL){
      Serial.println("<----------------------->");
      break;}
    else 
      delay(1000);
    }}  

void bg96_at_cimi(){
  Serial.println("sent the \"AT+CIMI\" command");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+CIMI");
  bg96_serial_read();
  delay(1000);
  Serial.println("sent the \"AT+CGSN\" command");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+CGSN");
  bg96_serial_read();
  delay(1000);
  }

void bg96_at_csq(){
  Serial.println("\n");
  Serial.println("the module look for the signal strenght:");
  while(1){
    bg96_serial_clearbuf();
    bg96_serial.println("AT+CSQ");
    bg96_serial_read();
   // if(strstr(return_dat,"+CSQ: 99,99") != NULL){
    //  Serial.println("config the NB modlue");
      //bianixe cheng xu
    // }
      if(strstr(return_dat,"+CSQ: 99,99") != NULL){
      bg96_serial.println("AT+QCFG=\"nwscanmode\",0,1");
      Serial.println("AT+QCFG=\"nwscanmode\",0,1");
      Serial.println("wait for 5 second");
      delay(5000);
      }
    else{
      Serial.println("the module has signal");
      break;}
    }
  }
void bg96_at_cereg()
{
  Serial.println("\n");
  Serial.println("look for network already register.");
  while(1){
    bg96_serial_clearbuf();
    bg96_serial.println("AT+CEREG?");
    bg96_serial_read();
    if(strstr(return_dat,"+CEREG: 0,1") != NULL)
    {
      Serial.println("the module already register");
      break;
      }
    else{

      delay(5000);
      }
  }
  }
void bg96_at_cgpaddr(){
  Serial.println("\n");
  Serial.println("look for the temporary IP");
    bg96_serial_clearbuf();
    bg96_serial.println("AT+CGPADDR");
    bg96_serial_read();
}

/********udp protocol*********/
void udp_protocal(){
  start_udp();
  udp_hardware();
  udp_send();
  udp_recieve();
  udp_close();
}
void snprintf_fun1(){
  char cmd[64]={"0"};
  snprintf(cmd,sizeof(cmd),"AT+QIOPEN=1,2,\"UDP SERVICE\",\"%s\",0,%d,0",UDP_server,UDP_port);
  Serial.println(cmd);
  bg96_serial.println(cmd);
  }
void start_udp()
{
  Serial.println("Start a UDP Service.");
  while(1){
  bg96_serial_clearbuf();
  snprintf_fun1();
  bg96_serial_read();
  if(strstr(return_dat,"+QIOPEN: 2,0")){
    Serial.println("the udp server already set up");
    break;}
   else{
    while(1){
    Serial.println("close the UDP service");
    bg96_serial_clearbuf();
    bg96_serial.println("AT+QICLOSE=2");
    bg96_serial_read();
    if(strstr(return_dat,"OK")!=NULL)
    break;
    }
    }
  }
  }
void udp_hardware(){
   bg96_serial_clearbuf();
   bg96_serial.println("AT+QISTATE=0,1");
   bg96_serial_read();
  }
void udp_send()
{
  int tem_x100,hum_x100;
  tem_x100 = (int)(tem*100);
  hum_x100 = hum*100;
  int press_c = (int)(press-100000);
  char cmd[64]={"0"};
  char cmd1[64]={"0"};
  snprintf(cmd1,sizeof(cmd1),"AT+QISENDEX=0,\"%04x%04x%04x%04x%04x%04x\"",tem_x100,hum_x100,press_c,upcount,light,sound);
  bg96_serial_clearbuf();
  Serial.println("Send UDP Data to Remote");
  //snprintf(cmd1,sizeof(cmd1),"dragino-->tem=%d.%d,hum=%d.%d",tem_h,tem_l,hum_h,hum_l);
  snprintf(cmd,sizeof(cmd),"AT+QISEND=2,%d,\"%s\",%d",(strlen(cmd1)+1),UDP_server,UDP_port);
  Serial.println(cmd);
  bg96_serial.println(cmd);
  bg96_serial_read();
  if(strstr(return_dat,">")!=NULL){
  bg96_serial.println(cmd1);
    }
  bg96_serial_read();
  }

void udp_recieve()
{
 delay(2000);
 Serial.println("receive the UDP data:");
 bg96_serial_clearbuf();
 bg96_serial.println("AT+QIRD=2");
 bg96_serial_read();
  }
void udp_close()
{
  Serial.println("close the udp service");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QICLOSE=2");
  bg96_serial_read();
  }
