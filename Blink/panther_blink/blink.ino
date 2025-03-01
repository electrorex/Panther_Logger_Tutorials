/*Using the Panther Logger (Episode 1): Getting started
 * This script demonstrates how to flash or blink the indicator LEDs on the Panther board
 * T. Rex Miller, February 28, 2025
 */
#include <Wire.h>
#include <wiring_private.h>

//Setup a new I2C wire on digital pins 3 and 4 for communicating with the GPIO expander
TwoWire myWire(&sercom2,4,3);

//A function to setup registers on the GPIO expander
//This function will be run in setup once
void setupMPC(){
  myWire.beginTransmission(0x20);
  myWire.write(0x00); // IODIRA register
  myWire.write(0x00); // set all of port A to outputs
  myWire.endTransmission();

  myWire.beginTransmission(0x20);
  myWire.write(0x01); // IODIRB register
  myWire.write(0x00); // set all of port B to outputs
  myWire.endTransmission();
}


//Number GPIO pins on MCP23017 as 0 through 15 where 0 to 7 is port A and 8 to 15 is port B
//Function to set pinMode of GPIO expander pins
int mcpModeB[8] = {0,0,0,0,0,0,0,0};
int mcpModeA[8] = {0,0,0,0,0,0,0,0};
void mcpMode(int pin, int mode){
  if (pin > 7){
    int pin2 = pin - 8;
    myWire.beginTransmission(0x20);
    myWire.write(0x13); //Select B port
    mcpModeB[pin2] = mode;
    int mcpModeBint = 0b10000000*!!mcpModeB[7] + 0b1000000*!!mcpModeB[6] + 0b100000*!!mcpModeB[5] +0b10000*!!mcpModeB[4] +0b1000*!!mcpModeB[3] +0b100*!!mcpModeB[2] + 0b10*!!mcpModeB[1] + !!mcpModeB[0];
    myWire.write(mcpModeBint);
    myWire.endTransmission();
  } else {
    myWire.beginTransmission(0x20);
    myWire.write(0x12); //Select A port
    mcpModeA[pin] = mode;
    int mcpModeAint = 0b10000000*!!mcpModeA[7] + 0b1000000*!!mcpModeA[6] + 0b100000*!!mcpModeA[5] +0b10000*!!mcpModeA[4] +0b1000*!!mcpModeA[3] +0b100*!!mcpModeA[2] + 0b10*!!mcpModeA[1] + !!mcpModeA[0];
    myWire.write(mcpModeAint);
    myWire.endTransmission();
  }
}

//Function to write to pin of GPIO expander
int mcpB[8] = {0,0,0,0,0,0,0,0};
int mcpA[8] = {0,0,0,0,0,0,0,0};
void mcpWrite(int pin, int polarity){
  if (pin > 7){
    int pin2 = pin - 8;
    myWire.beginTransmission(0x20);
    myWire.write(0x13); //Select B port
    mcpB[pin2] = polarity;
    int mcpBint = 0b10000000*!!mcpB[7] + 0b1000000*!!mcpB[6] + 0b100000*!!mcpB[5] +0b10000*!!mcpB[4] +0b1000*!!mcpB[3] +0b100*!!mcpB[2] + 0b10*!!mcpB[1] + !!mcpB[0];
    myWire.write(mcpBint);
    myWire.endTransmission();
  } else {
    myWire.beginTransmission(0x20);
    myWire.write(0x12); //Select A port
    mcpA[pin] = polarity;
    int mcpAint = 0b10000000*!!mcpA[7] + 0b1000000*!!mcpA[6] + 0b100000*!!mcpA[5] +0b10000*!!mcpA[4] +0b1000*!!mcpA[3] +0b100*!!mcpA[2] + 0b10*!!mcpA[1] + !!mcpA[0];
    myWire.write(mcpAint);
    myWire.endTransmission();
  }
}

//Below make a function to flash an LED
//The function takes several arguments
//LEDpin = the mcp pin of the LED to flash (these are one of 8, 9 or 10 on the Pather Logger)
//LEDflashes = the number of flashes to occur
//LEDontime = time in milliseconds to keep LED on during the flash
//LEDofftime = time in milliseconds to keep LED off during the flash
void flashLED(int LEDpin, int LEDflashes, int LEDontime, int LEDofftime){
  for(int i= 0;i <= LEDflashes; i++){
    mcpMode(LEDpin,OUTPUT);
    mcpWrite(LEDpin,HIGH);
    delay(LEDontime);
    mcpWrite(LEDpin,LOW);
    delay(LEDofftime);
  }
}

//Below make a function to turn all LEDs on simultaneously
void allLEDOn(){
  mcpMode(8,OUTPUT);
  mcpWrite(8,HIGH);
  mcpMode(9,OUTPUT);
  mcpWrite(9,HIGH);
  mcpMode(10,OUTPUT);
  mcpWrite(10,HIGH);
}

//Below make a function to turn all LEDs off simultaneously
void allLEDOff(){
  mcpMode(8,OUTPUT);
  mcpWrite(8,LOW);
  mcpMode(9,OUTPUT);
  mcpWrite(9,LOW);
  mcpMode(10,OUTPUT);
  mcpWrite(10,LOW);
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  Serial.begin(115200); //Start serial communications
  setupMPC();
  Wire.begin();
  myWire.begin(); 
  pinPeripheral(4, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);
  pinMode(13,OUTPUT); //Set pin 13 on SAMD21 as OUTPUT
  digitalWrite(13,HIGH); //Turn first red LED1 on
  
  Serial.println(F("Setting things up. Please wait"));
 
  mcpMode(7, OUTPUT);
  mcpWrite(7, LOW); //Turn off 12VS rail. We do not need it for this program.
  mcpMode(4, OUTPUT);
  mcpWrite(4, LOW); //Turn off 3VS rail. We do not need it for this program.
  
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
