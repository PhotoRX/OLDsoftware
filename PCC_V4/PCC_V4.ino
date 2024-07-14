// *****************************************************
// ************ Librerias: *****************************
// *****************************************************
#include <Wire.h>
#include "JJ_MCP4728.h"
#include "JJ_ADS1X15.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "PCC_V4.h"

// *****************************************************
// ************ Principal functions: *****************
// *****************************************************
void setup() {
  uint8_t b1 = 0;
  
  ConfiguraSistema();
  Canales_ON_OFF(HIGH);             //turn all channels OFF
  TRACE.begin(115200);
  MAESTRO.begin(115200);
#ifdef PRUEBAS
  _bTrace = true;
#else
  _bTrace = false;
#endif
  
  //Wait for characters to be received through either of the two channels or for the synchronization signal to go down:
  //while (TRACE.available()==0 && MAESTRO.available()==0) {
  //Wait for the master to turn OFF the synchronization pin
  while (digitalRead(C3_Sinc)==HIGH) {
#ifdef PRUEBAS
    b1++;
    TRACE.print(".");
    if (b1>20) {
      PRTRACE("\r\nTo start, press enter");
      b1 = 0;
    }
    delay(1000);
#endif
    //Checks if any character is sent from the TRACE:
    if (TRACE.available()>0) {
      _bTrace = true;     
      break;      
    }
  }
  PRTRACE();
  //Locates and initializes the two DACs, the two ADCs, and the 4 DS18B20 temperature sensors
  _bSisOK = InicializaDispositivos();       //true if detects all devices.
  PRTRACE(F("\r\nFor help click IA[Intro]"));
}

void loop() { 
#ifdef PRUEBAS
  digitalWrite(C3_Sinc, !digitalRead(C3_Sinc));
  if (_cTipTe && !_bTrabajo) {
    ActivaTest();
  }
#else
  uint32_t uTieAct = millis();
  //The second check is for overflow
  if (uTieAct>=_uTieUlt || (_uTieUlt-52)>uTieAct) {
    _uTieUlt = (uTieAct + 50L);         //varies the synchronization PIN every 50ms
    digitalWrite(C3_Sinc, !digitalRead(C3_Sinc));
  }
#endif  
  _uConBuc++;
  if (MAESTRO.available())  EntradaMAESTRO();
  if (TRACE.available())    EntradaTRACE();
  if (_bTrabajo)        Trabajando();
}

// *****************************************************
// ************ Common functions: *********************
// *****************************************************
bool EntradaTRACE() {
  uint32_t ulTie;
  uint8_t b1 = 0;

  ulTie = TIMEOUT + millis();           //reset the timeout
  while (true) {
    if (TRACE.available()>0) {
      ulTie = TIMEOUT + millis();       //reset the timeout
      _tEnt[b1] = TRACE.read();
      if (_tEnt[b1]=='\n') {
        _tEnt[b1] = (char)0;        //add final text
        EjecutaComando(_tEnt);
        return true;
      }
      if (_tEnt[b1]!='\r')  b1++;     //increments the counter whenever it is not '\r'.
      if (b1 >= (L_TX-3))   break;      //output by text length.
    }
    if (ulTie < millis())   break;      //output by TimeOut.
  }
  _tEnt[b1] = (char)0;              //add final text
  sprintf_P(_tSal, PSTR("wrong command(TRACE)\t[%s]"), _tEnt);
  PRTRACE(_tSal);
  
  return false;
}

bool EntradaMAESTRO() {
  uint32_t ulTie;
  uint8_t b1 = 0;

  ulTie = TIMEOUT + millis();           //reset the timeout
  while (true) {
    if (MAESTRO.available()>0) {
      ulTie = TIMEOUT + millis();       //reset the timeout
      _tEnt[b1] = MAESTRO.read();
      if (_tEnt[b1]=='\n') {
        _tEnt[b1] = (char)0;        //add final text
        EjecutaComando(_tEnt);
        return true;
      }
      if (_tEnt[b1]!='\r')  b1++;     //increments the counter whenever it is not '\r'.
      if (b1 >= (L_TX-3))   break;      //output by text length.
    }
    if (ulTie < millis())   break;      //output by TimeOut.
  }
  _tEnt[b1] = (char)0;              //add final text
  sprintf_P(_tSal, PSTR("wrong command(MAESTRO)\t[%s]"), _tEnt);
  PRTRACE(_tSal);
  
  return false;
}

