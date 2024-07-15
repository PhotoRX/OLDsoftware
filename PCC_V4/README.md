PCC_V4.ino have the followinh 

# Setup Function
## 1
```ruby
void setup() {
  uint8_t b1 = 0;
  
  ConfiguraSistema();
  Canales_ON_OFF(HIGH);            
  TRACE.begin(115200);
  MAESTRO.begin(115200);
#ifdef PRUEBAS
  _bTrace = true;
#else
  _bTrace = false;
#endif

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
 
  _bSisOK = InicializaDispositivos();    
  PRTRACE(F("\r\nFor help click IA[Intro]"));
}
```
A named variable of unsigned 8-bit integers initialized to 0.  Two functions named ConfiguraSistema(), and Canales_ON_OFF(HIGH) has been initiated. Also Serial & Serial1 for USB & UART1 initiated by TRACE.begin(115200), and MAESTRO.begin(115200).
## 1.1

```
ConfiguraSistema();
```
```ruby
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
```

