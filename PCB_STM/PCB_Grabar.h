/*
Versión 20200827

Module that controls data recording.

//The functions that manage files and directories are in SdFile within the Arduino directory, some are:
//uint8_t SdFile::remove(SdFile*, const char*);  delete file, returns true if it was deleted successfully.
//Suint8_t SdFile::rmDir(void);     delete subdirectory, it must be #define PRUE_ and without files.
//uint8_t SdFile::rmRfStar(void); deletes subdirectory, and all its contents.
//size_t SdFile::write(uint8_t b); size_t SdFile::write(const char* str); void SdFile::writeln_P(PGM_P str);
//size_t SdFile::write(const void* buf, uint16_t nbyte);  void SdFile::write_P(PGM_P str);

*/
#ifndef PCB_GRABAR_H
  #define PCB_GRABAR_H

  #include "PCB_STM.h"

  /* **********************************************
  // **** Declaration of variables ****************
  // *********************************************/
  #define     VAL_VEL_SPI 6
  #define SAL_PR        Serial        //defines the default output of the data
  
  uint8_t           _uimVelSPI[] = { 50, 36, 24, 18, 12, 8 };
  uint8_t           _uiVelSPI;
  SdFat           _sdfSD;
  SdFile            _fFich;
  bool            _bSD_Ok = false;
  char            _tIGra[L_TX];
  char *            _GraEven[]  =
  {
    "Card not initialized error",
    "Error exists but cannot be opened",
    "Error exists but it is not a directory",
    "It already exists",
    "Created correctly",
    "Error creating it",
    "Deleted successfully",
    "Error deleting",
    "Error opening"
  };

  // ****************************************************************
  // **** Function declaration *************************************
  // ****************************************************************
  bool      IniciaSD(void);
  bool      IniciaSD(uint8_t);
  bool      ParaSD();
  bool      ExisteFichero(int16_t, int16_t, char*, char*, char*);
  bool      Leer(char*);
  //void      ListaFicheros(print_t*, char*, bool = true);
  void      ListaFicheros(char*, bool = true);
  bool      CreaDirectorio(char*, bool = false);
  uint8_t     CreandoDirectorio(char*);
  bool      BorraDirectorio(char*);
  bool      BorraFichero(char*);
  uint8_t     NuevoFicheroDatos();
  bool      DaNombreFichero(char*, char*, char*, uint8_t);
  bool      FecharFichero(SdFile, uint8_t);
  bool      Grabar(char*, bool = true);
  bool      Grabar(char*, char*, bool, bool = true);
  bool      DaCabecera(int, char*, char*);

#endif