bool EjecutaComando(char *tEnt) {
  int iLonTex = strlen(tEnt);
  
  //All commands must be at least 2 characters
  if (iLonTex<2)   return false;
  switch (tEnt[0]) {
    //What comes after the asterisk are commands, and they only support uppercase letters.
    case '*':
      strcpy_P(_tSal, PSTR("&NACK"));     //in case it ends badly
      switch (tEnt[1]) {
        //*A activate work mode *A.
        case 'A':         
          PRTRACE(F("Activating system."));
          if (!_bSisOK) {
            Imprime(PSTR("&NACK"));
            PRTRACE(F("System unavailable."));
            _bTrabajo = false;
            return true;
          }
          Imprime(PSTR("&A"));
          _bTrabajo = Activar(true);
          PRTRACE((_bTrabajo) ? F("System activated.") : F("System disabled."));
          return true;
        //*D deactivate work mode *D.
        case 'D':
          PRTRACE(F("Deactivating system."));
          Imprime(PSTR("&D"));
          _bTrabajo = Activar(false);
          return true;
        //*M shows the current status.
        case 'M':
          PRTRACE(F("shows the current status."));
          Imprime(PSTR("&M"));
          MuestraEstadoActual();
          break;;
        //*T Activate or deactivate the trace.
        case 'T':
          PRTRACE(F("Deactivating trace."));
          _bTrace = !_bTrace;
          PRTRACE(F("Activating trace."));
          return true;
        //Parameterization commands (SET)
        case 'S':
          switch (tEnt[2]) {
           // *SAy Alarm, sends the values ​​that generate caution or alarm events, maximum temperatures, maximum intensities, etc. If they are in error, they will perform an emergency stop.
            // *SA[tab][Maximum temperature] [tab][Danger temperature][tab][Hazard intensity]
            case 'A':         
              PRTRACE(F("Alarm parameters."));
              ParametrosAlarma(&tEnt[3], _tSal);
              break;
            // *SDy DAC, sends the values ​​that the DAC channel outputs will have. The order of the channels is from 1 to 8 (they correspond to the blocking diodes), if the value is 0, the channel will remain blocked.
            // *SD[tab][Channel value 1][T][Channel value 2][T][VC3][T][VC4][T][VC5][T][VC6][T][VC7][T][VC8]
            case 'D':
              PRTRACE(F("Intensity level."));
              ParametrosDAC(&tEnt[3], _tSal);
              break;
            // *STy Time, sends the time in seconds that each channel will remain ON. The order of the channels is from 1 to 8 (they correspond to the blocking diodes), if the value is 0, the channel will remain blocked.
            // *ST[tab][Channel time 1][T][Channel time 2][T][TC3][T][TC4][T][TC5][T][TC6][T][TC7][T][TC8]
            case 'T':
              PRTRACE(F("Duration time."));
              ParametrosTiempo(&tEnt[3], _tSal);
              break;
            // *SOy other configuration parameters.
            // *SO[tab][Board temperature sensors][T][Total temperature sensors]
            case 'O':
              PRTRACE(F("Other parameters."));
              ParametrosOtros(&tEnt[3], _tSal);
              break;
          }
          break;      
      }
      //If the command is not recognized, send a &NACK
      Imprime(_tSal);     
      return true;
    case 'I': case 'i':     
      switch (tEnt[1]) {
        case 't': case 'T':         
          VerSituacionADC(0, &_oADC_0);
          VerSituacionADC(1, &_oADC_1);
          return true;
        default:              //información.
          Ayuda();
          return true;
      }
      break;    
#ifdef PRUEBAS      
    case 't': case 'T':
      _cTipTe = tEnt[1];
      switch (tEnt[1]) {
        //TA\tPuts the DAC in ladder mode, and activates the reading of the ADCs and temperature sensors.
        case 'a': case 'A':         
          PRTRACE(F("DAC check on ladder."));
          return true;
        //TBxy\tSets the ADC to read, 'x'=channel, 'y'=range[0=±6.144mV; 1=±4.096mV; 2=±2.048mV; 3=±1.024mV; 4=±512mV; 5=±256mV
        case 'b': case 'B':
          PRTRACE(F("ADC reading:"));
          PreparaPrueba_B(tEnt[2], tEnt[3]);
          return true;
        //TCxy\tThe DAC will put the value 'y' in channel 'x'[0;9]*100mV\r\n\t\tEj: 400mV en channel B -> TC14
        case 'c': case 'C':
          PRTRACE(F("Write value to dac:."));
          PreparaPrueba_C(tEnt[2], tEnt[3]);
          return true;
        //TD\tShows the temperature of all the sensors it finds.
        case 'd': case 'D':
          if (!PreparaTemperatura()) {
            _cTipTe = 0;
            return false;
          }
          PRTRACE(F("Read temperature."));
          return true;
        case 'e': case 'E':
          PRTRACE(F("Speed Check."));
          PreparaPrueba_E();
          return true;
        case 'f': case 'F': default:  //ends the tests.
          Canales_ON_OFF(HIGH);
          PRTRACE(F("ends the tests."));
          _cTipTe =  (char)0;
          return true;
      }
      _cTipTe =  (char)0;
      break;
#endif      
  }
  sprintf_P(_tSal, PSTR("Unknown command:\r\n%d[%s]"), strlen(tEnt), tEnt);
  PRTRACE(_tSal);
  return false;
}

