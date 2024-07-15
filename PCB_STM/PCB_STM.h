 
/*
Versión 20200901

Board version:
Arduino                1.8.13
Arduino AVR             1.8.3
SAM 32B Cortex M3(DUE)        1.6.12
STM32F1xx/GD32F1xx          2020.12.7

Library versions:
SD                  1.2.4
SDFAT               1.1.4

Preferencias:
https://dl.espressif.com/dl/package_esp32_index.json
http://arduino.esp8266.com/stable/package_esp8266com_index.json
http://dan.drown.org/stm32duino/package_STM32duino_index.json

Using Parser : Raw BINARY
Interface serial_w32: 115200 8E1
Version     : 0x22
Option 1    : 0x00
Option 2    : 0x00
Device ID   : 0x0410 (Medium-density)
- RAM       : 20KiB (512b reserved by bootloader)
- Flash     : 128KiB (sector size: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB

Total Flash memory: 131,072 Bytes. Total RAM memory: 20,480 Bytes. EEPROM starts at address 126.976
Flash Usage RAM Usage (Bytes) Remaining Version
 44.068 (33%)  7.176   (35%)    13.304    V01.0   (20210510) PRUEBAS  V_Arduino 1.8.13  Funciona bien
 51.168 (39%)  7.232   (35%)    13.248    V01.0   (20210510) PRUEBAS    V_Arduino 1.8.13  Funciona bien
 51.444 (39%)  7.208   (35%)    13.272    V01.1   (20210511) PRUEBAS    V_Arduino 1.8.13  Funciona bien
 51.932 (39%)  7.296   (35%)    13.184    V01.2   (20210512) PRUEBAS    V_Arduino 1.8.13  Funciona bien
 52.064 (39%)  7.296   (35%)    13.184    V01.2   (20210513) PRUEBAS    V_Arduino 1.8.13  Funciona bien
 52.576 (40%)  7.304   (35%)    13.176    V01.3   (20210514) PRUEBAS    V_Arduino 1.8.13  Funciona bien
 52.600 (40%)  7.304   (35%)    13.176    V01.3   (20210514) PRUEBAS    V_Arduino 1.8.13  Funciona bien
 52.448 (39%)  7.304   (35%)    13.176    V01.3   (20210514)        V_Arduino 1.8.13  Funciona bien
 53.168 (40%)  7.320   (35%)    13.160    V02.0   (20210515)        V_Arduino 1.8.13  Funciona bien
 53.100 (40%)  7.320   (35%)    13.160    V02.1   (20210516)        V_Arduino 1.8.13  Funciona bien
 53.728 (40%)  7.324   (35%)    13.156    V02.1   (20210517)        V_Arduino 1.8.13  Funciona bien

*/

