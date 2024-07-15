                            /* MCU: for ESP32 Family
 * Este programa hace de el ESP32 un coprocesador de comunicaciones (TFT, Bluetooth y Wi-Fi).
 * El ESP32 hace de Hub de comunicaciones entre el usuario (Bluetooth, Wi-Fi (IoT), TFT) y el 
 * STM32 Master (PCB_STM) hub que a su vez se comunica con las dos placas PCC.
 * Añade un menu gráfico con una pantalla TFT.
 *
 *
 * Desarrollado en Arduino 1.8.13
 * Desarrollado por: Carlos Agorreta
 *
 * 28/04/21: Primer datalog. Mirar versión firmware con comando "IV&"
 * 17/05/2021: Comunicación por Bluetooth, adaptado a PCC_V4 y PCB_STM
 * 29/07/2021: Menu gráfico con pantalla ILI9341
 * 
 * //Implemented and tested//
 * Lector Micro SD SPI mode.
 * Periféricos: Encoder KY-040, TFT ILI9341, 
 * RTC DS3231 sincronizado con RTC interno
 *
 * Bugs: 
 * PhotoRX v28.07.2021
 * Se junta el menú gráfico con la pantalla TFT de 240x320 con ILI9341 y TcMenu.
 */
// *****************************************************
// ************ Libraries and headers: *****************
// *****************************************************
#include <arduino.h>                //Include for Platformio (Visual Studio Code) and C++ migration
#include <stdio.h>                  //Generic library. https://www.cplusplus.com/reference/cstdio/ 
#include <HardwareSerial.h>         //ESP32 Arduino specific library for UARTs' configuration 
#include "src/PhotoRX_menu.h"
#include <esp_task_wdt.h>
#include <TFT_eSPI.h>               // Hardware-specific library
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;           //Uncomment TRACE if you are going to use BT Classic
                                    //If TRACE and Serial are used at the same time, they share UART0.
#include <SPI.h>                    //librería de SPI para pantalla,lector SD. Es genérica de Arduino.
#include "FS.h"                     //File System library. Genérica.
#include "SD.h"                     //library for SD card. Genérica. 
#include <Wire.h>                   //library for I2C for DS3231. Generic.
#include <ESP32Time.h>                                                    //_RTC_INT library for internal RTC. v1.0.3 de fbiego https://github.com/fbiego/ESP32Time
#include <ErriezDS3231.h>           //rtc library for DS3231. v2.0.0 Available to Download at https://github.com/Erriez/ErriezDS3231
#include "definitions.h"

