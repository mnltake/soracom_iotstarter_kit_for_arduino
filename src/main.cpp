
#include "main.h"

void setup() {
  pinMode(LIGHTSENSORPIN, INPUT);
  pinMode(SOUNDSENSORPIN, INPUT);
  pinMode(VOLUMEPIN, INPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(BG96_RESETPIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(BG96_RESETPIN, HIGH);//modem reset
  u8x8.begin();
  u8x8.setFlipMode(U8X8_ENABLE_180_DEGREE_ROTATION);
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.println("Start:");
  u8x8.println("Please wait");
  delay(100);
  digitalWrite(BG96_RESETPIN, LOW);//BG96 start
  digitalWrite(LED_BUILTIN, HIGH);//LED_BUILTIN ON
  tone(BUZZERPIN ,440);//beep
  bg96_initial();
  bmp280.init();
  oled_write(0);
  /*
  LIS.begin(Wire, 0x19);
  delay(100);
  LIS.setFullScaleRange(LIS3DHTR_RANGE_8G);
  LIS.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);
  LIS.setHighSolution(true); 
  */
  savebyte = EEPROM.read(EEPROMINDENX);
  Serial.println(savebyte, BIN);
  if (savebyte == 0xff){ //factory default
    savebyte = 0b10000101;
  }
  sendbyte=savebyte;
  dispflg=bitRead(sendbyte,7);
  ledflg=bitRead(sendbyte,3);

  noTone(BUZZERPIN);
}

void loop() {
  bg96_powersave_disable();
  delay(500);
  Watchdog.enable(8000);
  digitalWrite(LED_BUILTIN, HIGH);//wake
  /* read sensor*/
  light= analogRead(LIGHTSENSORPIN);
  Serial.print("light sensor is:");
  Serial.println(light);                

  long sum = 0;
  for (int i = 0; i < soundSamplingCount; i++) {
    sum += analogRead(SOUNDSENSORPIN);
    delay(5);
  }
  sound = sum / soundSamplingCount;
  Serial.print("Sound sensor is:");
  Serial.println(sound);                 

  DHT.read11(DHT11_PIN); 
  tem = DHT.temperature*1.0;    //float(℃)
  Serial.print("Tmperature is:");
  Serial.println(tem);
     
  hum = DHT.humidity* 1.0;      //int(%)
  Serial.print("Humidity is:");
  Serial.println(hum);

  press = bmp280.getPressure() ;//long(Pa)
  Serial.print("AirPress is:");
  Serial.println(press);
  /*
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
  Serial.print("Acceleratin is:");
  Serial.print("x:"); Serial.print(x); Serial.print("  ");
  Serial.print(", y:"); Serial.print(y); Serial.print("  ");
  Serial.print(", z:"); Serial.println(z);
  movex=(abs(x) >0.5)?true:false;
  movey=(abs(y) >0.5)?true:false;
  movez=(abs(z) >0.5)?true:false;
  */
  volume=analogRead(VOLUMEPIN);
  Serial.print("Volume is:");
  Serial.println(volume);     
  
  tempbmp = bmp280.getTemperature() ;//flaot(℃)
  Serial.print("TempBMP280 is:");
  Serial.println(tempbmp);
  
  Serial.print("upcount is:");
  Serial.println(upcount);                 
  bitWrite(sendbyte,7,dispflg);
  bitWrite(sendbyte,6,movex);
  bitWrite(sendbyte,5,movey);
  bitWrite(sendbyte,4,movez);
  bitWrite(sendbyte,3,ledflg);
  bitWrite(sendbyte,2,1);
  bitWrite(sendbyte,1,0);
  bitWrite(sendbyte,0,1);
  if (sendbyte != savebyte){
    Serial.println("Save byte to EEPROM");
    EEPROM.write(EEPROMINDENX, sendbyte);
  }
  Serial.println(sendbyte, BIN);
  savebyte = sendbyte;
  //start send data
    //tcp_protocal();
    tcp_send_dat();
    delay(2000);
    if(strstr(return_dat,"OK") == NULL && strstr(return_dat,"ok") == NULL  ){ 
      tone(BUZZERPIN ,440);//ERROR beep
      tcp_close();
      mqtt_close();
      delay(8000); //WDT restart
    }
    tcp_receive_dat();
   //sleep loop
  bg96_powersave_enable();
  digitalWrite(LED_BUILTIN, LOW);
  for(int i=0 ;i<18;i++){
    Watchdog.disable();
    //Serial.println(i),
    bg96_serial_clearbuf();
    bg96_serial_read();
    const char *p_msg;
  if(strstr(return_dat,"QMTRECV") != NULL ){
    Watchdog.reset();
    Serial.print("rcvmsg= ");
    p_msg=strstr(return_dat,"{\"cmd\"");
    Serial.println(p_msg);
    i=99;
    DeserializationError error = deserializeJson(doc, p_msg);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    subcmd=doc["cmd"];
    Serial.println(subcmd);
    oled_write(0);
    delay(1000);
  
    if (subcmd==1){
      ledflg=true;
    }else if(subcmd==0){
      ledflg=false;
    }else if(subcmd==2){
      dispflg= !dispflg;
    }

  }
  digitalWrite(LEDPIN, ledflg);
  //OLED display
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
  switch (i%9){
    case 0:
    if(strstr(return_dat,"OK")!=NULL){
      u8x8.println("return:");
      u8x8.println();
      u8x8.println("OK");
    }else if(strstr(return_dat,"ERROR")!=NULL){
      u8x8.println("return:");
      u8x8.println();
      u8x8.println("ERROR");
    }else {
      u8x8.println(i);
      u8x8.println("subcmd:");
      u8x8.println(subcmd);
    }
      return;
    case 1:
      u8x8.println(i);
      u8x8.println("Temperture:");
      u8x8.print(tem);u8x8.println(" \"C");
      return;
    case 2:
      u8x8.println(i);
      u8x8.println("Humidity:");
      u8x8.print(hum);u8x8.println(" %");
      return;
    case 3:
      u8x8.println(i);
      u8x8.println("Air Pressure:");
      u8x8.print(press*0.01);u8x8.println(" hPa");
      return;
    case 4:
      u8x8.println(i);
      u8x8.println("Light:");
      u8x8.println(light);
      return;
    case 5:
      u8x8.println(i);
      u8x8.println("Sound:");
      u8x8.println(sound);
      return;
    case 6:
      u8x8.println(i);
      u8x8.println("Volume:");
      u8x8.println(volume);
      return;
    case 7:
      u8x8.println(i);
      u8x8.println("TempBMP280:");
      u8x8.println(tempbmp);
      return;
    case 8:
      u8x8.println(i);
      u8x8.println("Up Count:");
      u8x8.println(upcount);
      return;
    default:
      return;
  }
return;
}

/********tcp protocol*********/
void tcp_protocal()
{
  tcp_send_dat();
  delay(2000);
  tcp_receive_dat();

}

void tcp_send_dat(){
  int tem_x100,hum_x100,tempbmp_x100;
  tem_x100 = (int)(tem*100);
  hum_x100 = hum*100;
  tempbmp_x100 = tempbmp*100;
  int press_c = (int)(press-100000);


  char cmd[64]={"0"};
  snprintf(cmd,sizeof(cmd),"AT+QISENDEX=0,\"%04x%04x%04x%04x%04x%04x%02x%04x%04x\"",
  tem_x100,hum_x100,press_c,upcount,light,sound,sendbyte,volume,tempbmp_x100);
  bg96_serial_clearbuf();
  bg96_serial.println(cmd);
  delay(500);
  bg96_serial_read();
  //Serial.println(cmd);
  //Serial.println("println the tcp send data");
  Watchdog.reset();
}


void tcp_receive_dat()
{
  Serial.println("receive TCP data:");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QIRD=0,1500");
  delay(500);
  bg96_serial_read();

  }


void  bg96_initial(){
  delay(1000);
  Serial.begin(USB_BAUD);            
  bg96_serial.begin(BG96_BAUD);                 //open the Serial ,BAUD is 9600bps
  Serial.println("start the serial port,BAUD is 9600bps.");
  delay(500);
  //bg96_at_cmd();
  /*
  delay(500);
  bg96_ate_cmd();
  delay(500);
  bg96_at_cimi();
  bg96_at_csq();
  delay(500);*/
  //bg96_at_cereg();
  //delay(500);
  //bg96_at_cgpaddr();
  //delay(500);
  Watchdog.reset();
  //tcp_config();
  tcp_close();
  delay(500);
  tcp_config();
  mqtt_config();
  }

void bg96_serial_read()
{
  delay(500);
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
void tcp_close()
{
  Serial.println("close tcp server.");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QICLOSE=0");
  bg96_serial_read();
}

/********mqtt protocol*********/
void mqtt_config()
{
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QMTOPEN=1,\"beam.soracom.io\",1883");//MQTT
  bg96_serial_read();
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QMTCONN=1,\"soracom\"");
  bg96_serial_read();
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QMTSUB=1,1,\"soracom/cmd/1\",0");
  bg96_serial_read();
}


void mqtt_close()
{
  Serial.println("disconnect mqtt");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QMTDISC=1");
  bg96_serial_read();
  Serial.println("close mqtt.");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+QMTCLOSE=1");
  bg96_serial_read();
}

/**********AT comade*******/
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
    }
 }

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
    delay(500);
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
    delay(500);
    bg96_serial_read();
}

void bg96_powersave_enable(){
  Serial.println("Power save mode setting");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+CPSMS=1,,,\"10100001\",\"00000000\"");//1min powersave 0sec active
  delay(300);
  bg96_serial_read();
}

void bg96_powersave_disable(){
  Serial.println("Power save mode disable");
  bg96_serial_clearbuf();
  bg96_serial.println("AT+CPSMS=0");
  delay(300);
  bg96_serial_read();
}