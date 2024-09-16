/*This script is a template for sending data to an internet data base using the Panther Logger board and
 * the Airgain Quectel BG96 modem using HTTP Post commands.
 *Todd Miller December 4, 2023
*/
#include "Adafruit_MCP23X17.h"
#include <OneWire.h>
#define SerialAT Serial1

Adafruit_MCP23X17 mcp;


//We only have one DS18B20 sensor in this tutorial, but we can replicate it a few times
#define NSENSORS 3 //Number of DS18B temperature sensors to measure

//Add the temp chain node addresses in order
byte Address[NSENSORS][8] = {
  {0x28, 0x0B, 0xAE, 0x73, 0x0E, 0x00, 0x00, 0xA3},
  {0x28, 0x0B, 0xAE, 0x73, 0x0E, 0x00, 0x00, 0xA3},
  {0x28, 0x0B, 0xAE, 0x73, 0x0E, 0x00, 0x00, 0xA3}
};

//String variables to post data that need to be changed
char url[] = "https://industrial.api.ubidots.com";
char Host[] = "industrial.api.ubidots.com:80";
char Token[] = "X-Auth-Token: BBUS-GqdJRJRyZz9yrWLH94R";                                                         
char Destination[] = "/api/v1.6/devices/Panther Logger2";
char ContentLength[30]; 
char UserAgent[] = "Panther Logger/1.0";    
char ContentType[] = "application/json"; 
char DataString[200];      
char Payload[2000]; //Make this large enough to hold expected post data plus ~300 to 500 for header.

void setup() {
  delay(5000);
  Serial.println("Setting up");
  Serial.begin(115700); 
  SerialAT.begin(115700);

  //Turn on red LED so we know board has power
  pinMode(13, OUTPUT); 
  digitalWrite(13,HIGH);
  Wire.begin();
  mcp.begin_I2C(0x20);
  mcp.pinMode(7,OUTPUT);
  mcp.digitalWrite(7,LOW); //Turn off 12VS rail
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, HIGH); //Turn on 3VS rail

  //Turn off all other LEDs to start
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,LOW);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,LOW);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,LOW);
  setupCell();
  Serial.println("Setup complete");
}

void loop() {
  mcp.digitalWrite(8,HIGH); //Turn on LED2
  Serial.println("Starting a data transmission");
  Serial.println("Reading sensors");
  mcp.digitalWrite(9,HIGH); //Turn on LED3
  readSensors();
  delay(3000);
  Serial.println("Posting sensor data");
  mcp.digitalWrite(9,LOW); //Turn off LED3
  mcp.digitalWrite(10,HIGH); //Turn on LED4
  makeDataString();
  makeHeader();
  sendData();
  mcp.digitalWrite(10,LOW); //Turn off LED4
  mcp.digitalWrite(8,LOW); //TUrn off LED2
  Serial.println("Finished data transmission");
  Serial.println("*****************************************************************************");
  delay(10000);
}
