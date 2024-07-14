/*
Version 20210501

Module with work functions.


*/

#include "PCC_V4.h"

// *****************************************************
// ************ Job functions *******************
// *****************************************************
bool Trabajando() {
  uint32_t uTieIni = millis();

  if (_bC0RDY) {
    _bTrabajo = LeeCanales(0);
    _bC0RDY = false;
  } 
  if (_bC1RDY) {
    _bTrabajo = LeeCanales(1);
    _bC1RDY = false;
  } 
  
  if (_uTiPxTe<=uTieIni) {
    MuestraADC(uTieIni, _tSal);
    CompruebaTemperatura(uTieIni, _tSal);
    _uTiPxTe+= 1000;              //Next temperature check.
    _bTrabajo = CompruebaTiempos(uTieIni);
  }

  return true;
}

bool CompruebaTiempos(uint32_t uTieIni) {
  uint8_t b1, b2 = 0;
  
  //Check the channels:
  for (b1=0; b1<8; b1++) {
    //I pass the checks
    if (_bCanON[b1]) {
      if (_uValTie[b1]>uTieIni) {       //completion time.
        b2++;               //increases the number of active channels.
      }
      else {
        digitalWrite(_uIdCan[b1], HIGH);  //turns the channel OFF.     
        _bCanON[b1] = false;        //completion time.
      }
    } 
  }
  
  //All channels have ended:
  if (b2==0) {
    _oDAC_0.analogWrite(0, 0, 0, 0);    
    _oDAC_1.analogWrite(0, 0, 0, 0);    
    return false;
  }
  return true;
}

bool Activar(bool bTrabajo) {
  uint8_t b1;
  uint32_t uTieIni;
  
  //Sets the DAC channels to 0.
  _oDAC_0.analogWrite(0, 0, 0, 0);    
  _oDAC_1.analogWrite(0, 0, 0, 0);    
  //Turns the channels OFF, _bCanON=false and _uValTie=0;
  for (b1=0; b1<8; b1++) {
    digitalWrite(_uIdCan[b1], HIGH);      //turns the channel OFF.
    _bCanON[b1] = false;            //indicates whether the channel is off.
    _uValTie[b1] = 0;             //completion time in ms.
  }
  if (!bTrabajo)        return false;
  if (!PreparaTemperatura())  return false;

  //Checks:
  //Time values ​​_uVaCaTi=time the channel will remain on in seconds:
  //DAC values; _uVaCaDa=value in LSB of the DAC channel; MX_VA_D=maximum value that the DAC cannot exceed.
  for (b1=0; b1<8; b1++) {
    if (_uVaCaTi[b1]==0 || _uVaCaDa[b1]==0 || _uVaCaDa[b1]>MX_VA_D) {
      _uVaCaTi[b1] = 0;           //time the channel will remain on in seconds.
      _uVaCaDa[b1] = 0;           //value in LSB of the DAC channel.
    }
    else {
      _bCanON[b1] = true;           //indicates whether the channel is on.     
    }
  }
  
  //Activate the DACs:
  //uint8_t _uCanDAC[] = { 0, 1, 2, 3, 7, 6, 5, 4 };
  _oDAC_0.analogWrite(_uVaCaDa[0], _uVaCaDa[1], _uVaCaDa[2], _uVaCaDa[3]);    
  _oDAC_1.analogWrite(_uVaCaDa[7], _uVaCaDa[6], _uVaCaDa[5], _uVaCaDa[4]);    
  //Activate the channels and prepare the times:
  uTieIni = millis();
  for (b1=0; b1<8; b1++) {
    //I pass the checks
    if (_bCanON[b1]) {
      digitalWrite(_uIdCan[b1], LOW);     //turns the channel ON.    
      _uValTie[b1] = (1000 * _uVaCaTi[b1]) + uTieIni; //completion time in ms.
    } 
  }
  for (b1=0; b1<1; b1++) {
    _uCaAc[b1] = 0;               //current ADC channel, [chip_ADC]
  }
  _oADC_0.requestADC(_uCaAc[0]);
  _oADC_1.requestADC(_uCaAc[1]);
  MuestraEstadoActual();
  LimpiaADC();
  _uTiPxIn = millis();          
  _uTiPxIn/= 100;
  _uTiPxIn+= 2;
  _uTiPxIn*= 100;
  
  return true;
}

