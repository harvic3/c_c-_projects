#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRLib.h>

//creamos el objeto receptor con el pin11 como entrada de datos
IRrecv My_Receiver(11);
//creamos el objeto decodificador
IRdecode My_Decoder;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  Serial.begin(9600);//Iniciamos el monitor serie
  My_Receiver.enableIRIn(); //Iniciamos el receptor
  My_Receiver.blink13(true); //Habikitamos Debuger
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Wireless System");
  lcd.setCursor(2, 1);
  lcd.print("Audio/Video");
  delay(2000);
  lcd.clear();

}

void loop() {
  //lazo que captura la señal, decodifica y envía al monitor serie
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();//decoficacion de la señal
    My_Decoder.DumpResults();//volcado de resultados al monitor serie
    if (My_Decoder.bits > 8){      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Type: ");
      lcd.print(Pnames(My_Decoder.decode_type));
      lcd.setCursor(0, 1);
      lcd.print("Code: ");
      lcd.print(My_Decoder.value, HEX);
    }    
    My_Receiver.resume(); //reinicio del receptor
    
  }
}