void setup() {
  uint8_t b1;
  bool ini_sync = false;
/***************** Serial UARTS 0/BT,1 and 2 initialization *****************/
  #ifdef PRUEBAS
    Serial.begin(SERIAL_DATA_BITS);                           //Uart0
  #define TRACE Serial
  #else
    SerialBT.begin("PhotoRX2");                                     //Bluetooth device name
  #define TRACE SerialBT
  #endif
  CO_MASTER.begin( SERIAL_DATA_BITS, SERIAL_8N1,P_RX1,P_TX1);         //PCP0 Base Board v0.2
  Serial2.begin( SERIAL_DATA_BITS, SERIAL_8N1,P_RX2,P_TX2);
/***************** I2C initialization ************************/
  Wire.begin();                                                       //Start I2C
  Wire.setClock(FAST_I2C);                                              //I2C Standard 400 kbps
/*********** Initialize External and Internal RTC ************/
  if(!rtc.begin()){                                               // Initialize external RTC (_RTC_INT)
    TRACE.println("DS3231 not found. Use Manual date, Bluetooth epoch or NTP epoch");
  }
  rtc.setSquareWave(SquareWaveDisable);
  time(&_Now);
  //if (!rtc.read(&t_hour, &t_min, &t_sec, &t_day, &t_year, &t_mon,&t_wday)) {
  if (!rtc.read(&dt)) {
    TRACE.println(F("Get time failed"));
  }         
  _RTC_INT.setTime(rtc.getEpoch()); 
  setenv("TZ","CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);       //configura TZ con TZ_INFO (time zone EUW)
  tzset();                                                                //inicializa variable TZ
/***************** SD card initialization ****************/
  initSDCard();                                                 //Initializes SD Card. Check SDinit.ino
/***************** Internal Watchdog configuration ***********************/
  TRACE.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true);                         //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                                       //add current thread to WDT watch
/***************** ESP32 MAC ID ***********************/  
_chipid=ESP.getEfuseMac();                                    //The chip ID is essentially its MAC address(length: 6 bytes).
TRACE.printf("ESP32 Chip ID = %04X",(uint16_t)(_chipid>>32));
TRACE.printf("%08X\n",(uint32_t)_chipid);                 //print Low 4bytes. 
/*********** TcMenu Task initialization ************/ 
  setupMenu();
  menuTime.setTime(TimeStorage(dt.tm_hour,dt.tm_min));
  menuDate.setDate(DateStorage(dt.tm_mday,dt.tm_mon+1  ,1900+dt.tm_year));
  menuTime.setChanged(true);
  menuLogMessage.setTextValue("No error detected");
/*********** Vector Initialization ************/
  snprintf(_tDSD,sizeof(_tDSD),"");           //Arrays initialized as NULL
  snprintf(_tDSD,sizeof(_tDST),"");
  snprintf(_tDSD,sizeof(_tISD),"");
  snprintf(_tDSD,sizeof(_tISD),"");
  menuLogMessage.setTextValue("Recently initialized");
/***************** time/sync variables initializations *******************/ 
  _tTimer1 = millis();
/***************** Sync with Master *******************/
while(!ini_sync){
  if((millis()-_tTimer1)>1000)
  {
    TRACE.println("Waiting for Master to pull line high...");
    _tTimer1 = millis();
  } 
  if(!digitalRead(P_SYNC)){
    ini_sync = true;
    TRACE.println("Sync pin detected, sending date");
    snprintf(_tDest,sizeof(_tDest),"FG ");
    _RTC_INT.getTime("%Y/%m/%d;%H:%M:%S").toCharArray(_tSource, sizeof(_tSource)); //turns back String to array
    strncat(_tDest,_tSource,sizeof(_tDest));
    #ifdef rst
    CO_MASTER.println("TR");
    CO_MASTER.println("BE");
    CO_MASTER.println("BI");
    CO_MASTER.println("BD");
    #endif
    CO_MASTER.println(_tDest);
    TRACE.println(_tDest);
  } 
    delay(15);
    pinMode(P_SYNC,OUTPUT);                                 
}
while(digitalRead(P_SYNC)){
  TRACE.println("Waiting for Master fo finish...");
}
TRACE.println("Now STM_PCB works as external Watchdog");
digitalWrite(P_SYNC, HIGH);                               //Al estar modo open drain por hardware, al estar en High, la linea se pone en low
CO_MASTER.println("IT");                                  //master imprime informacion tecnica
/***************** Setup finished ***********************/
TRACE.println("The device finished setup"); 
}

