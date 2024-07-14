/*
Version 20210501

Module with common functions.


*/

#include "PCC_V4.h"


/* **********************************************
// **** Support functions **********************
// **********************************************/

//PCC version 20210501
//Extract a signed integer from a text
//iPos is the position that the number occupies within the text, starting from position 0.
//Any character that is not numeric marks the separation between one number and another.
//if it is not located, it returns false and does not modify the plVal value
bool LeeNumero(char tEnt[], int16_t *piVal, int16_t iPos) {
  int32_t lVal = *piVal;
  if (LeeNumero(tEnt, &lVal, iPos)) {
    *piVal = (int16_t)lVal;
    return true;
  }
  return false;
}

bool LeeNumero(char tEnt[], int32_t *plVal, int16_t iPos) {
  int16_t iLen, b1, b2, b3 = -1, b4;
  bool bEsNum = false;
  int32_t lVal;

  iLen = strlen(tEnt);              //to undo the modifications
  strcat(tEnt, ";");
  b2 = strlen(tEnt);
  for (b1=0; b1<b2; b1++) {
    if (tEnt[b1]>='0' && tEnt[b1]<='9') {   //es numérico.
      if (!bEsNum) {
        lVal = 0;             //resets the accumulator to 0.
        bEsNum = true;        //indicates that you have located the first digit.
        b3++;               //position counter.
        b4 = b1-1;              //to locate the sign.
      }
      else {
        lVal*= 10;              //next digit.
      }
      lVal+= (int32_t)(tEnt[b1]-'0');     //acumulador.
    }
    else {                    //non-numeric character.
      if (bEsNum) {             //just completed a number.
        if (b3==iPos) {           //is in the desired position.
          if (b4>=0) {          //Check if it has a negative sign.
            if (tEnt[b1]=='-') {    //It is a negative number.
              lVal = -lVal;
            }
          }
          *plVal = lVal;          //passes the value to the pointer.
          tEnt[iLen] = (char)0;     //leave as it was tEnt.
          return true;          //comes out with the correct number.
        }
      }
      bEsNum = false;             //the last character read is not numeric.
    }
  }
  tEnt[iLen] = (char)0;             //leave as it was tEnt.

  return false;                 //didn't find the number.
}

bool LeeNumero(char tEnt[], uint16_t *piVal, int16_t iPos) {
  uint32_t lVal = *piVal;
  if (LeeNumero(tEnt, &lVal, iPos)) {
    *piVal = (uint16_t)lVal;
    return true;
  }
  return false;
}

//The text sent here does not have to exceed the maximum plus 2.
bool LeeNumero(char tEnt[], uint32_t *plVal, int16_t iPos) {
  int16_t iLen, b1, b2, b3 = -1;
  bool bEsNum = false;
  uint32_t lVal;

  iLen = strlen(tEnt);              //to undo the modifications
  //Be careful, from here on, it is treated as a char array, not as text:
  tEnt[iLen] = ';';
  b2 = iLen + 1;
  for (b1=0; b1<b2; b1++) {
    if (tEnt[b1]>='0' && tEnt[b1]<='9') {   //It is numerical.
      if (!bEsNum) {
        lVal = 0;             //resets the accumulator to 0.
        bEsNum = true;        //indicates that you have located the first digit.
        b3++;               //position counter.
      }
      else {
        lVal*= 10;              //next digit.
      }
      lVal+= (uint32_t)(tEnt[b1]-'0');    //accumulator
    }
    else {                    //non-numeric character.
    
      if (bEsNum) {             //just completed a number.
        if (b3==iPos) {           //is in the desired position.
          *plVal = lVal;          //passes the value to the pointer.
          tEnt[iLen] = (char)0;     //leave as it was tEnt.
          return true;          //comes out with the correct number.
        }
      }
      bEsNum = false;             //the last character read is not numeric.
    }
  }
  tEnt[iLen] = (char)0;             //leave as it was tEnt.

  return false;                 //didn't find the number.
}

int16_t PosTexto(int16_t iCaIn, char *tEnt) {
  int16_t b1 = iCaIn;

  while(tEnt[b1]!=(char)0) {
    if (tEnt[b1]>' ') return b1;        //removes all characters <= to space.
    b1++;
  }
  return b1;
}

void Imprime(const __FlashStringHelper* tEnt, bool bRC) {
  strcpy_P(_tSal, (const char PROGMEM *)tEnt);
  Imprime(_tSal, bRC);
}

void Imprime(char * tEnt, bool bRC) {
  if (bRC) {
    MAESTRO.println(tEnt);  
  }
  else {
    MAESTRO.print(tEnt);  
  }
  PRTRACE(tEnt);
}

size_t DaLonFSH(const __FlashStringHelper *tFSH) {
  PGM_P pFSH = reinterpret_cast<PGM_P>(tFSH);
  size_t b1 = 0;

  while (true) {
    uint8_t uiCar = pgm_read_byte(pFSH++);
    if (uiCar==0) break;
    b1++;
  }

  return b1;
}

int16_t Valor(bool *pbOk, int16_t iPos, int16_t iLon, int16_t iMax, int16_t iMin, char *tEnt) {
  int16_t b1 = 0, b2, b3;

  *pbOk = false;
  b3 = iPos + iLon;
  for (b2=iPos; b2<b3; b2++) {
    if (tEnt[b2]<'0' || tEnt[b2]>'9') {
      return iMin;
    }
    b1*= 10;
    b1+= ((int16_t)tEnt[b2] - (int16_t)'0');
  }
  if (b1>iMax || b1<iMin)   return iMin;
  *pbOk = true;

  return b1;
}

char * AcotaTexto(char cCarIni, char cCarFin, char * tEnt) {
  int b1, iIni = 0, iLon = strlen(tEnt);
  for (b1=0; b1<iLon; b1++) {
    if (cCarIni==tEnt[b1])    iIni = b1;
    if (cCarFin==tEnt[b1]) {
      tEnt[b1] = (char)0;
      break;
    }
  }
  return &tEnt[iIni];
}
