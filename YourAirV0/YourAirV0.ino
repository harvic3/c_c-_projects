#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "FS.h"
#include <EEPROM.h>

#define pinPower 16
#define pinWiFi 2

//Buffers de Datos
const short bufferSize = 255; // Maximum length of Buffers
char dataBuffer[bufferSize]; //buffer para datos
char replyBuffer[bufferSize]; //bufferr para respuesta
//Default Port

//Network Params
//IPAddress ip(192.168.1.70);
//IPAddress subNet(255,255,255.0);
//IPAddress gateWay(192.168.1.254);
unsigned int port = 23; //Puerto de conexión

//WiFi Credentials
const char* netName = "UNE_1A05";
const char* netPass = "3136952973";
bool online = true;

//Sockets
WiFiServer server(port);

void setup(){
    pinMode(pinPower, OUTPUT);
    pinMode(pinWiFi, OUTPUT);
    Serial.begin(9600);
    digitalWrite(pinPower, LOW);
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
    digitalWrite(pinWiFi, LOW);
    Serial.println("");
    Serial.println("WiFi Connected!!");

    //Start Server
    server.setPort(port);
    server.begin();
    Serial.println("Server online!");
    IPAddress ip = WiFi.localIP();
    Serial.println("IP address: " + String(ip[0], DEC) + "." + String(ip[1], DEC) + "." + String(ip[2], DEC) + "." + String(ip[3], DEC));
}

void loop(){
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    if (client) {
        Serial.println("Client connected!!");
        while (client.connected()){
        //     while(!client.available()){
                delay(10);
                if (!client.remotePort() > 0){
                    client.stop();
                    Serial.println("Client lost!!");
                    return;
                }
            //}
            //String request = client.readStringUntil('\r');
            client.println("Read data from sensor...");
            //Leer información
            readSensor();
            client.println(String(dataBuffer));            
            //delay(1000);
        }
    }
}

void readSensor(){
    ReadSerialData();
}

void ReadSerialData(){
    dataBuffer[0] = '\0';
    while (!Serial.available() > 0){
        //String message = "No data!!";
        //message.toCharArray(dataBuffer, message.length() + 1);
        //return        
        Serial.println("READ");
        delay(200);
    }
    // short dataLenght = Serial.available();
    // for (int i = 0; i < dataLenght; i++){
    //     dataBuffer[i] = Serial.read();
    // }
    int i = 0;
    while (Serial.available() > 0){
        dataBuffer[i] = Serial.read();
        i++;
    }
}