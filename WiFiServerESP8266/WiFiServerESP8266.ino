
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FS.h"

#define outPin 16
//const char* netName = "HIGUITA_VI";
//const char* netPass = "3136952973";
char tempChar[255];

WiFiServer server(21);

int counter = 0;
char* netName = "HIGUITA_VI";
char* netPass = "3136952973";
bool online = true;

// the setup function runs once when you press reset or power the board
void setup() {
  //Initialize digital pin 2 as output.
  pinMode(outPin, OUTPUT);
  Serial.begin(9600);

  //Connecting
  Serial.println();
  Serial.println();
  Serial.print("Connecting to " + String(netName));

  //Setting WiFi
  WiFi.begin(netName, netPass);

  //Connecting WiFi
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  //Connected
  Serial.println("");
  Serial.println("WiFi Connected");

  //Start Server
  server.setPort(23);
  server.begin();
  Serial.println("Server online!");
  IPAddress ip = WiFi.localIP();
  Serial.println("IP address: " + String(ip[0], DEC) + "." + String(ip[1], DEC) + "." + String(ip[2], DEC) + "." + String(ip[3], DEC));

  //Init File System
  if (!SPIFFS.begin()) {    
    Serial.println("");
    Serial.println("Failed to mount file system!");
    return;
  }
  
//  //Formatear el sistema de Archivos
//  if (SPIFFS.format()){
//    Serial.println("FS formated!");
//    return;
//  }

}


void loop() {  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  while(!client.available()){
    delay(10);
  }

  int dataLenght = client.available();
  String request = client.readStringUntil('\r');
  
  request.toCharArray(tempChar, request.length() + 1);
  if(strncmp("PinOn", tempChar, 5) == 0){
    digitalWrite(outPin, LOW);
  }else if(strncmp("PinOff", tempChar, 6) == 0){
    digitalWrite(outPin, HIGH); 
  }else if(strncmp("Write", tempChar, 5) == 0){
    String path = "/WiFiSetting.txt";
    delay(5000);
    String data = Serial.readStringUntil('\r');     
    if (ProcesarArchivo(path, data)){
      client.println("Leyendo archivo!");
      File file = SPIFFS.open(path, "r");
      if (file){
        client.printf("Size = %d\r\n", file.size());
        String text = "Leido: " + file.readStringUntil('\n');
        client.println(text);
        file.close();
      }
    }
  }

  counter++;
  String response = "Llegó " + request + ", van " + String(counter) + " acciones! Tamaño " + String(dataLenght);
  Serial.println(response);
  client.println(response);
  
  //client.flush();
}

bool ProcesarArchivo(String path, String data){
  if (SPIFFS.begin()){
    Serial.println("FS Started");  
    File newFile = SPIFFS.open(path, "w+");
    if (newFile){
      Serial.println("Se crea el archivo");      
      newFile.println(data);
      newFile.close();
      return true;
    }
    Serial.println("No se crea archivo");
  }
  return false;
}