// *SAy Alarm, sends the values ​​that generate caution or alarm events, maximum temperatures, maximum intensities, etc. If they are in error, they will perform an emergency stop.
// *SA[tab][Maximum temperature] [tab][Hazard temperature][tab][Hazard intensity]
void ParametrosAlarma(char * tEnt, char * tSal) {
  //Extract the values:
  if (!LeeNumero(tEnt, &_iTemAvi, 0)) return;   //temperature fromcual genera un aviso.
  if (!LeeNumero(tEnt, &_iTemAla, 1)) return;   //temperature fromcual genera una alarma.
  if (!LeeNumero(tEnt, &_iIntMx, 2))  return;   //maximum intensity
  //You have successfully extracted the values, the formed command returns
  sprintf_P(tSal, PSTR("&SA\t%d\t%d\t%d"), _iTemAvi, _iTemAla, _iIntMx);
}

// *SDy DAC, sends the values ​​that the DAC channel outputs will have. The order of the channels is from 1 to 8 (they correspond to the blocking diodes), if the value is 0, the channel will remain blocked.
// *SD[tab][Channel value 1][T][Channel value 2][T][VC3][T][VC4][T][VC5][T][VC6][T][VC7][T][VC8]
void ParametrosDAC(char * tEnt, char * tSal) {
  uint8_t b1;
  //Extract the values:
  for (b1=0; b1<8; b1++) {
    if (!LeeNumero(tEnt, &_uVaCaDa[b1], b1))  return; //where it stores the DAC values
  }
  //You have successfully extracted the values, the formed command returns
  strcpy_P(tSal, PSTR("&SD"));  
  for (b1=0; b1<8; b1++) {
    sprintf_P(_tInt, PSTR("\t%d"), _uVaCaDa[b1]);
    strcat(tSal, _tInt);
  }
}

