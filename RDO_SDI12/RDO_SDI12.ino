/*
This sketch was written for the Electrorex Panther Logger Tutorial 8.
The sketch reads the In-Situ RDO Pro dissolved oxygen sensor using SDI12 
and parses the data into float variables. Digital pin 11 on the
Panther Logger is used as the SDI12 port.

Wiring (RDO Pro wires to Panther Logger screw terminal ports):
White ---> D11
Red ---> 12VS
Black ---> GND

-Todd R. Miller, March 6, 2025
*/

//Libraries needed
#include <SDI12.h>
#include <Panther.h>

Panther ptr;

#define sdi12pin 11        
SDI12 sdi12(sdi12pin);

float DO;
float DOS;
float RDOTemp;
 
void readRDO() {
    ptr.set12v(HIGH); //Power sensor with 12V rail. It needs at least 9V.
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
       
        //Tell sensor to make a measurement. Assumes sensor address is 4
        String command1 = "4M!"; 
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
   ptr.set12v(LOW);
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  Serial.begin(115200);
  ptr.begin();

  //Turn on red LED1 to indicate the program has started
  ptr.LED(1,HIGH);
  
  Serial.println("Setting things up. Please wait");

  ptr.set3v3(HIGH);
  ptr.set12v(HIGH);
  delay(100);
  ptr.LED(2,LOW);
  ptr.LED(3,LOW);
  ptr.LED(4,LOW);
  Serial.println("Setup finished");
}

void loop() {
  readRDO();
  Serial.print("Dissolved Oxygen (mg/L) = "); Serial.println(DO);
  Serial.print("Dissolved Oxygen Saturation (%) = "); Serial.println(DOS);
  Serial.print("Temperature (C) = "); Serial.println(RDOTemp);
  delay(10000); //Wait ten seconds before doing another read
}