void loop() {
    uint8_t t_hour,t_min,t_sec;
   uint8_t j,b1=0;
  esp_task_wdt_reset();                                       //internal watchdog timer reset
  while(CO_MASTER.available()>0){                               //clear RX buffer
    TRACE.print((char)CO_MASTER.read());
  }
  while(Serial2.available()>0){                             //clear RX buffer
    TRACE.print((char)Serial2.read());
  }
  while(TRACE.available()>0){                              //clear RX buffer
    EntradaBT();
  }
  if((millis()-_tTimer3)>100){
    _tTimer3 = millis();
    digitalWrite(P_SYNC, !digitalRead(P_SYNC));             //pin de sincronismo de 10 Hz, sirve para debuguear 
  }
    taskManager.runLoop();                                  //rutina de gestión de tareas para el menú
  if((millis()-_tTimer1)>1000){
    _tTimer1 = millis();
    WholeAndFraction wf2(200, 5, 0);
    WholeAndFraction wf_temp(25, 0, 0);
    menuChannel15.setFromWholeAndFraction(wf2);                     //ejemplo de mostrar valor analogico
    menuChannel0.setCurrentValue(1000);                       //ejemplo
    menuTemperaturePCP1.setCurrentValue((127+25), false);
    menuTemperaturePCP2.setFromWholeAndFraction(wf_temp);
    menuTemperaturePCP3.setFromWholeAndFraction(wf_temp);
    menuTemperaturePCP4.setFromWholeAndFraction(wf_temp);
    int8_t temperature = 0;
    uint8_t fraction = 0;
    if (!rtc.startTemperatureConversion()) {
      //       TRACE.println("Error: Start temperature conversion failed");
      menuLogMessage.setTextValue("Temperature conversion error");  
    }
    if (!rtc.getTemperature(&temperature, &fraction)) {
      //TRACE.println("Error: Get temperature failed");
        menuLogMessage.setTextValue("RTC Temp Error");
    }
    WholeAndFraction mb_temp(temperature, fraction, 0);
    menuMotherboardTemp.setFromWholeAndFraction(mb_temp);
  }
  if((millis()-_tTimer2)>60000){
    _tTimer2 = millis();
  if (!rtc.read(&dt)) {
    //TRACE.println(F("Get time failed"));
    menuLogMessage.setTextValue("RTC Time Error");
  }
  menuTime.setActive(true);
  menuTime.setTime(TimeStorage(dt.tm_hour,dt.tm_min));
  menuTime.setActive(false);
  _uCount++;
  if(_uCount == 1440){
    menuTime.setActive(false);
    menuDate.setActive(true);
    menuDate.setDate(DateStorage(dt.tm_mday,dt.tm_mon,1900+dt.tm_year));
    _RTC_INT.setTime(rtc.getEpoch());             //recalibrar reloj interno con reloj externo de precision 
    menuDate.setActive(false);
  }
  }
}

bool EntradaBT() {
  uint8_t b1 = 0;
  char command[STR_S];
  bool r_detected = false;
  //uint32_t ulTie;
  //if(command[b1]!='*') return false;                     //SOF detection
  while(true){
  //if(TRACE.available()>0){                //reset RX buffer
    command[b1] = TRACE.read();
    if (command[b1]=='\n' /*&& r_detected*/) {
    command[b1] = (char) 0;
    CO_MASTER.println(command);
    command[b1] = '$';                            //añade identificador
    command[b1+1] = (char)0;                        //añade EOF
    TRACE.println(command);
    b1 = 0;
    r_detected = false;
    return true;
    }
    //else  r_detected = true;
    if(command[b1] == '&'){
    TRACE.println(command);                   
    TRACE.flush();                                    //clear TX buffer 
    EjecutaComando(command,b1);
    r_detected = false;
return true;
    }
      //}
    if(command[b1] != '\r') b1++;  
    if(b1>STR_L){
    b1 = 0; 
    TRACE.println("Max length reached");
    TRACE.flush();                                     //clear TX buffer
    }
  }
  return false;
}

