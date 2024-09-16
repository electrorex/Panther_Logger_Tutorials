/*This sketch will setup the serial monitor to send AT commands to the Telelogger's
 * cellular modem and read the responses
 * Todd R Miller, December 11, 2023
 */

#define SerialAT Serial1
#include "Adafruit_MCP23X17.h"

Adafruit_MCP23X17 mcp;

void ModemReset(){
 Serial.println("Resetting modem");
 pinMode(2,OUTPUT);
 digitalWrite(2,LOW);
 delay(1000);
 pinMode(2,INPUT);
 Serial.println("Modem is reset");
}

void ModemOn(){
 Serial.println("Turning modem on");
 pinMode(5,OUTPUT);
 digitalWrite(5,LOW);
 delay(1000);
 pinMode(5,INPUT);
 Serial.println("Modem is on");
}

void setup() {
  delay(5000);
  Serial.println("Setting Up");
  Serial.begin(115200);
  SerialAT.begin(115200);
  
  Wire.begin();
  mcp.begin_I2C();
  mcp.pinMode(7,OUTPUT);
  mcp.digitalWrite(7,LOW); //Turn off 12VS rail
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, LOW); //Turn off 3VS rail
 
  ModemOn();
  ModemReset();
  delay(1000);
}

void loop() {
  while(true) {
    if (SerialAT.available()) {
      Serial.write(SerialAT.read());
    }
    if (Serial.available()) {
      SerialAT.write(Serial.read());
    }
    delay(0);
  }
}
