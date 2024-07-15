PCC_V4.ino have the followinh 

# Setup Function

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
