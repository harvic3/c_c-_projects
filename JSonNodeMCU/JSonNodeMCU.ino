#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FS.h"

#define pinPower 16

const String pathSettingsFile = "/Settings.json";
const int bufferSize = 256;
char netName[30];
char netKey[30];
char dataBuffer[bufferSize];
char replyBuffer[bufferSize];

void setup() {
 
  pinMode(pinPower, OUTPUT);
  Serial.begin(9600);
  
  while (!Serial) {
    // wait serial port initialization
  }
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system!");
    return;
  }
  //Configuración desde Archivo
  if (!LoadConfigurations()){
    Serial.println("Failed to set configurations!");
    return;
  }

  //Todo ok
  Serial.println("Running!");
  digitalWrite(pinPower, LOW);
}

void loop() {
  //Buscar por Datos seriales
  if (Serial.available() > 0) {
    delay(10);
    capturarDatosSeriales();
  }

}

void capturarDatosSeriales(){
  int dataLenght = Serial.available();
  char tBuffer[bufferSize];
  for (int i = 0; i < dataLenght; i++){
    tBuffer[i] = Serial.read();
  }  
  //Verificar si es para recibir varios paquetes
  if (tBuffer[0] == '@' && tBuffer[1] == '#' && tBuffer[2] == '@' && tBuffer[3] == ':'){    
    //Se van a recibir varios paquetes
    short packages = String(tBuffer[4]).toInt();
    //String sizeStr = String(tBuffer).substring(6, dataLenght - 1);
    int sizeData = (String(tBuffer).substring(6, dataLenght - 1)).toInt();
    //Confirmar cuántos y el tamaño de lo que se va a recibir
    Serial.println("OK-" + String(packages) + "-" + String(sizeData));   
    //Leer paquetes
    short counter = 0;
    bool flag = true;
    while (flag){
      if (Serial.available() > 0){
        delay(100);
        dataLenght = Serial.available();
        tBuffer[0] = '\0';
        for (int i = 0; i < dataLenght; i++){
          tBuffer[i] = Serial.read();
        }
        tBuffer[dataLenght - 1] = '\0';
        strcat(dataBuffer, tBuffer);
        //Controlar variables y banderas
        counter++;
        //Responder al emisor
        Serial.println("GO-" + String(counter) + "-" + String(dataLenght));
        //Verificar para ver si deben enviar de nuevo
        if(counter == packages){
          bool verify = false;          
          //Enviar data para verificar con el Emisor
          Serial.println(String(dataBuffer));
          //Esperar para verificar la respuesta
          while (!verify){
            if (Serial.available() > 0){
              delay(10);
              dataLenght = Serial.available();
              tBuffer[0] = '\0';
              for (int i = 0; i < dataLenght; i++){
                tBuffer[i] = Serial.read();
              }
              tBuffer[dataLenght - 1] = '\0';
              //Si la respuesta es positiva terminar transferencia, de lo contrario volver a empezar.
              if (String(tBuffer) == "OKPASS"){
                //Resetear banderass de los dos ciclos While para salir
                flag = false;
                verify = true;
                Serial.println("OKEND");
              }else if (String(tBuffer) == "NOPASS"){
                //Volver contador a cero y bandera de verificación
                counter = 0;         
                verify = true;
                dataBuffer[0] = '\0';
                Serial.println("OK-" + String(packages) + "-" + String(sizeData));
              }
            }
          }           
        }        
      }      
    }
    //Aquí ya se puede procesar el archivo JSON
    SaveConfigurations(dataBuffer);
    dataBuffer[0] = '\0';
  }else{
    //Aquí otros comandos
    Serial.print("Secuencia rota!!");
  }
}

bool SaveConfigurations(char* dataFile) {
  if (SPIFFS.begin()) {
    File configFile = SPIFFS.open(pathSettingsFile, "w+");
    if (configFile) {
      Serial.println("Processing File. Please Wait!");
      configFile.println(String(dataFile));
      configFile.close();
      Serial.println("File processed. Please Restart device!!");
      //return true;
    }
    configFile = SPIFFS.open(pathSettingsFile, "r");
    if (configFile) {
      Serial.println("File content");
      Serial.println(configFile.readStringUntil('\n'));
      configFile.close();
      return true;
    }
  }
  Serial.println("Errors processing File!");
  return false;
}

bool LoadConfigurations(){
  Serial.println("Settings Started!");
  File configFile = SPIFFS.open(pathSettingsFile, "r");
  if (!configFile) {
    Serial.println("Failed to open Config file!");
    return false;
  }
  //Parsear y obtener datos
  size_t sizeA = configFile.size() - 2;
  //OJO El archivo no puede ser más grande de 256 bytes
  StaticJsonBuffer<256> jsonBuffer;
  //char fileR[] = "{"NetWork":"HIGUITA_VI","Key":"3136952973","IpAddress":"192.168.1.50","Port":21,"SubNet":"none","GateWay":"none","State":0,"UserSet":false}";
  char fileR[sizeA];
  configFile.readStringUntil('\r').toCharArray(fileR, sizeA + 1);  
  JsonObject& json = jsonBuffer.parseObject(fileR);  
  if (!json.success()) {
    Serial.println("Error to parse file!");
    return false;
  }  
  String net = json["NetWork"];
  String key = json["Key"];
  net.toCharArray(netName, net.length() + 1);
  key.toCharArray(netKey, key.length() + 1);
  String ip = json["IPAddress"];
  int port = json["Port"];
  String subNet = json["SubNet"];
  String gateWay = json["GateWay"];
  String mac = json["MacAddress"];
  byte stateOuts = json["State"];
  bool userSet = json["UserSet"];
  
  Serial.println("NetWork " + String(netName));
  Serial.println("Key " + String(netKey));
  Serial.println("IP " + ip);
  Serial.println("Port " + String(port));
  Serial.println("SubNet " + subNet);
  Serial.println("GateWay " + gateWay);
  Serial.println("MAC " + mac);
  Serial.println("State " + String(stateOuts));
  Serial.println("UserSet " + String(userSet));
  
  configFile.close();  
  return true;
  
}
