/*
This script goes with the Electrorex tutorials on using the LoRa modem
It measures a DS18B20 temperature sensor and battery voltage,
then sends the data to ubidots using the LoRa modem and The Things Network
Thus, an account at Ubidots and The Things Network is required.
See tutorials in the Electrorex learning center at electrorex.io
 */

/********************************************************************************************************************/
/***************************************CHANGE THESE SETTINGS PRIOR TO DEPLOYMENT************************************/
//CHANGE THESE BELOW SETTINGS PRIOR TO DEPLOYMENT!
//Change DEBUG to 1 to allow verbose printing to the serial monitor, otherwise change DEBUG to zero to turn verbose off during deployments
#define DEBUG 1

//Set initial approximate date and time. This will be set to exact time after first succesfull join and send
int Yeari = 23;
int Monthi = 11;
int Dayi = 4;
int Houri = 14; //military time
int Minutei = 10;
int Secondi = 0;

//Should the program be delayed for a pre-determnined amount of time until the unit is deployed?
//If yes then enter time in minutes to delay deployment. If no then enter 0.
int DelayDeployTime = 0;

//Should the system sleep if battery voltage is too low? Note, battery must be connected then.
int syscheck = 0

//Should the board sleep? 1 = yes, 0 = No
int Sleep = 0

//Code will run for as long as it needs to and then the code will delay or sleep. How long to delay/sleep? Enter in whole minutes
int Interval = 5;

char DEVEUI[] = "AT+DEVEUI=AC1F09FFFE0C5D79";
char APPEUI[] = "AT+APPEUI=AC1F09FFF9151720";
char APPKEY[] = "AT+APPKEY=AC1F09FFFE0C5D79AC1F09FFF9151720";

#define NSENSORS 3 //Number of DS18B temperature sensors to measure

//Add the temp chain node addresses in order
byte Address[NSENSORS][8] = {
  {0x28, 0xD7, 0xA8, 0xE2, 0x08, 0x00, 0x00, 0x39},
  {0x28, 0xD7, 0xA8, 0xE2, 0x08, 0x00, 0x00, 0x39},
  {0x28, 0xD7, 0xA8, 0xE2, 0x08, 0x00, 0x00, 0x39}
};

/************************************END OF SETTINGS NEEDING TO BE CHANGED*******************************************/
/********************************************************************************************************************/
//Libraries needed
#include "wiring_private.h" 
#include <OneWire.h>
#include "Adafruit_MCP23X17.h"
#include <Adafruit_SleepyDog.h>
#include <avr/dtostrf.h>
#include <stdlib.h>
#include <string>  
#include <RTCZero.h>
#include <MemoryFree.h>
#include <memory>

Adafruit_MCP23X17 mcp;
RTCZero rtc;

//Pin 10 and 12 are RX and TX for UART multiplexer communications
#define PIN_SERIAL2_RX       (34ul)                                           // Pin description number for PIO_SERCOM on D12
#define PIN_SERIAL2_TX       (36ul)                                           // Pin description number for PIO_SERCOM on D10
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)                                  // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)                                // SERCOM pad 3

Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
// Interrupt handler for SERCOM1
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

#define SerialAT Serial2

//Replacing serial.print with debug based on Ralph Bacon's tips, https://www.youtube.com/watch?v=--KxxMaiwSE
#if DEBUG==1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugwrite(x) Serial.write(x)
#else
#define debug(x)
#define debugln(x)
#define debugwrite(x)
#endif

int SleepIntervals;

char result_char[5000];
int Check; //If 1 then an expected response was given by modem, otherwise if 0 unexpected or modem failed to give response
//Function to send AT commands to modem and listen for expected response, error code, or timeout after so many seconds
String sendAT(const char *toSend, const char *toCheck1, const char *toCheck2, unsigned long milliseconds) {
  String result;
  debug(F("Sending: "));
  debugln(F(toSend));
  SerialAT.println(toSend);
  unsigned long startTime = millis();
  debug(F("Received: "));
  while (millis() - startTime < milliseconds) {
    SerialAT.flush();
    Serial.flush();
    if (SerialAT.available()) {
      char c = SerialAT.read();
      Serial.write(c);
      result += c;  // append to the result string
      unsigned int Len = result.length();
      result.toCharArray(result_char, Len);
      
      if (strstr(result_char, toCheck1) || strstr(result_char, toCheck2))
      {
        milliseconds = 0;
        Check = 1;
      }
      else {
        Check = 0; 
      }
      
    }
  }
  return result;
  debugln(result);
}

void setup() {
  delay(DelayDeployTime*(60*1000));
  Serial.begin(115200);
  SerialAT.begin(115200);
  debugln("Setting up");
  Wire.begin();
  rtc.begin();
  setTimeInitial();
  mcp.begin_I2C(); 
  
  //Turn on LEDs 2, 3, and 4 to let us know its in setup
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,HIGH);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,HIGH);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,HIGH);

  //Turn on 3.3V rail
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,HIGH); 
  delay(2000); //Let things turn on
  
  //Setup UART communications with LoRa modem and setup modem with initial AT commands
  setupLoRaUART();
  setupLoRa();
  
  //Turn off all indicator LEDs
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,LOW);
  delay(100);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,LOW);
  delay(100);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,LOW);
  delay(100);
  
  debugln("Finished Setting up. Waiting for first interval.");
  debugln(F("*********************************************************")); 
}

void loop() {
    if(syscheck==1){
      SystemCheck(); //Is battery voltage too low to continue? If so then go to sleep.
    }
    debugln(F("-------------------------------------------------------------------------"));
    debugln("Next interval. Reading sensors");
    delay(100);
    mcp.digitalWrite(8,HIGH); //turn on green LED to let us know its measuring sensors. All others off.
    delay(100);
    mcp.digitalWrite(9,LOW);
    delay(100);
    mcp.digitalWrite(10,LOW);
    delay(100);
    SerialAT.begin(115200); //Start serial communications with LoRa modem as it was ended at end of loop.
    
    //Read all sensors
    readBat();
    readTemps();
    mcp.digitalWrite(8,LOW); //finished reading sensors, turn green LED off
    delay(100);

    //Construct payload and send data with LoRa modem
    getPayload();
    debugln(F("Finished constructing payload. Getting ready to send data"));
    mcp.digitalWrite(9,HIGH); //sending data, second red LED on
    delay(100);
    sendLoRa();
    debugln(F("Finished attemping to send data via LoRa"));
    delay(100);
    debugln(F("Requesting and setting time on the RTC"));
    printResults();
    delay(100);
    mcp.digitalWrite(9,LOW); //finished sending data, second red LED off
    delay(100);
    
    if(Sleep == 1){
      debug(F("Finished. Waiting for next interval. Going to sleep for ")); 
      debug(Interval);
      debugln(F(" minutes")); 
      debugln(F("-------------------------------------------------------------------------"));
      GoToSleep();
    } else (
      debug(F("Finished. Waiting for next interval. Stopping for ")); 
      debug(Interval);
      debugln(F(" minutes")); 
      debugln(F("-------------------------------------------------------------------------"));
      delay(Interval *1000);
    )
}