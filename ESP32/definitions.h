/*
* File with defines for PhotoRX sketch
*
*/
// *****************************************************
// ************ Board Pin and other definitions: *********
// *****************************************************
       //#define PRUEBAS               //enables USB and disables Bluetooth
  #ifdef PRUEBAS
  #define P_SYNC          14
  #endif
  //#define rst
  #define P_SCK         17
  #define P_MISO        19
  #define P_MOSI        23
  #define P_CS0         5
  #define P_RX1         16
  #define P_TX1 17
  #define P_RX2 27
  #define P_TX2 26
  #define P_SYNC 15
  #define SERIAL_DATA_BITS  115200
  #define WDT_TIMEOUT     8               //internal watchdog timeout in seconds.
  #define STR_L 128
  #define TIMEOUT 5000                        //2 seconds timeout
  #define STR_S 32
  #define CO_MASTER       Serial1
  #define TRACE       Serial
  #define FAST_I2C      400000
// *****************************************************
// ************ Object declaration: *********
// *****************************************************
  ESP32Time _RTC_INT;                         //object for internal RTC
  ErriezDS3231 rtc;                         //object of DS3231
// *****************************************************
// ************ Global variables: *********
// *****************************************************
  time_t _Now;                                //
  struct tm dt;                               //
  uint16_t _uCount = 0;
  uint32_t _tTimer1,_tTimer2,_tTimer3;
  uint8_t _uSide = 255, _uSquare = 255,_counter = 0;
  char _tSource[STR_L], _tDest[STR_L];
  uint64_t _chipid;                           //chip ID is essentially its MAC address
  bool detected = false;
  char _tDSD[STR_L],_tDST[STR_L],_tIST[STR_L],_tISD[STR_L];
// *****************************************************
// ************ Configuration definitions: *********
// *****************************************************
  #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
  #endif
// *****************************************************
// ************ Routines' definition: *********
// *****************************************************
  void perc_to_current(uint16_t*);
  uint32_t time_in_seconds(uint8_t,uint8_t,uint8_t);
  void setup();
  void loop();
  void listDir(fs::FS&, const char * , uint8_t );
  void createDir(fs::FS&, const char *);
  void removeDir(fs::FS&, const char *);
  void readFile(fs::FS &, const char *);
  void appendFile(fs::FS &, const char * , const char * );
  void appendlnFile(fs::FS &, const char *, const char *);
  void renameFile(fs::FS &, const char *, const char *);
  void deleteFile(fs::FS &, const char *);
  void writeFile(fs::FS &, const char *, /*const char*/ String);
  void writeFile(fs::FS &, const char *, /*const char*/ uint32_t);
  void saveData(char,const char *);
  void saveData(char*,uint8_t,const char *);
