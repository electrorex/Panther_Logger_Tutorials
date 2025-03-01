/*Using the Panther Logger (Episode 1): Getting started
 * This script demonstrates how to read the Panther Logger's onboard temperature and humidity sensor
 * T. Rex Miller, February 28, 2025
 */

#include "Panther.h"

Panther ptr;

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  Serial.begin(115200); //Start serial communications
  ptr.begin();
  Serial.println(F("Setting things up. Please wait"));
  ptr.set3v3(LOW);
  ptr.set12v(LOW);
}

void loop() {
  Serial.println("Reading the temperature and humidity sensor");
  float Temp = ptr.pTemp();
  float Humid = ptr.pHumid();
  Serial.print("Temperature (C) = "); Serial.println(Temp);
  Serial.print("Humidity (%) = "); Serial.println(Humid);
  delay(2000);
}