void EjecutaComando(char *command, uint8_t& i){
//uint8_t iLonTex = strlen(command);
//  if (i<2)   return false; //comando menor a 2
  switch (command[i-2]){
    case 'E': //experimento
    TRACE.print("Comando de experimento recibido");
    break;
  case 'R':
    TRACE.flush();
    TRACE.println("About to Reset ESP32");
    ESP.restart(); //software reset user can force a restart
    break;
    case 'W':
    switch (command[i-1]){
      case 'T': //r //testea watchdog
        TRACE.println("Watchdog Test Activated");
        TRACE.println("System will enter infinite loop and should automatically recover");
        while(1){}
        break;
      }
    case 'I':
        switch (command[i-1]){
      case 'D': //ID& //muestra ID único del ESP32
        TRACE.printf("ESP32 Chip ID = %04X",(uint16_t)(_chipid>>32));//print High 2 bytes
        TRACE.printf("%08X\n",(uint32_t)_chipid);//print Low 4bytes.
        break;
          case 'V':
        TRACE.println("Version firmware Base Board 13.07");
        break;
          case 'T':
        CO_MASTER.println("IT");
        break;
        }
    case 'T':
    switch (command[i-1]){
          case 'A':
        TRACE.println("*DSA\t605\t750\t2000");          //% stands for ESP32-->App/BT
        CO_MASTER.println("*DSA\t605\t750\t2000");
        delay(100);
        TRACE.println("*DSD\t500\t500\t500\t500\t500\t500\t500\t500");
        CO_MASTER.println("*DSD\t500\t500\t500\t500\t500\t500\t500\t500");
        delay(100);
        TRACE.println("*DST\t5\t10\t15\t20\t25\t30\t35\t40");
        CO_MASTER.println("*DST\t5\t10\t15\t20\t25\t30\t35\t40");
        delay(100);
        break;
          case 'B':
        TRACE.println("*MD");
        break;
          case 'I': //TI& //imprime hora actual en el Serial
        TRACE.println(_RTC_INT.getTime());
        break;
          case 'C':
        rtc.setEpoch(1621847167);
        _RTC_INT.setTime(rtc.getEpoch());
        TRACE.println(_RTC_INT.getTime("%A, %B %d %Y %H:%M:%S"));
        break;
          case 'S': //TS& //Set Time
        digitalWrite(P_SYNC, HIGH);
        snprintf(_tDest,sizeof(_tDest),"FG ");
        _RTC_INT.getTime("%Y/%m/%d;%H:%M:%S").toCharArray(_tSource, sizeof(_tSource)); //turns back String to array
        strncat(_tDest,_tSource,sizeof(_tDest));
        //TRACE.println(myrtc.getTime("%Y/%m/%d;%H:%M:%S"));
        CO_MASTER.println(_tDest);
        TRACE.println(_tDest);
        break;
          case 'T': //TD is for Time Delay
        TRACE.println("*ISA\t605\t750\t2000");          //% stands for ESP32-->App/BT
        CO_MASTER.println("*ISA\t605\t750\t2000");
        delay(100);
        TRACE.println("*IST\t5\t10\t15\t20\t25\t30\t35\t40");
        CO_MASTER.println("*IST\t5\t10\t15\t20\t25\t30\t35\t40");
        delay(100);
        TRACE.println("*ISD\t500\t500\t500\t500\t500\t500\t500\t500");
        CO_MASTER.println("*ISD\t500\t500\t500\t500\t500\t500\t500\t500");
        delay(100);  
        break;
          case 'Y': //TD is for Time Delay
        TRACE.println("*DSA\t605\t750\t2000");          //% stands for ESP32-->App/BT
        CO_MASTER.println("*DSA\t605\t750\t2000");
        delay(50);
        TRACE.println("*DST\t5\t10\t15\t20\t25\t30\t35\t40");
        CO_MASTER.println("*DST\t5\t10\t15\t20\t25\t30\t35\t40");
        delay(50);
        TRACE.println("*DSD\t500\t500\t500\t500\t500\t500\t500\t500");
        CO_MASTER.println("*DSD\t500\t500\t500\t500\t500\t500\t500\t500");
        delay(50);  
        break;        
        }
        
    default:  
    break;
  } 
}