#ifndef MED_CON_H
  #define MED_CON_H
  // **********************************************
  // **** Constant declaration ***************
  // **********************************************
  //Comment the line when it is not in test mode, among other things, wait until the USB is available as a trace.
  //#define TESTS //show the USB trace even if it is not activated
  #define VERSION           PSTR("_V02.1\n")

  //SIN_OSC->PB7->pin   where the oscilloscope is connected.
  //#define SIN_OSC(x)  gpio_write_bit(GPIOB, 7, x)
  //P_LED_2->PB8->pin   where the red LED (external) is connected.
  //#define LEE_LED_2   gpio_read_bit(GPIOB, 8)
  //#define RECORD_LED_2(x) gpio_write_bit(GPIOB, 8, x)
  //#define INV_LED_2 gpio_toggle_bit(GPIOB, 8) //inverts the red LED.
  //P_LED_3->PB9->pin   where the yellow LED (external) is connected.
  //#define LEE_LED_3   gpio_read_bit(GPIOB, 9)
  //#define RECORD_LED_3(x) gpio_write_bit(GPIOB, 9, x)
  //#define INV_LED_3   gpio_toggle_bit(GPIOB, 9) //inverts the yellow LED.
  #define PCB1_0 //adapter pins to the board "PCB1.0 9V"
  //Definiciones de pines:
  //#define P_RST_I           PA1       //pin reset PCC_I.
  //#define P_RST_D           PB0       //pin reset PCC_D.
  #define P_RST_E           PA8       //pin reset ESP32.
  //#define P_SIN_I           PA0       //pin synchronism PCC_I
  //#define P_SIN_D           PB1       //pin synchronism PCC_D
  #ifdef PCB1_0
    #define P_RST_I           PA0       //pin reset PCC_I.
    #define P_RST_D           PB0       //pin reset PCC_D.
    #define P_SIN_I           PA1       //pin synchronism PCC_I
    #define P_SIN_D           PB1       //pin synchronism PCC_D
  #else
    #define P_RST_I           PA1       //pin reset PCC_I.
    #define P_RST_D           PB0       //pin reset PCC_D.
    #define P_SIN_I           PA0       //pin synchronism PCC_I
    #define P_SIN_D           PB1       //pin synchronism PCC_D
  #endif
  #define P_SIN_E           PB15      //pin synchronism ESP32.
  #define P_SD_CS           PA4       //pin selection SD
  #define P_OSCI            PB7       //Oscilloscope output pin for time control.
  #define P_W_D             PB6       //pin watchdog.
  #define P_LED_2           PB8       //pin where LED 2 is connected (external red).
  #define P_LED_3           PB9       //pin where LED 3 is connected (external red).
  #define P_LED           PC13      //pin where the green LED (internal) is connected.
  //Declaraciones varias:
  #define TIMEOUT           2000      //2 second timeout.
  #define TI_E_EJ           1000      //time between runs by default.
  #define L_TX            256       //text length.
  #define L_TX_24           232       //text length.
  //#define P_ME_F            (const prog_char *)F       
  //#define P_ME_F            PSTR
  #define PR_USB            Serial      //communications with the trace; USB.
  #define CO_ESP32          Serial1     //communications with ESP32; PA10_Rx1->Tx; PA9_Tx1->Rx.
  #define CO_PCC_I          Serial2     //communications with PCC_I; PA3_Rx2->Tx; PA2_Tx2->Rx.
  #define CO_PCC_D          Serial3     //communications with PCC_D; PB11_Rx2->Tx; PB10_Tx2->Rx.
  #define TRACE           if (_bTrace) Serial1
  
  // **********************************************
  // **** Declaration of global classes **********
  // **********************************************
  //In this version, if they are not declared, they do not work:
  //HardwareSerial Serial1(PA10, PA9);
  HardwareSerial Serial2(PA3, PA2);
  HardwareSerial Serial3(PB11, PB10);

  // **********************************************
  // ****Global variable declaration ***********
  // **** *****************************************
  volatile bool         _bComTmp;   //check alarm.
  int16_t             _iNumMd;    //number of measurements.
  volatile bool         _bPCC_I;
  volatile uint32_t       _vuPCC_I;
  volatile bool         _bPCC_D;
  volatile uint32_t       _vuPCC_D;
  volatile bool         _bESP32;
  volatile uint32_t       _vuESP32;
  uint16_t            _uPCC_I;
  uint16_t            _uPCC_D;
  uint16_t            _uESP32;
  uint32_t            _uIConBuc;
  uint32_t            _uIESP32;
  uint32_t            _uIPCC_I;
  uint32_t            _uIPCC_D;
  bool              _bI_PCC_I;    //to be able to block the wachdog from menu BI
  bool              _bI_PCC_D;    //to be able to block the wachdog from menu BD
  bool              _bI_ESP32;    //to be able to block the wachdog from menu BE
  
  bool              _bTrace;
  bool              _bFechaOK;
  bool              _bGraAct;
  bool              _bActInt;
  char              _tEnt[L_TX];  //default input text.
  char              _tFSH[L_TX];  //error text.
  char              _tInt[L_TX];  //intermediate default text.
  char              _tNFiLog[16]; //name of the LOG file.
  char              _tNFiTra[16]; //name of the job LOG file.
  SdFile              _fFicT;     //job LOG file.
  bool              _bFicTOK;
  char              _tSal[L_TX];  //default output text.
  char              _tSTrA[L_TX]; //text used to record traces.
  char              _tSTrB[L_TX]; //text used to record traces.

  //These variables are const, because they do not vary during the execution.
  const char            *_tOrigen[]  = { "USB", "ESP32", "PCC_I", "PCC_D", "Sist", "PRUEBAS", "\r\n##Test##" };
  
  const char T_MENU[] =
    "\tThis information only appears in\r\n" \
    "in case the file cannot be read\r\n" \
    "SIST/MENU.TXT what is in the SD\r\n" \
    "\r\n**** find out SD ****\r\n";
    
  DateVar             _dvFecha;
  TimeVar             _tvHora;
  STM32F1_RTC           _rtcVal;
  uint32_t            _rtcTie;
  uint32_t            _uTieCom;   //last time.
  uint16_t            _uTraAct;
  uint16_t            _uConBuc;
  

  //Interruptions to detect if any microphone has been fried. 
  void INT_SIN_I() {  _bPCC_I = true; _vuPCC_I++; }
  void INT_SIN_D() {  _bPCC_D = true; _vuPCC_D++; }
  void INT_SIN_E() {  _bESP32 = true; _vuESP32++; }

/*
  // **********************************************
  // **** Function declaration: ***************
  // **********************************************
  void    Int_Tiempo();
  void    Intermitencia(int);
  void    setup();
  void    loop();
  void    BorraContadores();
  void    InicializaSistema();

  bool    Entrada(uint8_t, HardwareSerial);
  bool    EntradaUSB();
  bool    EjecutaComando(uint8_t, char*);
  void    Info();
  void    InfoTec();
  void    InfoFW(char*);

  // ********************************************************
  // ********** Function declaration Med_Co *************
  // ********************************************************
  char*   DaHora(char*);
  char*   DaFecha(char*);
  size_t    getLength(const __FlashStringHelper*);
  bool    GrabarFecha(char*);
  bool    HayError(bool, const __FlashStringHelper*);
  bool    HayError(uint8_t, uint8_t, const __FlashStringHelper*);
  bool    LeeNumero(char[], int16_t*, int16_t);
  bool    LeeNumero(char[], int32_t*, int16_t);
  bool    LeeNumero(char[], uint16_t*, int16_t);
  bool    LeeNumero(char[], uint32_t*, int16_t);
  int16_t   PosTexto(int16_t, char*);
  size_t    strcatFSH(char *, const __FlashStringHelper*);
  int16_t   Valor(bool*, int16_t, int16_t, int16_t, int16_t, char*);
  char*   AcotaTexto(char, char, char*);
  void    ResetearMicro(uint32_t);
  void    ApagarMicro(uint32_t);
*/

  void    Imprime(const __FlashStringHelper*, bool = true);
  void    Imprime(char*, bool = true);

#endif