void MuestraEstadoActual() {
  int16_t b1, b2;
  uint32_t uVal, uTieIni = millis();

  //>MA Caution and danger alarm messages.
//>MA[T][Current time (ms)][T][Maximum temperature (ºC)][T][Hazard temperature (ºC)][T][Hazard intensity (mA)]
  //Change the ADC values ​​from LSB to mA
  uVal = ((uint32_t)_iIntMx * 250)/1000;      //maximum current in mA
  sprintf_P(_tSal, PSTR(">MA\t%lu\t%d,%d\t%d,%d\t%lu"), uTieIni, (int16_t)(_iTemAvi/10), (int16_t)(_iTemAvi%10)
    , (int16_t)(_iTemAla/10), (int16_t)(_iTemAla%10), uVal);
  Imprime(_tSal);

  //>MD DAC output message in mV.
  //>MD[T][T_Act][T][DAC Voltage C1 (mV)][T][VDAC2][T][VDAC3][T][VDAC4][T][VDAC5][T][VDAC6][T][VDAC7][T][VDAC8]
  sprintf_P(_tSal, PSTR(">MD\t%lu"), uTieIni);
  for (b1=0; b1<8; b1++) {
    uVal = ((uint32_t)_uVaCaDa[b1] * 125)/1000; //DAC output in mV
    sprintf_P(_tInt, PSTR("\t%lu"), uVal);
    strcat(_tSal, _tInt);
  }
  Imprime(_tSal);

  //>MT Message completion time in ms.
  //>MT[T][T_Act][T][Channel End Time_1 (ms)][T][TFC2][T][TFC3][T][TFC4][T][TFC5][T][TFC6][T][ TFC7][T][TFC8]
  sprintf_P(_tSal, PSTR(">MT\t%lu"), uTieIni);
  for (b1=0; b1<8; b1++) {
    sprintf_P(_tInt, PSTR("\t%lu"), _uValTie[b1]);
    strcat(_tSal, _tInt);
  }
  Imprime(_tSal);

  //>MA Channel status message, 0=OFF 1=ON.
  //>MA[T][T_Act][T][Channel status 1][T][EC2][T][EC3][T][EC4][T][EC5][T][EC6][T][EC7][T][EC8]
  sprintf_P(_tSal, PSTR(">MA\t%lu"), uTieIni);
  for (b1=0; b1<8; b1++) {
    uVal = ( _bCanON[b1]) ? 1 : 0;        //1=ON 0 = OFF
    sprintf_P(_tInt, PSTR("\t%lu"), uVal);
    strcat(_tSal, _tInt);
  }
  Imprime(_tSal);
}

//Returns true if the overtemperature alarm has occurred.
bool CompruebaTemperatura(uint32_t uTieIni, char *tSal) {
  bool bAlarma = false;
  uint8_t b1;
  int16_t iTemAct;
  
  //IT temperature information package
  sprintf_P(tSal, PSTR(">IT\t%lu\t%lu"), _uNuCoTe, uTieIni);
  for (b1=0; b1<_uNumTer[0]; b1++) {
    iTemAct = (int16_t)(_dtPCP0.getTempC(_daPCP0[b1]) * 10);
    bAlarma|= AlarmaTemperatura(0, b1, iTemAct, uTieIni);
    sprintf_P(_tInt, PSTR("\t%d"), iTemAct);
    strcat(tSal, _tInt);
  }
  for (b1=0; b1<_uNumTer[1]; b1++) {
    iTemAct = (int16_t)(_dtPCP1.getTempC(_daPCP1[b1]) * 10);
    bAlarma|= AlarmaTemperatura(1, b1, iTemAct, uTieIni);
    sprintf_P(_tInt, PSTR("\t%d"), iTemAct);
    strcat(tSal, _tInt);
  }
  PeticionTemperatura();
  Imprime(tSal);
  _uNuCoTe++;

  return bAlarma;
}