// *STy Time, sends the time in seconds that each channel will remain ON. The order of the channels is from 1 to 8 (they correspond to the blocking diodes), if the value is 0, the channel will remain blocked.
// *ST[tab][Tiempo canal 1][T][Tiempo canal 2][T][TC3][T][TC4][T][TC5][T][TC6][T][TC7][T][TC8]
void ParametrosTiempo(char * tEnt, char * tSal) {
  uint8_t b1;
  //Extract the values:
  for (b1=0; b1<8; b1++) {
    if (!LeeNumero(tEnt, &_uVaCaTi[b1], b1))  return; //where it stores the time values
  }
  //You have successfully extracted the values, the formed command returns
  strcpy_P(tSal, PSTR("&ST"));  
  for (b1=0; b1<8; b1++) {
    sprintf_P(_tInt, PSTR("\t%lu"), _uVaCaTi[b1]);
    strcat(tSal, _tInt);
  }
}

// *SOy other configuration parameters.
// *SO[tab][Temperature sensors per board][T][Total temperature sensors]
void ParametrosOtros(char * tEnt, char * tSal) {
  uint16_t uVal;
  
  //Extract the number of temperature sensors per board:
  if (!LeeNumero(tEnt, &uVal, 0) || uVal>10) {
    //Error in taking the number, or number of temperature sensors per board is too large.
    _uTemNSxP = TEM_N_SxP;            //number of temperature sensors per board by default.
    return;                   //exits with error.
  }
  _uTemNSxP = (uint8_t)uVal;            //number of temperature sensors per board.
  
  //Extracts the total number of temperature sensors:
  if (!LeeNumero(tEnt, &uVal, 1) || uVal>10) {
    //Error in taking the number, or total number of temperature sensors too large.
    _uTemNS = TEM_N_S;              //total number of temperature sensors.
    return;                   //exits with error.
  }
  _uTemNS = (uint8_t)uVal;            //total number of temperature sensors.

 //You have successfully extracted the values, return the formed command
  sprintf_P(tSal, PSTR("&ST\t%u\t%u"), _uTemNSxP, _uTemNS);
}

// *****************************************************
// ************ Support functions*********************
// *****************************************************
void Ayuda() {
  sprintf_P(_tSal, PSTR("Versión: %s\r\n"), VERSION);
  PRTRACE(_tSal);
  PRTRACE(F("Test commands:"));
  PRTRACE(F("Testing with DAC and ADC involves both at the same time"));
  PRTRACE(F("The channels go from 0 to 3, if 4 all four channels at the same time"));
  PRTRACE(F("Written in uppercase, they are executed continuously, in lowercase only once"));
  PRTRACE(F("TA\tPuts the DAC in ladder mode, and activates the reading of the ADCs and temperature sensors."));
  PRTRACE(F("TBxy\tSets the ADC to read, 'x'=channel, 'y'=range [0=±6.144mV; 1=±4.096mV; 2=±2.048mV; 3=±1.024mV; 4=±512mV; 5=±256mV"));
  PRTRACE(F("TCxy\tThe DAC will put the value 'y' in channel 'x'[0;9]*50mV "));
  PRTRACE(F("\t\tEj: 400mV in channel B -> TC18\t\tand checks it with the ADC"));
  PRTRACE(F("TD\tShows the temperature of all the sensors it finds."));
  PRTRACE(F("TF\tFinish the tests that are in continuous mode."));
  delay(20);
}

//Emergency stop HIGH=stop; LOW=start
void Canales_ON_OFF(uint8_t uiAct) {
  uint8_t b1;
  
  digitalWrite(MO_C1OFF1, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C1OFF2, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C1OFF3, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C1OFF4, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C2OFF1, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C2OFF2, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C2OFF3, uiAct);         //turns the channel OFF.
  digitalWrite(MO_C2OFF4, uiAct);         //TURN THE CHANNEL OFF.
  for (b1=0; b1<8; b1++) {
    _bCanON[b1] = (uiAct==LOW);         //indicates if the channel is on.
    _uValTie[b1] = (uiAct==LOW) ? 4000000000 : 0; //time at which channel switching ends 
  }
}

