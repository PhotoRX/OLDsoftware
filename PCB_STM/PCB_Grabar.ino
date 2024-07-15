/*
Versión 20191012

Module that controls data recording.

//The functions that manage files and directories are in SdFile within the Arduino directory, some are:
//uint8_t SdFile::remove(SdFile*, const char*);  delete file, returns true if deleted successfully.
//Suint8_t SdFile::rmDir(void);       delete subdirectory, it must be open and without files.
//uint8_t SdFile::rmRfStar(void);     deletes subdirectory, and all its contents.
//size_t SdFile::write(uint8_t b);  size_t SdFile::write(const char* str);  void SdFile::writeln_P(PGM_P str);
//size_t SdFile::write(const void* buf, uint16_t nbyte);  void SdFile::write_P(PGM_P str);
*/

// **********************************************
// **** Libraries declaration ****************
// **********************************************
#include "PCB_Grabar.h"

// ****************************************************************
// **** Library functions ****************************************
// ****************************************************************

bool IniciaSD(uint8_t uiVelSPI) {
  //F_CPU/2 72/2=36
  //F_CPU/3 72/3=24
  //F_CPU/4 72/4=18
  //F_CPU/6 72/6=12
  //F_CPU/9 72/9=8
  //_bSD_Ok = _sdfSD.begin(P_SD_CS, SD_SCK_MHZ(24));
  _uiVelSPI = uiVelSPI;
  _bSD_Ok = false;
  while (!_bSD_Ok && _uiVelSPI<VAL_VEL_SPI) {
    _bSD_Ok = _sdfSD.begin(P_SD_CS, SD_SCK_MHZ(_uimVelSPI[_uiVelSPI]));
    _uiVelSPI++;
  }
  _uiVelSPI--;

  return _bSD_Ok;
}

bool IniciaSD() {
  //F_CPU/2 72/2=36
  //F_CPU/3 72/3=24
  //F_CPU/4 72/4=18
  //F_CPU/6 72/6=12
  //F_CPU/9 72/9=8
  //_bSD_Ok = _sdfSD.begin(P_SD_CS, SD_SCK_MHZ(24));
  _uiVelSPI = 0;
  _bSD_Ok = false;
  while (!_bSD_Ok && _uiVelSPI<VAL_VEL_SPI) {
    _bSD_Ok = _sdfSD.begin(P_SD_CS, SD_SCK_MHZ(_uimVelSPI[_uiVelSPI]));
    _uiVelSPI++;
  }
  _uiVelSPI--;

  return _bSD_Ok;
}

bool ParaSD() {
  _bSD_Ok = false;
  return _bSD_Ok;
}

//Makes a file name with a number up to 4 digits (10,000 elements) if it does not exist it returns true, if it exists false
//int16_t iCif Figures that will have the added number. If it is 0, it does not take it into account, it only checks the alphabetical fields.
//int16_t iVal Number that will be added to the file name.
//char *nFichC File name header. It cannot be less than 1 character or more than 5.
//char *nFichX File extension.  It cannot be less than 1 character or more than 3.
//char *nFich Full name of the file.
bool ExisteFichero(int16_t iCif, int16_t iVal, char *nFichC, char *nFichX, char *nFich) {
  int16_t b1, b2;
  char cNum[8];
  SdFile fFich;
  strcpy_P(nFich, PSTR("nulo"));

  //Protections:
  if (!_bSD_Ok) {               //uninitialized card.
    SAL_PR.println(_GraEven[0]);
    return false;
  }
  if (iCif>4 || iCif<0)     return false;
  b1 = strlen(nFichC);
  if (b1<1 || b1>(8-iCif))    return false;
  b1 = strlen(nFichX);
  if (b1<1 || b1>3)       return false;
  if (iCif==0) {
    sprintf_P(nFich, PSTR("%s.%s"), nFichC, nFichX);
  }
  else {
    b2 = 1;
    for (b1=0; b1<iCif; b1++) b2*= 10;
    if (iVal<0 || iVal>=b2)   return false;
    sprintf_P(cNum, PSTR("%d"), (b2+iVal));
    sprintf_P(nFich, PSTR("%s%s.%s"), nFichC, &cNum[1], nFichX);
  }

  //Verification of the existence of the file:
  if (fFich.open(nFich, O_READ)) {
    fFich.close();
    return true;
  }

  return false;
}