//True if an alarm message has been generated
bool AlarmaTemperatura(uint8_t uPlaca, uint8_t uSensor, int16_t iTemp, uint32_t uTieIni) {
  //Generates an alarm message (ET temperature error). 
  if (iTemp>=_iTemAla) {        //temperature from which it generates an alarm.
    _uError = 121 + (uPlaca * 2) + uSensor;   //errors from 121 to 124 are temperature
    sprintf_P(_tErr, PSTR(">ET\tError %u temperatura\tPlaca:%u\tSensor:%u\tValor:%d\t%lu\t%lu")
      , _uError, uPlaca, uSensor, iTemp, _uNuCoTe, uTieIni);
    Imprime(_tErr);
    return true;
  }
  //Generates a caution warning (PT=temperature warning).
  if (iTemp>=_iTemAvi) {        //temperature from which it generates a warning.
    sprintf_P(_tErr, PSTR(">PT\tPeligro temperatura alta\t%lu\t%lu\tPlaca:%u\tSensor:%u\tValor:%d")
      , _uNuCoTe, uTieIni, _uError, uPlaca, uSensor, iTemp);
    Imprime(_tErr);
  }       

  return false;
}

uint8_t PeticionTemperatura() {
  if (_uNumTer[0]>0)  _dtPCP0.requestTemperatures();      //sends the temperature request command.
  if (_uNumTer[1]>0)  _dtPCP1.requestTemperatures();      //sends the temperature request command.
  return _uNumTer[0] + _uNumTer[1]; 
}

bool LeeCanales(uint8_t uC_I2C) {
  int16_t iVal;
  uint32_t uTieIni = micros();
  
  //Lee el valor:
  if(uC_I2C==0) {
    iVal = _oADC_0.getValue();
  }
  else {
    iVal = _oADC_1.getValue();
  }
  //Localiza el mínimo:
  if (_iVMin[uC_I2C][_uCaAc[uC_I2C]]>iVal) {
    _iVMin[uC_I2C][_uCaAc[uC_I2C]] = iVal;    //minimum ADC value, [chip_ADC][canal_ADC]
  }
  //Locate the maximum and check overcurrent:
  if (_iVMax[uC_I2C][_uCaAc[uC_I2C]]<iVal) {
    _iVMax[uC_I2C][_uCaAc[uC_I2C]] = iVal;    //maximum ADC value, [chip_ADC][canal_ADC]
    if (iVal>_iIntMx) {             //Errors from 138 to 145
      return ErrorADC((138 + (4 * uC_I2C) + _uCaAc[uC_I2C]), iVal);
    }
  }
  //Stores the value and number of measurements for calculating averages:
  _iVPro[uC_I2C][_uCaAc[uC_I2C]]+= (int32_t)iVal; //ADC average value,[chip_ADC][canal_ADC]
  _uNMed[uC_I2C][_uCaAc[uC_I2C]]++;       //number of ADC measurements, [chip_ADC][canal_ADC]
  //Increase ADC channel:
  _uCaAc[uC_I2C]++;               //current ADC channel, [chip_ADC]
  if (_uCaAc[uC_I2C]>3) _uCaAc[uC_I2C] = 0;   //has traveled all 4 channels.
  //Make the conversion request for the new channel:
  if(uC_I2C==0) {
    _oADC_0.requestADC(_uCaAc[uC_I2C]);
  }
  else {
    _oADC_1.requestADC(_uCaAc[uC_I2C]);
  }
  
  _uTiUs[uC_I2C]+= (micros() - uTieIni);
  
  return true;
}

bool ErrorADC(uint8_t uError, int16_t iVal) {
  Canales_ON_OFF(HIGH);             //Emergency stop.
  _uError = uError;
  sprintf_P(_tErr, PSTR(">EI\tError %u ADC\t%d"), _uError, iVal);
  Imprime(_tErr);
  return false;
}

