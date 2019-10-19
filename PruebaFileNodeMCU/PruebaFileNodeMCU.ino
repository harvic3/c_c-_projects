#include "FS.h"

#define outPin 16

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(outPin, OUTPUT);
  
  if (SPIFFS.begin()){
    Serial.println("Hola, FS started!");
    //Formatear la unidad
    if (SPIFFS.format()){
      Serial.println("FS formated!");
      return;
    }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);

  String path = "/data/data.txt";
  if (SPIFFS.exists(path)){
    SPIFFS.remove(path);
    Serial.println("Se eliminó " + path);
  }

  path = "/data/Prueba.bin";
  if (SPIFFS.exists(path)){
    SPIFFS.remove(path);
    Serial.println("Se eliminó " + path);
  }
    
  digitalWrite(outPin, HIGH);  
  delay(3000);
  digitalWrite(outPin, LOW);
}