bool Leer(char * nFich) {
  int16_t b1;
  SdFile fFich;

  if (!_bSD_Ok) {                 //uninitialized card.
    SAL_PR.println(_GraEven[0]);
    return false;
  }
  //Open the file and read it:
  if (fFich.open(nFich, O_READ)) {
    sprintf_P(_tIGra, PSTR("\r\nDatos fichero %s:"), nFich);
    SAL_PR.println(_tIGra);
    while (true) {                //reading loop until end of file.
      b1 = fFich.read(_tIGra, L_TX_24);
      if (b1<1)     break;
      _tIGra[b1] = (char)0;
      SAL_PR.print(_tIGra);
      delay((uint32_t)L_TX_24);
    }
    fFich.close();                //close the file.
    return true;
  }
  //If you can't open it.
  sprintf_P(_tIGra, PSTR("Fichero %s-> %s"), nFich, _GraEven[8]);
  SAL_PR.println(_tIGra);

  return false;
}

//LS_DATE - %Print file modification date
//LS_SIZE - %Print file size.
//LS_R - Recursive list of subdirectories.
//void ListaFicheros(print_t * prSal, char * tDirEnt, bool bVerSub) {
void ListaFicheros(char * tDirEnt, bool bVerSub) {
  //Also print subdirectories
  if (bVerSub) {
    _sdfSD.ls(tDirEnt, LS_R | LS_SIZE | LS_DATE);
  }
  else {
    _sdfSD.ls(tDirEnt, LS_SIZE | LS_DATE);
  }
}

//Returns true, if it exists, or has been created.
bool CreaDirectorio(char *tDir, bool bTrace) {
  bool bEsDirOK = false;
  sprintf_P(_tIGra, PSTR("Creando directorio %s-> "), tDir);
  switch (CreandoDirectorio(tDir)) {
    case 0:
      strcpy_P(_tIGra, _GraEven[0]);    //Card not initialized error
      break;
    case 1:
      strcat_P(_tIGra, _GraEven[1]);    //Error exists but cannot be opened
      break;
    case 2:
      strcat_P(_tIGra, _GraEven[2]);    //Error exists but it is not a directory
      break;
    case 3:
      strcat_P(_tIGra, _GraEven[3]);    //It already exists
      bEsDirOK = true;
      break;
    case 4:
      strcat_P(_tIGra, _GraEven[4]);    //Successfully created
      bEsDirOK = true;
      break;
    case 5:
      strcat_P(_tIGra, _GraEven[5]);    //Error creating it
      break;
  }
  if (bTrace) SAL_PR.println(_tIGra);

  return bEsDirOK;
}

uint8_t CreandoDirectorio(char *tDir) {
  SdFile sdfDir;
  uint8_t b1;

  if (!_bSD_Ok)       return 0;   //uninitialized card.
  if (_sdfSD.exists(tDir)) {
    b1 = 1;
    if (sdfDir.open(tDir, O_RDONLY)) {
      b1++;               //b1 = 2
      if (sdfDir.isDir()) b1++;     //b1 = 3
      sdfDir.close();
    }
    return b1;
  }
  if (_sdfSD.mkdir(tDir))   return 4;

  return 5;
}

bool BorraDirectorio(char *tDir) {
  if (!_bSD_Ok) {               //uninitialized card.
    SAL_PR.println(_GraEven[0]);
    return false;
  }
  if (_sdfSD.rmdir(tDir)) {         //Deleted successfully
    sprintf_P(_tIGra, PSTR("Directorio %s-> %s"), tDir, _GraEven[6]);
    SAL_PR.println(_tIGra);
    return true;
  }
  //Error al borrar
  sprintf_P(_tIGra, PSTR("Directorio %s-> %s"), tDir, _GraEven[7]);
  SAL_PR.println(_tIGra);

  return false;
}

bool BorraFichero(char * nFich) {
  if (!_bSD_Ok) {               //uninitialized card.
    SAL_PR.println(_GraEven[0]);
    return false;
  }
  if (_sdfSD.remove(nFich)) {         //Deleted successfully
    sprintf_P(_tIGra, PSTR("Fichero %s-> %s"), nFich, _GraEven[6]);
    SAL_PR.println(_tIGra);
    return true;
  }
  //Error al borrar
  sprintf_P(_tIGra, PSTR("Fichero %s-> %s"), nFich, _GraEven[7]);
  SAL_PR.println(_tIGra);

  return false;
}