void MuestraADC(uint32_t uTieIni, char * tSal) {
  uint8_t b1, b2;
  int16_t iVPro;
  uint16_t  uVal0, uVal1;
  
  //Error 146
  for (b1=0; b1<4; b1++) {
    if (_uNMed[0][b1]==0 || _uNMed[1][b1]==0) {
      ErrorADC(146, 0);
      return;
    }
  }
  
  //uint8_t _uCanADC[] = { 0, 1, 2, 3, 3, 2, 1, 0 };
  //>IN Information on minimum intensity.
  //>IN[T][N_P][T][T_Act][T][Channel_Minimum_Intensity_0][T][INC1][T][INC2][T][INC3][T][INC4][T][INC5][T][INC6][T][INC7]
  sprintf_P(tSal, PSTR(">IN\t%lu\t%lu"), _uNuCoTe, uTieIni);
  for (b1=0; b1<8; b1++) {
    if (b1>3) b2 = 1;   else  b2 = 0;
    sprintf_P(_tInt, PSTR("\t%ld"), _oADC_0.toMilliVolts(_iVMin[b2][_uCanADC[b1]]));
    strcat(tSal, _tInt);
  }
  Imprime(tSal);
  _uNuCoTe++;
  
  //>IP Information on average intensity.
  //>IP[T][N_P][T][T_Act][T][Channel_Average_Intensity_0][T][IPC1][T][IPC2][T][IPC3][T][IPC4][T][IPC5][T][IPC6][T][IPC7]
  sprintf_P(tSal, PSTR(">IP\t%lu\t%lu"), _uNuCoTe, uTieIni);
  for (b1=0; b1<8; b1++) {
    if (b1>3) b2 = 1;   else  b2 = 0;
    iVPro = (int16_t)(_iVPro[b2][_uCanADC[b1]] / (int32_t)_uNMed[b2][_uCanADC[b1]]);
    sprintf_P(_tInt, PSTR("\t%ld"), _oADC_0.toMilliVolts(iVPro));
    strcat(tSal, _tInt);
  }
  Imprime(tSal);
  _uNuCoTe++;
  
  //>IX Information on average intensity.
  //>IX[T][N_P][T][T_Act][T][Channel_Maximum_Intensity_0][T][IXC1][T][IXC2][T][IXC3][T][IXC4][T][IXC5][T][IXC6][T][IXC7]
  sprintf_P(tSal, PSTR(">IX\t%lu\t%lu"), _uNuCoTe, uTieIni);
  for (b1=0; b1<8; b1++) {
    if (b1>3) b2 = 1;   else  b2 = 0;
    sprintf_P(_tInt, PSTR("\t%ld"), _oADC_0.toMilliVolts(_iVMax[b2][_uCanADC[b1]]));
    strcat(tSal, _tInt);
  }
  Imprime(tSal);
  _uNuCoTe++;

  //TRACE information only
  uVal0 = 0;
  uVal1 = 0;
  for (b1=0; b1<4; b1++) {
    uVal0+= _uNMed[0][b1];
    uVal1+= _uNMed[1][b1];
  }
  sprintf_P(_tSal, PSTR("Med=(%u;%u)\tRDY=(%u;%u)\tOcupa(µs)=(%lu;%lu)\tbuc=%lu\t%lu")
    , uVal0, uVal1, _vuAcRDY[0], _vuAcRDY[1], _uTiUs[0], _uTiUs[1], _uConBuc, uTieIni);
  PRTRACE(_tSal);
  
  LimpiaADC();
}

void LimpiaADC() {
  uint8_t b1, b2;
  
  _uConBuc = 0;
  for (b1=0; b1<2; b1++) {    
    for (b2=0; b2<4; b2++) {
      _iVMax[b1][b2] = -16000;        //maximum ADC value, [chip_ADC][canal_ADC]
      _iVMin[b1][b2] = 16000;         //minimum ADC value, [chip_ADC][canal_ADC]
      _iVPro[b1][b2] = 0;           //ADC average value, [chip_ADC][canal_ADC]
      _uNMed[b1][b2] = 0;           //number of ADC measurements, [chip_ADC][canal_ADC]
    }
    _uTiUs[b1] = 0;               //ADC Occupancy Time, [chip_ADC]
    _vuAcRDY[b1] = 0;             //number of ADC RDY PIN activations, [chip_ADC]
  } 
}

