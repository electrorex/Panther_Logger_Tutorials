/*
This sketch was written for the Electrorex Panther Logger Tutorial 8.
The sketch reads the In-Situ RDO Pro sensor using SDI12 and parses
the data into float variables.

Wiring (RDO Pro wires to Panther Logger screw terminal ports):
White ---> D11
Red ---> 12VS
Black ---> GND
*/

//Libraries needed
#include <SDI12.h>
#include "Adafruit_MCP23X17.h"

Adafruit_MCP23X17 mcp;

#define sdi12pin 11        
SDI12 sdi12(sdi12pin);

float DO;
float DOS;
float RDOTemp;
 
void readRDO() {
    mcp.pinMode(7, OUTPUT);
    mcp.digitalWrite(7, HIGH); //Turn on 12VS rail
  
    DO = -9999;
    DOS = -9999;
    RDOTemp = -9999;
    unsigned int RDONowTime = millis();
    unsigned int RDOInterval = 10000; //Give sensor 10 seconds to send good data
    while(RDONowTime + RDOInterval > millis()){ 
      if(DO == -9999 || DOS == -9999 || RDOTemp == -9999){
        sdi12.begin();
        sdi12.clearBuffer();
        float RDOData[4] = {0};
        sdi12.clearBuffer();
        String command1 = "4M!"; //Tell sensor to make a measurement
        sdi12.sendCommand(command1);
        delay(100);
        String sdiResponse = sdi12.readStringUntil('\n');
        Serial.println(sdiResponse);
        delay(3000); //RDO pro needs two seconds to make measurement
        sdi12.clearBuffer();
        String command2 = "4D0!"; //Tell sensor to send us the data
        sdi12.sendCommand(command2);
        delay(100);

        for (uint8_t i = 0; i <= 3; i++){
          RDOData[i] = sdi12.parseFloat();
        }

        DO = RDOData[1];
        DOS = RDOData[2];
        RDOTemp = RDOData[3];

        sdi12.clearBuffer();
        sdi12.end();
    }
  }
   mcp.pinMode(7, OUTPUT);
   mcp.digitalWrite(7, LOW); //Turn off 12VS rail
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  
  //Start all of the serial ports
  Serial.begin(115200);

  //Turn on red LED1 to indicate the program has started
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  
  Serial.println("Setting things up. Please wait");

  Wire.begin();
  mcp.begin_I2C(0x20);

  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, HIGH); //Turn on 3VS rail
  delay(100);
  
  mcp.pinMode(7, OUTPUT);
  mcp.digitalWrite(7, HIGH); //Turn off 12VS rail
  delay(100);
  
  mcp.pinMode(8, OUTPUT);
  mcp.digitalWrite(8, LOW); //Turn off indicator LED2
  delay(100);
  
  mcp.pinMode(9, OUTPUT);
  mcp.digitalWrite(9, LOW); //Turn off indicator LED3
  delay(100);
  
  mcp.pinMode(10, OUTPUT);
  mcp.digitalWrite(10, LOW); //Turn off indicator LED4
  delay(100);
  
  Serial.println("Setup finished");
}

void loop() {
  readRDO();
  Serial.print("Dissolved Oxygen (mg/L) = "); Serial.println(DO);
  Serial.print("Dissolved Oxygen Saturation (%) = "); Serial.println(DOS);
  Serial.print("Temperature (C) = "); Serial.println(RDOTemp);
  delay(10000); //Wait ten seconds before doing another read
}