void CALLBACK_FUNCTION onSetCurrent(int id) {
  bool kilo_mode = false;
  uint32_t total_time/*,total_time_r0,total_time_r1,total_time_l0,total_timel1*/;
  uint16_t ch_y,u_current/*r0_ch_g, r0_ch_m, r0_ch_b,r1_ch_g, r1_ch_m, r1_ch_b,l0_ch_g, l0_ch_m, l0_ch_b,l1_ch_g, l1_ch_m, l1_ch_b,*/;
  float real_current;
  char _tInfo[STR_S];
  uint8_t vial,sent_hours,sent_minutes,sent_seconds;
  _uSide         = menuReactorSide.getCurrentValue();
  sent_hours   = menuHours.getCurrentValue();
  sent_minutes = menuMinutes.getCurrentValue();
  sent_seconds = menuSeconds.getCurrentValue();
  vial = menuVialConf.getCurrentValue();
  _uSquare = menuSquareConfig.getCurrentValue();
  u_current = menuPercentage.getCurrentValue();
  snprintf(_tInfo,sizeof(_tInfo),"Selected percentage %u %%",u_current);
  TRACE.println(_tInfo);
  perc_to_current(&u_current);
  real_current = u_current*0.244;
  snprintf(_tInfo,sizeof(_tInfo),"Selected current %u mA",(int)(real_current+0.5f));
  TRACE.println(_tInfo);
  total_time = time_in_seconds(sent_hours,sent_minutes,sent_seconds);           //pasa de horas, minutos, segundos a segundos
  /*  if(prepara_trama(vial,_uSquare,current,sent_hours,sent_minutes,sent_seconds)) {
  TRACE.print("Trama Enviada");
  }else{
  TRACE.print("No se pudo enviar la trama");
  }*/
  //TRACE.println(menuHours.getCurrentValue());
  switch(vial){
    case 0: TRACE.println("High Troughput Mode (32)");
    break;
    case 1: TRACE.println("Medium Size Vial (4)");      
    break;
    case 2: TRACE.println("Big Size Vial (1)");
    break;
    case 3: TRACE.println("Kilo Super Vial");
        kilo_mode = true;        
    break;
    default: TRACE.println("bug Vial onSend");
    break;
  }
  if(kilo_mode){
    snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,u_current,u_current,u_current,u_current); 
    TRACE.println(_tISD);
    CO_MASTER.println(_tISD);   
    delay(100);
    snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
    TRACE.println(_tIST);
    CO_MASTER.println(_tIST);   
    delay(100);   
    snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,u_current,u_current,u_current,u_current); 
    TRACE.println(_tDSD);
    CO_MASTER.println(_tDSD);   
    delay(100);   
    snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
    TRACE.println(_tDST);
    CO_MASTER.println(_tDST);     
  }
  else{
    //side = 0 (left), side = 1 (right), side = 2 (both)
    switch(_uSquare){
      case 0: menuLogMessage.setTextValue("No module selected");           //OOOO
      break;
      case 1: //OX
        if(_uSide == 0){ //OOOX
          TRACE.println("Left side module 1");
          if(vial == 0){ //Small vial, HTE Mode (32), left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,0,0,0,0);
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
          }
          if (vial == 1){ //Medium vial, left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,0,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
          }
          if (vial == 2){ //Big vial, left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,0,0,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,0,0,0,0);
          }
          TRACE.println(_tISD);
          TRACE.println(_tIST); 
          CO_MASTER.println(_tISD);
          delay(100);
          CO_MASTER.println(_tIST);         
        }
        if(_uSide == 1){ //OXOO
          TRACE.println("Right side module 1");
          if(vial == 0){ //Small vial, HTE Mode (32), right side
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,0,0,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
          }
          if (vial == 1){ //Medium vial, right side
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,0,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
              }
        if (vial == 2){ //Big vial, right side
          snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,0,0,0,0);  
          snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,0,0,0,0);
        } 
        TRACE.println(_tDSD);
        TRACE.println(_tDST);
        CO_MASTER.println(_tDSD);
        delay(100);
        CO_MASTER.println(_tDST);         
        } 
        if(_uSide == 2){ //OXOX
          TRACE.println("Both sides module 1");
          if(vial == 0){ //Small vial, HTE Mode (32), right side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,0,0,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,0,0,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
          }
          if (vial == 1){ //Medium vial, right side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,0,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);                  
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,0,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,0,0,0,0);
          }
          if (vial == 2){ //Big vial, right side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,0,0,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,0,0,0,0);
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,0,0,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,0,0,0,0);
          }
          TRACE.println(_tISD);
          TRACE.println(_tIST); 
          TRACE.println(_tDSD);
          TRACE.println(_tDST);
          CO_MASTER.println(_tISD);
          delay(100);
          CO_MASTER.println(_tIST);
          delay(100); 
          CO_MASTER.println(_tDSD);
          delay(100);
          CO_MASTER.println(_tDST);           
          }       
      break;
      case 2: //XO
        if(_uSide == 0){ //OOXO
          if(vial == 0){ //Small vial, HTE Mode (32), left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,u_current,u_current);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,total_time,total_time);
          }
          if (vial == 1){ //Medium vial, left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,0,u_current,u_current,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,total_time,0);
          }
          if (vial == 2){ //Big vial, left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,0,0);
          }
          TRACE.println(_tISD);
          TRACE.println(_tIST);
          CO_MASTER.println(_tISD);
          delay(100);
          CO_MASTER.println(_tIST);           
          }
        if(_uSide == 1){ //XOOO
          if(vial == 0){ //Small vial, HTE Mode (32), right side
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,u_current,u_current);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,total_time,total_time);
          }
          if (vial == 1){ //Medium vial, right side
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,0,u_current,u_current,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,total_time,total_time);
          }
          if (vial == 2){ //Big vial, right side
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,0,0);
          }
          TRACE.println(_tDSD);
          TRACE.println(_tDST);
          CO_MASTER.println(_tDSD);
          delay(100);
          CO_MASTER.println(_tDST);         
          } 
        if(_uSide == 2){ //XOXO
          if(vial == 0){ //Small vial, HTE Mode (32), both sides
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,u_current,u_current);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,total_time,total_time);
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,u_current,u_current);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,total_time,total_time);
          }
          if (vial == 1){ //Medium vial, both sides
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,0,u_current,u_current,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,0,total_time,total_time,0);                
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,0,u_current,u_current,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,0,total_time,total_time,0);
          }
          if (vial == 2){ //Big vial, both sides
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,0,0);
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,0,0,0,u_current,u_current,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,0,0,0,total_time,total_time,0,0);
          }
          TRACE.println(_tISD);
          TRACE.println(_tIST); 
          TRACE.println(_tDSD);
          TRACE.println(_tDST);
          CO_MASTER.println(_tISD);
          delay(100);
          CO_MASTER.println(_tIST);
          delay(100); 
          CO_MASTER.println(_tDSD);
          delay(100);
          CO_MASTER.println(_tDST);         
          } 
      break;
      case 3: //XX
        if(_uSide == 0){ //OOXX
          if(vial == 0){ //Small vial, HTE Mode (32), both sides
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,u_current,u_current,u_current,u_current);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
          }
          if (vial == 1){ //Medium vial, left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,u_current,u_current,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,total_time,total_time,0,0,total_time,total_time,0);
          }
          if (vial == 2){ //Big vial, left side
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,u_current,u_current,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,total_time,total_time,0,0);
          }
          TRACE.println(_tISD);
          TRACE.println(_tIST);
          CO_MASTER.println(_tISD);
          delay(100);
          CO_MASTER.println(_tIST);           
        }
        if(_uSide == 1){ //XXOO
          if(vial == 0){ //Small vial, HTE Mode (32), both sides
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,u_current,u_current,u_current,u_current);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
          }
          if (vial == 1){ //Medium vial, both sides
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,u_current,u_current,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",0,total_time,total_time,0,0,total_time,total_time,0);
          }
          if (vial == 2){ //Big vial, both sides
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,u_current,u_current,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,total_time,total_time,0,0);
          }
          TRACE.println(_tDSD);
          TRACE.println(_tDST);
          CO_MASTER.println(_tDSD);
          delay(100);
          CO_MASTER.println(_tDST);         
        } 
        if(_uSide == 2){ //XXXX
          if(vial == 0){ //Small vial, HTE Mode (32), all on
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,u_current,u_current,u_current,u_current);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,u_current,u_current,u_current,u_current,u_current,u_current);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
          }
          if (vial == 1){ //Medium vial, all on
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,u_current,u_current,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);                
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",0,u_current,u_current,0,0,u_current,u_current,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,total_time,total_time,total_time,total_time,total_time,total_time);
          }
          if (vial == 2){ //Big vial, all on
            snprintf(_tISD,sizeof(_tISD),"*ISD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,u_current,u_current,0,0);  
            snprintf(_tIST,sizeof(_tIST),"*IST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,total_time,total_time,0,0);
            snprintf(_tDSD,sizeof(_tDSD),"*DSD\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u",u_current,u_current,0,0,u_current,u_current,0,0);  
            snprintf(_tDST,sizeof(_tDST),"*DST\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu",total_time,total_time,0,0,total_time,total_time,0,0);
          }
          TRACE.println(_tISD);
          TRACE.println(_tIST); 
          TRACE.println(_tDSD);
          TRACE.println(_tDST);
          CO_MASTER.println(_tISD);
          delay(100);
          CO_MASTER.println(_tIST);
          delay(100); 
          CO_MASTER.println(_tDSD);
          delay(100);
          CO_MASTER.println(_tDST);           
        } 
      break;
      default: menuLogMessage.setTextValue("Configuration error");
      break;
    }
  }
}