void VerSituacionDAC(uint8_t uCan, JJ_MCP4728 * oDAC) {
  sprintf_P(_tSal, PSTR("DAC_%u\t\tVref   \t\tGain   \t\tPwDown \t\tDACData"), uCan);
  PRTRACE(_tSal);
    for (int b1 = 0; b1 < 4; ++b1) {
    sprintf_P(_tSal, PSTR("DAC_%c\t\t0x%X-0x%X\t\t0x%X-0x%X\t\t0x%X-0x%X\t\t%u-%u")
      , (char)(b1+'A'), oDAC->getVref(b1), oDAC->getVref(b1, true)
      , oDAC->getGain(b1), oDAC->getGain(b1, true)
      , oDAC->getPowerDown(b1), oDAC->getPowerDown(b1, true)
      , oDAC->getDACData(b1), oDAC->getDACData(b1, true));
    PRTRACE(_tSal);
    }
}

void ConfiguraDAC(uint8_t uCan, JJ_MCP4728 * oDAC) {
    oDAC->readRegisters();
  VerSituacionDAC(uCan, oDAC);
    oDAC->selectVref(DAC_R_2V8, DAC_R_2V8, DAC_R_2V8, DAC_R_2V8);
    oDAC->selectPowerDown(DAC_O_ST, DAC_O_ST, DAC_O_ST, DAC_O_ST);
  //DAC_G_X1 o DAC_G_X2 only works when selectVref=DAC_R_2V8, si = DAC_R_VDD; selectGain=DAC_G_X1
    oDAC->selectGain(DAC_G_X1, DAC_G_X1, DAC_G_X1, DAC_G_X1); 
    oDAC->analogWrite(0, 0);
    oDAC->analogWrite(1, 0);
    oDAC->analogWrite(2, 0);
    oDAC->analogWrite(3, 0);
    oDAC->enable(true);
    oDAC->readRegisters();
  VerSituacionDAC(uCan, oDAC);
}

