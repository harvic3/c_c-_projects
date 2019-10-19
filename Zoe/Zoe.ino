#include "DHT.h"

#define DHTTYPE DHT11

DHT dht(7, DHTTYPE);

int ultra_echo = 2;
int ultra_trig = 3;
int pin_motor = 17;

void setup() {
  
  Serial.begin(9600);  
  dht.begin();
  
  pinMode(pin_motor, OUTPUT);
  pinMode(ultra_echo, INPUT);
  pinMode(ultra_trig, OUTPUT);
  
}

double temperatura;
double humedad;
double nivel;

double leer_nivel() {
  
  digitalWrite(ultra_trig, LOW);
  delayMicroseconds(1);
  digitalWrite(ultra_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultra_trig, LOW);
  
  return pulseIn(ultra_echo, HIGH) / 2.0 * 0.03432;
  
}

void imprimir_datos() {
  
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  Serial.print("Humedad: ");
  Serial.println(humedad);
  Serial.print("Nivel: ");
  Serial.println(nivel);  
  
}

void loop() {
  
  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();
  nivel = leer_nivel();
  imprimir_datos();
    
}