void CALLBACK_FUNCTION onChangeStartExperiment(int id) {
  CO_MASTER.println("*DSO\t1\t2");
  delay(100);
  CO_MASTER.println("*ISO\t1\t2");
  delay(100);
  CO_MASTER.println("*DSA\t605\t750\t3000");
  delay(100);
  CO_MASTER.println("*ISA\t605\t750\t3000"); 
  delay(100);
  TRACE.println("*IA");
  TRACE.println("*DA");
  CO_MASTER.println("*IA");
  CO_MASTER.println("*DA");
  menuLogMessage.setTextValue("Running Experiment");
}

void CALLBACK_FUNCTION onChangeEmergencyStop(int id) {
    TRACE.println("*DD");
    TRACE.println("*ID");
    CO_MASTER.println("*DD");
    delay(100);
    CO_MASTER.println("*ID");
    menuLogMessage.setTextValue("Emergency Stop");
}




void CALLBACK_FUNCTION onSaveToMode2(int id) {
    // TODO - your menu change code
}


void CALLBACK_FUNCTION onSaveToMode1(int id) {
    // TODO - your menu change code
}

uint32_t time_in_seconds(uint8_t horas,uint8_t minutos,uint8_t segundos){
  uint32_t t_time;
  t_time = segundos+60*(minutos+horas*60); //total time seconds
  return t_time;
}
void perc_to_current(uint16_t *perc){
  float f1;
  uint16_t i1;
  f1 = (*perc)*28.67;
  *perc = (int)(f1+0.5f);                      //nearest integer round
}