void ConfiguraSistema() {
  uint16_t uiVal;

  _uTieUlt = 0;
  _uCiclos = 0;
  _uTiPxTe = 0;
  _uTiPxIn = 0;
  _uNuCoTe = 0;
  _iIntMx = (int16_t)VOL_ERR;
  _cTipTe = (char)0;
  _bTrabajo = false;
  _iTemAvi = TEM_AVI;               //temperature from which it generates a warning.
  _iTemAla = TEM_ALA;               //temperature from which it generates an alarm.
  _uTemNS = TEM_N_S;                //ntotal number of temperature sensors.
  _uTemNSxP = TEM_N_SxP;              //number of temperature sensors per board.
  //ADC timing and detection pin ready.
  pinMode(C3_Sinc, OUTPUT_OPEN_DRAIN);      //output mode.
  digitalWrite(C3_Sinc, HIGH);          //waiting for signs
  pinMode(MI_C1RDY, INPUT_PULLUP);
  pinMode(MI_C2RDY, INPUT_PULLUP);

  //For all channels:
  pinMode(MO_C1OFF1, OUTPUT);           //output mode.
  digitalWrite(MO_C1OFF1, HIGH);          //turns the channel OFF.
  pinMode(MO_C1OFF2, OUTPUT);           //output mode.
  digitalWrite(MO_C1OFF2, HIGH);          //turns the channel OFF.
  pinMode(MO_C1OFF3, OUTPUT);           //output mode.
  digitalWrite(MO_C1OFF3, HIGH);          //turns the channel OFF.
  pinMode(MO_C1OFF4, OUTPUT);           //output mode.
  digitalWrite(MO_C1OFF4, HIGH);          //turns the channel OFF.
  pinMode(MO_C2OFF1, OUTPUT);           //output mode.
  digitalWrite(MO_C2OFF1, HIGH);          //turns the channel OFF.
  pinMode(MO_C2OFF2, OUTPUT);           //output mode.
  digitalWrite(MO_C2OFF2, HIGH);          //turns the channel OFF.
  pinMode(MO_C2OFF3, OUTPUT);           //output mode.
  digitalWrite(MO_C2OFF3, HIGH);          //turns the channel OFF.
  pinMode(MO_C2OFF4, OUTPUT);           //output mode.
  digitalWrite(MO_C2OFF4, HIGH);          //turns the channel OFF.

}

