
/*
* 
*/
void initSDCard(){
  // Initialize SD card
  if(!SD.begin(P_CS0)){ //if no parameter, by default is CS = 5 //SD.begin(CS,spi,SPI_FREQ)
    TRACE.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    TRACE.println("No SD card attached");
    return;
  }
  TRACE.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    TRACE.println("MMC");
  } else if(cardType == CARD_SD){
    TRACE.println("SDSC");
  } else if(cardType == CARD_SDHC){
    TRACE.println("SDHC");
  } else {
    TRACE.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize()>>20; // / (1024 * 1024); //divide by 1024*1024 is equivalent to >>20
  TRACE.printf("SD Card Size: %lluMB\n", cardSize);
  createDir(SD,"/PhotoRX");
  //writeFile(SD,"/PhotoRX/ESP32.txt","ESP32 ID: "+(uint32_t)_chipid); //sobreescribe identificador del ESP32 usado
  //saveData("ESP32 ID:  ","/PhotoRX/MCUinfo.txt");
  snprintf(_tDest,sizeof(_tDest),"ESP32 ID: %d ",(uint32_t)_chipid); //avoids overflow error
  _RTC_INT.getDateTime(true).toCharArray(_tSource, sizeof(_tSource)); //turns back String to array (!) Original library casts array to String
  strncat(_tDest,_tSource,STR_L);
  saveData(_tDest,sizeof(_tDest), "/PhotoRX/MCUinfo.txt");
  memset(_tDest, 0, sizeof _tDest); //flush array
  snprintf(_tDest,sizeof(_tDest), "New session ");
  strncat(_tDest,_tSource,STR_L);
  saveData(_tDest,sizeof(_tDest),"/PhotoRX/Commandlog.txt");
  //saveData("New session "+_RTC_INT.getDateTime(true),"/PhotoRX/_cCommandlog.txt"); //uses String (!!!) Possible Overflow error
  /*
  sprintf_P(_tSal, PSTR("Modo=%s\tVelocidad=%u m*s\tRango=%s\tResolución=%dµV")
  , _tInt, oADC->getNumSPS(), _tEnt, oADC->getResolution());
  strncat, strcpy, sprintf, snprintf, sscanf
  */
}
/*
* Create a directory
*/
void createDir(fs::FS &fs, const char * path){
  TRACE.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    TRACE.println("Dir created");
  } else {
  TRACE.println("mkdir failed");
  }
}
/*
* @SaveData
*/
void saveData(char datos, const char* path)
{
  File archivo = SD.open(path, FILE_APPEND);
  if(archivo){
    //TRACE.print(datos);
    archivo.print(datos);
    //archivo.write(datos,i);
    archivo.close();    
  }
}
/*
* @SaveData overloaded
*/
void saveData(char datos[],uint8_t i, const char* path)
{
File archivo = SD.open(path, FILE_APPEND);
  if(archivo){
    //TRACE.print(datos);
    //archivo.print(datos);
    archivo.write((uint8_t*)datos,i);
    archivo.println("");
    archivo.close();    
  }
}
/*
* @SaveData overloaded
*/
void saveData(String datos, const char* path)
{
  File archivo = SD.open(path, FILE_APPEND);
  if(archivo){
    //TRACE.print(datos);
    archivo.println(datos);
    //archivo.write(datos,i);
    archivo.close();    
  }
}
/*
* Remove a directory
*/
void removeDir(fs::FS &fs, const char * path){
  TRACE.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    TRACE.println("Dir removed");
  } else {
    TRACE.println("rmdir failed");
  }
}
/*
* Opens file in read only mode
*/
void readFile(fs::FS &fs, const char * path){
  TRACE.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){ // !!! revisar qué hace
    TRACE.println("Failed to open file for reading");
    return;
  }

  TRACE.print("Read from file: ");
  while(file.available()){
    TRACE.write(file.read());
  }
  file.close();
}
/*
* Opens file in read+write mode and writes ASCII at the end of the file
*/
void appendFile(fs::FS &fs, const char * path, const char * message){
  TRACE.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    TRACE.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
    TRACE.println("Message appended");
  } else {
    TRACE.println("Append failed");
  }
  file.close();
}
/*
* Opens file in read+write mode and writes ASCII at the end of the file in a new line
*/
void appendlnFile(fs::FS &fs, const char * path, const char * message){
  TRACE.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    TRACE.println("Failed to open file for appending");
    return;
  }
  if(file.println(message)){
    TRACE.println("Message appended");
  } else {
    TRACE.println("Append failed");
  }
  file.close();
}
//Overloaded function
/*
* 
*/
void renameFile(fs::FS &fs, const char * path1, const char * path2){
  TRACE.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    TRACE.println("File renamed");
  } else {
    TRACE.println("Rename failed");
  }
}
/*
* 
*/
void deleteFile(fs::FS &fs, const char * path){
  TRACE.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    TRACE.println("File deleted");
  } else {
    TRACE.println("Delete failed");
  }
}
/*
* Overwrites content in existing file
*/
void writeFile(fs::FS &fs, const char * path, /*const char*/ String message){
  TRACE.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    TRACE.println("Failed to open file for writing");
  return;
  }
  if(file.print(message)){
    TRACE.println("File written");
  } else {
    TRACE.println("Write failed");
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, /*const char*/ uint32_t message){
  TRACE.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    TRACE.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    TRACE.println("File written");
  } else {
    TRACE.println("Write failed");
  }
  file.close();
}
/*
* List a directory
*/
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  TRACE.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    TRACE.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    TRACE.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      TRACE.print("  DIR : ");
      TRACE.println(file.name());
      if(levels){
      listDir(fs, file.name(), levels -1);
      }
    } else {
      TRACE.print("  FILE: ");
      TRACE.print(file.name());
      TRACE.print("  SIZE: ");
      TRACE.println(file.size());
    }
    file = root.openNextFile();
  }
}
