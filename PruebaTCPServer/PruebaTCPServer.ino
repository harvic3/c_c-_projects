#include <SPI.h>
#include <Ethernet.h>

// Set values below to match your network needs:
byte mac[] = {0x54, 0x52, 0x49, 0x41, 0x44, 0x00};   // MAC Address
byte ip[] = {192, 168, 0, 50};                       // Network Address
byte gateway[] = {192, 168, 0, 1};                   // Gateway Address
byte subnet[] = {255, 255, 255, 0};                  // Subnet Mask
EthernetServer server(23);                           // Set Server Port
EthernetClient client;                               // Define client
const int BUFFER_SIZE = 255;                              // Maximum length of a command

//Buffer for data
char dataBuffer[BUFFER_SIZE]; //buffer para datos
char replyBuffer[BUFFER_SIZE]; //buffer para respuesta
int flag = 1;

//Setup
void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);          // Start the Ethernet connection
  server.begin();                                    // Begin acting like a server
  Serial.begin(9600);
  Serial.println("Arranqué!!");
}

//Loop
void loop()
{
  client = server.available();  // Check for server availability
  if (client)
  {    
    readBuffer();     // Read the character in the buffer
    Serial.print("Entro cliente y dijo: ");
    Serial.println(dataBuffer);
    //client.print("Te reenvio: ");
    //client.println(dataBuffer);
    processData(dataBuffer);
  }
}

void readBuffer()
{
  int dataLenght = client.available();
  for (int i = 0; i < dataLenght; i++){
    dataBuffer[i] = client.read();
  }
}

void processData(char command[BUFFER_SIZE])  // Command received: process it!
{
  client.print("Isabel te dice: ");
  client.println(String(dataBuffer));
}