#ifdef ADS16B
  bool ConfiguraADC(uint8_t uCan, JJ_ADS1115 * oADC) {
#else
  bool ConfiguraADC(uint8_t uCan, JJ_ADS1015 * oADC) {
#endif
  if (!oADC->isConnected()) {
    sprintf_P(_tSal, PSTR("\r\nADC_%u desconectado."), uCan);
    PRTRACE(_tSal);
    return false;
  }
  VerSituacionADC(uCan, oADC);
  
  //Gain:   0=±6.144mV(defecto); 1=±4.096mV; 2=±2.048mV; 3=±1.024mV; 4=±512mV; 5=±256mV
  oADC->setGain(ADS1X1X_RANGO_512);       //±512mV
  //Speed:  0=128, 1=250, 2=490, 3=920, 4=1.600, 5=2.400, 6=3.300, 7=3.300
  oADC->setDataRate(6);             //3.300mxs
  //Setting PIN RDY:
  oADC->setComparatorThresholdHigh(0x8000);
  oADC->setComparatorThresholdLow(0x0000);
  oADC->setComparatorQueConvert(0);       //a single sample triggers the comparator
  //Mode: 0=ADS1X1X_MODE_CONTINUE=continuous mode;  1=ADS1X1X_MODE_SINGLE=single measurement mode
  oADC->setMode(ADS1X1X_MODE_SINGLE);       //single measurement mode.
  
  VerSituacionADC(uCan, oADC);
  return true;
}

#ifdef ADS16B
  void VerSituacionADC(uint8_t uCan, JJ_ADS1115 * oADC) {
#else
  void VerSituacionADC(uint8_t uCan, JJ_ADS1015 * oADC) {
#endif
  oADC->getMode(_tInt);
  oADC->getRango(_tEnt);
  sprintf_P(_tSal, PSTR("Modo=%s\tVelocidad=%u m*s\tRango=%s\tResolución=%dµV")
    , _tInt, oADC->getNumSPS(), _tEnt, oADC->getResolution());
  PRTRACE(_tSal);

  //ComparatorMode: Modo=0  Nivel=1 LATCH=0 Alerta=3
  // 0    = TRADITIONAL   > high          => on      < low   => off
  // else = WINDOW        > high or < low => on      between => off

  //ComparatorPolarity
  // 0    = LOW (default)
  // else = HIGH

  //ComparatorLatch
  // 0    = NON LATCH
  // else = LATCH

  //ComparatorQueConvert
  // 0   = trigger alert after 1 conversion
  // 1   = trigger alert after 2 conversions
  // 2   = trigegr alert after 4 conversions
  // 3   = Disable comparator =  default, also for all other values.
  
  sprintf_P(_tSal, PSTR("Comparador:\tModo=%u\tNivel=%u\tLATCH=%u\tNumCon=%u")
    , oADC->getComparatorMode(), oADC->getComparatorPolarity()
    , oADC->getComparatorLatch(), oADC->getComparatorQueConvert());
  PRTRACE(_tSal);
}

void EscanearI2C() {
  byte bError, bDirDis;
  uint8_t b1;
  uint32_t uTieIni, uTieInt, uTieFin;
  int iNuDiC[2];

  //Clear the booleans
  for (b1=0; b1<2; b1++) {
    _bDAC_OK[b1] = false;
    _bADC_OK[b1] = false;
    iNuDiC[b1] = 0;
  }

  PRTRACE(F("Searching for devices in I2C_0 y I2C_1"));
  delay(10);                    //
  //The return value of "endTransmisstion" is used to see if a device responds to the requested address.
  uTieIni = millis();
  for(bDirDis=1; bDirDis<127; bDirDis++) {
    Wire.beginTransmission(bDirDis);
    bError = Wire.endTransmission();
    iNuDiC[0]+= MuestraEscanerI2C(bDirDis, bError, 0);
  }
  PRTRACE();
  uTieInt = millis();
  for(bDirDis=1; bDirDis<127; bDirDis++) {
    _owI2C_1.beginTransmission(bDirDis);
    bError = _owI2C_1.endTransmission();
    iNuDiC[1]+= MuestraEscanerI2C(bDirDis, bError, 1);
  }
  PRTRACE();
  uTieFin = millis();
  sprintf_P(_tSal, PSTR("Encontrados %d en I2C_0 (%lums) y %d en I2C_1 (%lums)")
    , iNuDiC[0], (uTieInt-uTieIni), iNuDiC[1], (uTieFin-uTieInt));
  PRTRACE(_tSal);
}

int MuestraEscanerI2C(byte bDirDis, byte bError, uint8_t uiCan) {
  if (bError == 0) {
    sprintf_P(_tSal, PSTR("I2C_%u->0x%2.2X"), uiCan, bDirDis);
    PRTRACE(_tSal);
    if (bDirDis==0x60)          _bDAC_OK[uiCan] = true;
    if (uiCan==0 && bDirDis==0x48)    _bADC_OK[uiCan] = true;
    if (uiCan==1 && bDirDis==0x49)    _bADC_OK[uiCan] = true;
    return 1;
  }
  else if (bError==4) {
    sprintf_P(_tSal, PSTR("\r\nUnknown error in I2C_%u: 0x%2.2X"), uiCan, bDirDis);
    PRTRACE(_tSal);
  }
  return 0;
}

bool PreparaTemperatura() {
  uint8_t uNuSe = PeticionTemperatura();
  
  _uTiPxTe = millis();          
  if (uNuSe<_uTemNS) {              //if there are no temperature sensors, return false.
    sprintf_P(_tErr, PSTR(">ET\tTemperature error 120, %u temperature probes have been detected %u\t%lu"), uNuSe, _uTemNS, _uTiPxTe);
    Imprime(_tErr);
    return false;
  }
  _uTiPxTe/= 1000;
  _uTiPxTe+= 2;
  _uTiPxTe*= 1000;
  
  return true;
}