void CALLBACK_FUNCTION onSetMode1(int id) {
  menuLogMessage.setTextValue("Executing Mode 1");
  CO_MASTER.println("*DSA\t605\t750\t3000");
  delay(100);
  CO_MASTER.println("*ISA\t605\t750\t3000");
  delay(100);
  CO_MASTER.println("*DSD\t4095\t4095\t4095\t4095\t4095\t4095\t4095\t4095");
  delay(100);
  CO_MASTER.println("*ISD\t4095\t4095\t4095\t4095\t4095\t4095\t4095\t4095");
  delay(100);
  CO_MASTER.println("*DST\t57600\t57600\t57600\t57600\t57600\t57600\t57600\t57600");
  delay(100);
  CO_MASTER.println("*DST\t57600\t57600\t57600\t57600\t57600\t57600\t57600\t57600");
}

void CALLBACK_FUNCTION onSetMode2(int id) {
    // TODO - your menu change code
}

void CALLBACK_FUNCTION onResetConfig(int id) {
    menuLogMessage.setTextValue("Empty Config");
    CO_MASTER.println("*DSD\t0\t0\t0\t0\t0\t0\t0\t0");
  delay(100);
  CO_MASTER.println("*ISD\t0\t0\t0\t0\t0\t0\t0\t0");
  delay(100);
  CO_MASTER.println("*IST\t0\t0\t0\t0\t0\t0\t0\t0");
  delay(100);
  CO_MASTER.println("*DST\t0\t0\t0\t0\t0\t0\t0\t0");
  delay(100);
  CO_MASTER.println("BI");
  delay(100);
  CO_MASTER.println("BD");
  menuLogMessage.setTextValue("Resetted Config");
  
}