bool DaNombreFichero(char *cCab, char *cExt, char * nFich, uint8_t uiOrd) {
  if (uiOrd>(uint8_t)99)      return false;
  sprintf_P(nFich, PSTR("%s%2.2d.%s"), cCab, uiOrd, cExt);

  return true;
}

bool FecharFichero(SdFile fFich, uint8_t uiFlags) {
  _rtcTie = _rtcVal.getTime();
  _rtcVal.epochToDate(_rtcTie, _dvFecha);
  _rtcVal.epochToTime(_rtcTie, _tvHora);  

  return (bool)fFich.timestamp(uiFlags, _dvFecha.year, _dvFecha.month, _dvFecha.day
    , _tvHora.hours, _tvHora.minutes, _tvHora.seconds); 
  
/*

#ifdef PUNTERO_ON
  _pstmT = _oRTC.getTime(_pstmT);
  return (bool)fFich.timestamp(uiFlags, _pstmT->tm_year + 1900
    , _pstmT->tm_mon + 1, _pstmT->tm_mday, _pstmT->tm_hour, _pstmT->tm_min, _pstmT->tm_sec);
#else
  _oRTC.getTime(_stmT);
  return (bool)fFich.timestamp(uiFlags, _stmT.year + 1900, _stmT.month, _stmT.day
    , _stmT.hour, _stmT.minute, _stmT.second);
#endif
*/
}

//nFich = [File name (if it does not exist, it creates it)]@[Text to record (adds it at the end)].
//bDate (default true)->if true = will date the file.
bool Grabar(char * nFich, bool bFecha) {
  int16_t b1, b2;
  char * tEnt;

  //Separate the name from the text to be recorded.
  b2 = strlen(nFich);
  if (b2<2)           return false; //there is no name
  for (b1=0; b1<b2; b1++) {
    if (nFich[b1]=='@') {
      nFich[b1] = (char)0;
      b1++;
      tEnt = &nFich[b1];
      break;
    }
  }

  return Grabar(nFich, tEnt, bFecha);
}
//nFich = File name (if it doesn't exist, create it).
//tEnt = Text to record (adds it to the end).
//bDate->if true = will date the file.
//bCrLf (default true)->if true = add \r\n to the end.
bool Grabar(char * nFich, char * tEnt, bool bFecha, bool bCrLf) {
  uint8_t uiEti;
  SdFile fFich;

  if (!_bSD_Ok) {                 //uninitialized card.
    SAL_PR.println(_GraEven[0]);
    return false;
  }
  if (strlen(nFich)<2)      return false; //there is no name
  if (strlen(tEnt)<1)       return false; //there is no text

  //Open the file and save:
  if (fFich.open(nFich, O_APPEND | O_WRITE | O_CREAT)) {
    if (fFich.fileSize()<5) {
      uiEti = T_ACCESS | T_CREATE | T_WRITE;
    }
    else {
      uiEti = T_WRITE;
    }
  
    if (bFecha) FecharFichero(fFich, uiEti);
    if (bCrLf)    fFich.println(tEnt);  else  fFich.print(tEnt);
    fFich.close();                //close the file.
    return true;
  }
  sprintf_P(_tIGra, PSTR("Fichero %s-> %s"), nFich, _GraEven[8]);
  SAL_PR.println(_tInt);

  return false;
}

//Returns a new file number that does not exist, if all exist it returns 0:
uint8_t NuevoFicheroDatos() {
  for (uint8_t b1=0; b1<100; b1++) {
    if (!ExisteFichero(2, (int16_t)b1, "Datos", "txt", _tSal)) {
      return b1;
    }
  }
  return 0;
}

bool DaCabecera(int iLTx, char * nFich, char * tSal) {
  int16_t b1;//, b2;
  SdFile fFich;

  if (!_bSD_Ok) {                 //uninitialized card.
    SAL_PR.println(_GraEven[0]);
    return false;
  }
  b1 = 0;
  //b2 = sizeof(tSal-1);
  //Open the file and read it:
  if (fFich.open(nFich, O_READ)) {
    b1 = fFich.read(tSal, iLTx);
    if (b1>0) tSal[b1] = (char)0;
    fFich.close();                //close the file.
  }

  return (b1>0);
}
