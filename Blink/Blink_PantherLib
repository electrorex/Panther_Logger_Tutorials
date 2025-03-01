/*Using the Panther Logger (Episode 1): Getting started
 * This script demonstrates how to flash or blink the indicator LEDs on the Panther board
 * T. Rex Miller, February 28, 2025
 */

#include "Panther.h"

Panther ptr;

//Below make a function to flash an LED
//LED = There are 4 indicator LEDs near the battery connector labeled 1 thorugh 4
//LEDflashes = the number of flashes to occur
//LEDontime = time in milliseconds to keep LED on during the flash
//LEDofftime = time in milliseconds to keep LED off during the flash
void flashLED(int LED, int LEDflashes, int LEDontime, int LEDofftime){
  for(int i= 0;i <= LEDflashes; i++){
    ptr.LED(LED, HIGH);
    delay(LEDontime);
    ptr.LED(LED, LOW);
    delay(LEDofftime);
  }
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  Serial.begin(115200); //Start serial communications
  ptr.begin();
  Serial.println(F("Setting things up. Please wait"));
  ptr.set3v3(LOW);
  ptr.set12v(LOW);
  ptr.LEDs(LOW); 
}

void loop() {
  //flash LED2 ten times, 100 ms on and 100 ms off
  Serial.println(F("Flashing LED 2"));
  flashLED(2,10,100,100);
  delay(2000);
  //flash LED3 ten times, 100 ms on and 100 ms off
  Serial.println(F("Flashing LED 3"));
  flashLED(3,10,100,100);
  delay(2000);
  //flash LED4 ten times, 100 ms on and 100 ms off
  Serial.println(F("Flashing LED 4"));
  flashLED(4,10,100,100);
  delay(2000);
  //Flash on all LEDs ten times at same time
  Serial.println(F("Flashing all LEDs"));
  for (int i=0; i<10; i++){
    ptr.LEDs(HIGH);
    delay(100);
    ptr.LEDs(LOW);
    delay(100);
  }

  ptr.LED(2,HIGH);
  delay(5000);
  ptr.LED(2,LOW);
  ptr.LED(3,HIGH);
  delay(5000);
  ptr.LED(3,LOW);
  ptr.LED(4,HIGH);
  delay(5000);
  ptr.LED(4,LOW);
  delay(5000); //wait five seconds and do it all over again
  Serial.println("");
}