//Locates and initializes the two DACs, the two ADCs, and the 4 DS18B20 temperature sensors
bool InicializaDispositivos() {
  uint8_t b1;
  //Open the two I2C channels:
  Wire.begin();
  _owI2C_1.begin();

  //Modify the speed
  Wire.setClock(1000000UL);       //1 MHz for STM32
  _owI2C_1.setClock(1000000UL);

  //Scan connected devices:
  EscanearI2C();
  
  PRTRACE(F("\r\nDetected devices:"));
  //Activating and configuring DAC I2C channel 0
  if (_bDAC_OK[0]) {
    PRTRACE(F("DAC in I2C_0 0x60\tConfiguring (Registration-EEPROM):"));
    _oDAC_0.begin(Wire, MO_C1LDAC);
    ConfiguraDAC(0, &_oDAC_0);
  }
  //Activating and configuring DAC I2C channel 1
  if (_bDAC_OK[1]) {
    PRTRACE(F("DAC in I2C_1 0x60\tConfiguring (Registration-EEPROM):"));
    _oDAC_1.begin(_owI2C_1, MO_C2LDAC);
    ConfiguraDAC(1, &_oDAC_1);
  }
  
  //Activating and configuring ADC I2C channel 0
  if (_bADC_OK[0]) {
    PRTRACE(F("\r\nADC en I2C_0 0x48\tConfiguring:"));
    _oADC_0.begin(Wire);
    ConfiguraADC(0, &_oADC_0);
  }
  //Activating and configuring ADC I2C channel 1
  if (_bADC_OK[1]) {
    PRTRACE(F("\r\nADC en I2C_1 0x49\tConfiguring:"));
    _oADC_1.begin(_owI2C_1);
    ConfiguraADC(1, &_oADC_1);
  }
  //Interrupt activation:
  attachInterrupt(digitalPinToInterrupt(MI_C1RDY), INT_C0RDY, RISING);
  attachInterrupt(digitalPinToInterrupt(MI_C2RDY), INT_C1RDY, RISING);
  PRTRACE(F("End configuration I2C."));
  
  //Locating board temperature sensors 0
  _dtPCP0.begin();                        //initializes the temperature sensor.
  _dtPCP0.setWaitForConversion(false);      //does not wait for results, so as not to waste time.
  _uNumTer[0] = _dtPCP0.getDeviceCount();
  sprintf_P(_tSal, PSTR("\r\nLocalized temperature probes PCP_0: (%u)"), _uNumTer[0]);
  PRTRACE(_tSal);
  if (_uNumTer[0]>0) {
    for (b1=0; b1<_uNumTer[0]; b1++) {
      if (_dtPCP0.getAddress(_daPCP0[b1], b1)) {
        sprintf_P(_tSal, PSTR("ID%d: "), b1);
        ImprimeDireccion(_daPCP0[b1], _tSal);
        PRTRACE(_tSal);
      }
    }
    delay(5);                 //so that it looks good on the oscilloscope.
    _dtPCP0.requestTemperatures();        //sends the temperature request command.
  }

  //Locating board temperature sensors 1
  _dtPCP1.begin();                        //initializes the temperature sensor.
  _dtPCP1.setWaitForConversion(false);      //does not wait for results, so as not to waste time.
  _uNumTer[1] = _dtPCP1.getDeviceCount();
  sprintf_P(_tSal, PSTR("\r\nLocalized temperature probes PCP_1: (%u)"), _uNumTer[1]);
  PRTRACE(_tSal);
  if (_uNumTer[1]>0) {
    for (b1=0; b1<_uNumTer[1]; b1++) {
      if (_dtPCP1.getAddress(_daPCP1[b1], b1)) {
        sprintf_P(_tSal, PSTR("ID%d:"), b1);
        ImprimeDireccion(_daPCP1[b1], _tSal);
        PRTRACE(_tSal);
      }
    }
    delay(5);                 //so that it looks good on the oscilloscope.
    _dtPCP1.requestTemperatures();        //sends the temperature request command.
  } 
  delay(10);                    //so that it looks good on the oscilloscope.
  PRTRACE(F("End of temperature sensor configuration."));

  //Error management of the DAC, ADC and temperature sensors:
  for (b1=0; b1<2; b1++) {
    //Errors 32 and 33 are from the DAC
    if (!_bDAC_OK[b1]) {
      _uError = 32 + b1;            //Errors 32 and 33 are from the DAC
      sprintf_P(_tErr, PSTR(">ES\tError %u DAC\tI2C:%u"), _uError, b1);
      Imprime(_tErr);   
    }
    //Errors 34 and 35 are from the ADCs
    if (!_bADC_OK[b1]) {
      _uError = 34 + b1;            //errors 34 and 35 are from the ADCs
      sprintf_P(_tErr, PSTR(">ES\tError %u ADC\tI2C:%u"), _uError, b1);
      Imprime(_tErr);   
    }
    //Errors 36 and 37 are from the temperature sensors.
    if (_uNumTer[b1]<_uTemNSxP) {       //number of temperature sensors per board.number of temperature sensors per board.
      _uError = 36 + b1;            //Errors 36 and 37 are from the temperature sensors.
      sprintf_P(_tErr, PSTR(">ES\tError %u, temperature sensors detected\t%u"), _uError, _uNumTer[b1]);
      Imprime(_tErr);   
    }
  }

  //Returns true, if it locates the two DACs, the two ADCs, and at least one temperature sensor per board.
  return (_bDAC_OK[0] && _bDAC_OK[1] && _bADC_OK[0] && _bADC_OK[1] && (_uNumTer[0]>0) && (_uNumTer[1]>0));
}


void ImprimeDireccion(DeviceAddress daSonda, char * tSal) {
  for (uint8_t b1=0; b1<8; b1++) {
    sprintf_P(_tInt, PSTR(" 0x%2.2X,"), daSonda[b1]);
    strcat(tSal, _tInt);
  }
}
