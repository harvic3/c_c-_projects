#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include <EEPROM.h>

#define READY     2 //Pin para habilitar la conmutación de los Relevos. LED indicador de READY

//Buffers de Datos
const short bufferSize = 255; // Maximum length of Buffers
char dataBuffer[bufferSize]; //buffer para datos
char replyBuffer[bufferSize]; //bufferr para respuesta
//Default Port
const unsigned int defaultPort = 2500; //Puerto de conexión por defecto

//Network Params
IPAddress ip(0,0,0,0);
IPAddress subNet(0,0,0,0);
IPAddress gateWay(0,0,0,0);
IPAddress ipRemote(0,0,0,0);
String mac;
unsigned int port = 2500; //Puerto de conexión

//WiFi Credentials
char netName[32] = "UNE_1A05";
char netKey[60] = "3136952973";

//Sockets
WiFiServer server(defaultPort);

//Others
short pinCounter = 0;
bool online = false;
unsigned long timeClock = 0; 

void setup() {
  // put your setup code here, to run once:
  pinMode(READY, OUTPUT);
    
  //Init Serial comm
  Serial.begin(9600);
  while (!Serial) {
    // wait serial port initialization
  }

  //Connectar a WiFi
  ConnectToNetwork();  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Check for Clients
  if (online){
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Client connected!!");
      while (client.connected()){
        while(!client.available()){
          delay(10);
          if (!client.remotePort() > 0){
            client.stop();
            Serial.println("Client lost!!");
            return;
          }
          //Datos seriales
          if (VerifyIfDataFromSerial()){         
            client.print(String(replyBuffer));
            replyBuffer[0] = '\0';
          };
        }
        delay(10);  
        String request = client.readStringUntil('\r');
        
        //Process Command
        ProccessCommand(request);

        dataBuffer[0] = '\0';
      } 
    }
  }
    
  //Si red no configurada No entrar
  if (String(netName) != "NOT_ASSIGNED"){
    //Reconnectar si está desconectado
    if (WiFi.status() != WL_CONNECTED){
      Serial.println("Trying to reconnect");
      //Disconnected
      online = false;
      ConnectToNetwork();
    }
  }else if (timeClock + 5000 < millis()){
    Serial.println("Network not configured!!");
    timeClock = millis();    
  }
}

//Función para conecctar a la Red ya sea al inicio o por reconexión
void ConnectToNetwork(){
  //Apagar led de Ready
  digitalWrite(READY, HIGH);

  //Detener servidor
  server.stop();
  
  //Config and init Wifi Module
  WiFi.begin(netName, netKey);
  
  //Connecting WiFi
  Serial.print("\nConnecting to " + String(netName));
  while (WiFi.status() != WL_CONNECTED){    
    delay(500);
    Serial.print(".");
    if (pinCounter == 10){
      Serial.println("");
      Serial.println("\nFailed connecting to network!!");
      pinCounter = 0;
      return;
    }
    //Verify data from Serial
    VerifyIfDataFromSerial();
    pinCounter++;
  }

  pinCounter = 0;
  
  //Connected
  online = true;
  Serial.println("");
  Serial.println("Connected to NetWork!!");

  //Set Port
  if (port == 0){
    port = defaultPort;
  }

  //Reiniciar puerto del Servidor
  server.setPort(port);
  //Start Server
  server.begin();
  
  ip = WiFi.localIP();  
  subNet = WiFi.subnetMask();
  gateWay = WiFi.gatewayIP();
  mac = WiFi.macAddress();

  PrintNetParams();

  //Si este LED enciende, está listo
  digitalWrite(READY, LOW);
}

//Función para comprobar si hay datos seriales
bool VerifyIfDataFromSerial(){
  if (Serial.available()){
    delay(50);
    ReadSerialData();
    return true;
  }
  return false;
}

//Información de parámetros
void PrintNetParams(){
  Serial.println("\nSystem ready with \n" + 
     String("Ip: " + ConvertIpToString(ip)) + 
     String("\nSubnet Mask: " + ConvertIpToString(subNet)) + 
     String("\nGateway: " + ConvertIpToString(gateWay)) + 
     String("\nPort: " + String(port))
   );
}

//Función para capturar los datos seriales
void ReadSerialData(){
  short dataLenght = Serial.available();
  char tBuffer[dataLenght + 1];
  tBuffer[0] = '\0';
  for (short i = 0; i < dataLenght; i++){
    tBuffer[i] = Serial.read();
  }
  Serial.flush();

  //Copy to buffer to send
  replyBuffer[0] = '\0';
  strcat(replyBuffer, tBuffer);
}

//Función para procesar los datos del Cliente WiFi
void ProccessCommand(String command){ 
  if (command.length() > 0){
    Serial.println(command);
  }
}

//Función para convertir una IP en String
String ConvertIpToString(IPAddress ipAd){
  String ipCadena = String(ipAd[0], DEC) + "." + String(ipAd[1], DEC) + "." + String(ipAd[2], DEC) + "." + String(ipAd[3], DEC);
  return ipCadena;
}

