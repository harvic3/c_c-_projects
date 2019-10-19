#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define DHTTYPE       DHT11
#define ONE_WIRE_BUS  4
#define PinDHT        5
#define PinServoB     8
#define PinServoP     9
#define PinInterrup   2
#define posBar        0 //Posicion de reposo Barra
#define maxPosPlat    40 //Maxima posicion de la plataforma


//Objects and variables
DHT dht(PinDHT, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
Servo platform;
Servo bar;
DallasTemperature sensorCompost(&oneWire);

double tempMixture;
double humMixture;
double tempCompost;

void setup() {
  Serial.begin(9600);
  dht.begin();
  sensorCompost.begin();
  platform.attach(PinServoP);
  bar.attach(PinServoB);
  initialize();
}

void loop() {    
  for (int x = 0; x < 10; x++){
    mezclar();
    verifyParams();
  }
  vaciar();    
  for (int i = 0; i < 10; i++){
    verifyParams();    
    delay(1000);
  }
  initialize();  
}

void verifyParams(){
  readSensorTH();
  readsensorCompost();
  printData();
}

void initialize(){  
  platform.write(0);
  delay(3000);
  bar.write(0);
}

void vaciar(){  
  bar.write(posBar);
  delay(3000);
  int pos = maxPosPlat;
  for (int j = 0; j < 5; j++){
    if (pos == maxPosPlat){
      pos = 20;
    }else{
      pos = maxPosPlat;
    }
    delay(1000);
    platform.write(pos);
  }  
}

void mezclar(){
  for (int pos = 0; pos <= 180; pos++){
    bar.write(pos);
    delay(10);
  }
  for (int pos = 180; pos >= 0; pos--){
    bar.write(pos);
    delay(10);
  }
}

void printData(){
  Serial.println("Temperatura Mezcla " + String(tempMixture) + " °C");
  Serial.println("Humedad Mezcla " + String(humMixture));
  Serial.println("Temperatura Compost " + String(tempCompost) + " °C");
  Serial.println("Temperatura Compost " + String(tempCompost) + " °C");
}

void readSensorTH(){
  tempMixture = dht.readTemperature();
  humMixture = dht.readHumidity();
}

void readsensorCompost(){
  sensorCompost.requestTemperatures();
  tempCompost = sensorCompost.getTempCByIndex(0);
}

void ReadSerialData(){
  short dataLenght = Serial.available();
  char tBuffer[dataLenght + 1];
  tBuffer[0] = '\0';
  for (short i = 0; i < dataLenght; i++){
    tBuffer[i] = Serial.read();
  }
}
