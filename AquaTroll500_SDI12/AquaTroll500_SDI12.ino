/*This sketch was written for the Electrorex Panther Logger Tutorial 10
The sketch reads the In-Situ Aqua Troll sonde using SDI12 and parses
the data into float variables.

Wiring (Aquatroll wires to Panther Logger screw terminal ports):
White ---> D11
Red ---> 12VS
Black ---> GND

The sonde should be configured for SDI-12 communication using VuSitu software
Select variables and order output
*/

//Libraries needed
#include <SDI12.h>
#include <Panther.h>

//Instatiate the Panther library
Panther ptr;

//D11 or D6 can be used for SDI12 communicaton
#define sdi12pin 11
SDI12 sdi12(sdi12pin);

//Define individual float variables to store sonde output
//This is order of my SDI12 variables as setup in VuSitu. Change accordingly
float INTemp1 = -9999; //water temperature (C)
float INCond = -9999; //specific conductivity (uS/cm)
float INSal = -9999; //salinity (psu)
float INRes = -9999; //resistivity (ohm-cm)
float INDens = -9999; //water density (g/cm^3)
float INTDS = -9999; //total dissolved solids (ppt)
float INRDO = -9999; //dissolved oxygen (mg/L)
float INRDOS = -9999; //dissolved oxygen % saturation
float INRDOP = -9999; //oxygen partial pressure (tor)
float INpH = -9999; //pH
float INORP = -9999; //oxidative-reductive potential (mV)
float INTemp2 = -9999; //internal temperature (C)

void readTroll() {
    ptr.set12v(HIGH);  
    unsigned int TrollNowTime = millis();
    unsigned int TrollInterval = 10000; //Give sensor 10 seconds to send good data
    while(TrollNowTime + TrollInterval > millis()){ 
      if(INTemp1 == -9999 || INTemp2 == -9999){ //sensor must replace -9999 with data
        sdi12.begin();
        sdi12.clearBuffer();
        sdi12.sendCommand("4M!"); //sonde address is 4. The "M" means start a measurement
        Serial.println(sdi12.readStringUntil('\n')); //sonde returns time until data is available
        delay(15000);
        sdi12.clearBuffer();
        sdi12.sendCommand("4D0!"); //sonde address 4 and D0 means get first set of data
        delay(100);
        for (uint8_t i = 0; i <= 3; i++){
          TrollData[i] = sdi12.parseFloat(); //read the data and parse into a float array
        }
        INTemp1 = TrollData[1];
        INCond = TrollData[2];
        INSal = TrollData[3];
        sdi12.clearBuffer();
        sdi12.sendCommand("4D1!"); //request the second section of data
        delay(100);
        for (uint8_t i = 4; i <= 6; i++){
          TrollData[i] = sdi12.parseFloat(); //read the data and parse into a float
        }
        INRes = TrollData[4];
        INDens = TrollData[5];
        INTDS = TrollData[6];
        sdi12.clearBuffer();
        sdi12.sendCommand("4D2!");
        delay(100);
        for (uint8_t i = 7; i <= 9; i++){
          TrollData[i] = sdi12.parseFloat();
        }
        INRDO = TrollData[7];
        INRDOS = TrollData[8];
        INRDOP = TrollData[9];
        sdi12.clearBuffer();
        sdi12.sendCommand("4M1!");
        Serial.println(sdi12.readStringUntil('\n'));
        delay(15000);
        sdi12.clearBuffer();
        sdi12.sendCommand("4D0!");
        delay(100);
        for (uint8_t i = 10; i <= 12; i++){
          TrollData[i] = sdi12.parseFloat();
        }
        INpH = TrollData[1];
        INORP = TrollData[2];
        INTemp2 = TrollData[3];
        sdi12.clearBuffer();
        sdi12.end();
      }
    }
    ptr.set12v(LOW);
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  
  //Start all of the serial ports
  Serial.begin(115200);
  ptr.begin();
  //Turn on red LED1 to indicate the program has started
  ptr.LED(1,HIGH);
  
  Serial.println("Setting things up. Please wait");

  ptr.set3v3(HIGH);
  ptr.set12v(HIGH);
  
  ptr.LED(2,LOW);
  ptr.LED(3,LOW);
  ptr.LED(4,LOW);
  
  Serial.println("Setup finished");
}

void loop() {
  readTroll();
  delay(10000); //Wait ten seconds before doing another read
}
