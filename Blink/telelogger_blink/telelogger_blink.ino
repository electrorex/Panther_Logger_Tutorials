/*Telelogger_Blink Script. Using the Telelogger (Episode 1): Getting started
 * This script demonstrates how to flash or blink the indicator LEDs on the Telelogger board
 * T. Rex Miller, November 18, 2023
 */

//Below, we include this Adafruit library to use the GPIO expander on the Telelogger. 
//Pins 8,9 and 10 on the MCP are used to turn LEDs 2, 3 and 4 on and off, respectively.
//LED1 is conrolled by digital pin 13 on the Telelogger processor
#include "Adafruit_MCP23X17.h"

//Instatiate an object for the MCP library
Adafruit_MCP23X17 mcp;

//Below make a function to flash an LED
//The function takes several arguments
//LEDpin = the mcp pin of the LED to flash (these are one of 8, 9 or 10 on the Telelogger)
//LEDflashes = the number of flashes to occur
//LEDontime = time in milliseconds to keep LED on during the flash
//LEDofftime = time in milliseconds to keep LED off during the flash

void flashLED(int LEDpin, int LEDflashes, int LEDontime, int LEDofftime){
  for(int i= 0;i <= LEDflashes; i++){
    mcp.pinMode(LEDpin,OUTPUT);
    mcp.digitalWrite(LEDpin,HIGH);
    delay(LEDontime);
    mcp.digitalWrite(LEDpin,LOW);
    delay(LEDofftime);
  }
}

//Below make a function to turn all LEDs on simultaneously
void allLEDOn(){
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,HIGH);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,HIGH);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,HIGH);
}

//Below make a function to turn all LEDs off simultaneously
void allLEDOff(){
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,LOW);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,LOW);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,LOW);
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  Serial.begin(115200); //Start serial communications
  
  pinMode(13,OUTPUT); //Set pin 13 on SAMD21 as OUTPUT
  digitalWrite(13,HIGH); //Turn first red LED1 on
  
  Serial.println(F("Setting things up. Please wait"));
  Wire.begin(); 
  mcp.begin_I2C(); //Start communications with the MCP GPIO expander over I2C
  
  mcp.pinMode(7, OUTPUT);
  mcp.digitalWrite(7, LOW); //Turn off 12VS rail. We do not need it for this program.
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, LOW); //Turn off 3VS rail. We do not need it for this program.
  
  allLEDOff(); //start with all LEDs off except for LED1.
}

void loop() {
  //flash LED2 ten times, 100 ms on and 100 ms off
  Serial.println(F("Flashing LED 2"));
  flashLED(8,10,100,100);

  //flash LED3 ten times, 100 ms on and 100 ms off
  Serial.println(F("Flashing LED 3"));
  flashLED(9,10,100,100);

  //flash LED4 ten times, 100 ms on and 100 ms off
  Serial.println(F("Flashing LED 4"));
  flashLED(10,10,100,100);

  //Flash on all LEDs ten times at same time
  Serial.println(F("Flashing all LEDs"));
  for (int i=0; i<10; i++){
    allLEDOn();
    delay(100);
    allLEDOff();
    
    delay(100);
  }
  delay(5000); //wait five seconds and do it all over again
  Serial.println("");
}
