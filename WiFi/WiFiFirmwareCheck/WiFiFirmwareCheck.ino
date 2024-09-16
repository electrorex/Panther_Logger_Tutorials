  
/*
 * This example check if the firmware loaded on the WiFi101
 * The original code has been modified to make it work with the Panther Logger
 */
 
#include <SPI.h>
#include <WiFi101.h>
#include <driver/source/nmasic.h>
#include "Adafruit_MCP23X17.h"
#include "driver/include/m2m_periph.h"

Adafruit_MCP23X17 mcp;

void setup() {
  Wire.begin();
  mcp.begin_I2C();
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,LOW); 
  delay(100);
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,LOW); 
  delay(100);
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,HIGH);
  delay(100);
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,HIGH); 
  
  WiFi.setPins(9,7,3,-1);
  //CS, IRQ, RST

  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1);
  //m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0);
  
  // Initialize serial
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // Print a welcome message
  Serial.println("WiFi101 firmware check.");
  Serial.println();

  // Check for the presence of the shield
  Serial.print("WiFi101 shield: ");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    return; // don't continue
  }
  Serial.println("DETECTED");
  WiFi.begin();
  // Print firmware version on the shield
  String fv = WiFi.firmwareVersion();
  String latestFv;
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  if (REV(GET_CHIPID()) >= REV_3A0) {
    // model B
    latestFv = WIFI_FIRMWARE_LATEST_MODEL_B;
  } else {
    // model A
    latestFv = WIFI_FIRMWARE_LATEST_MODEL_A;
  }

  // Print required firmware version
  Serial.print("Latest firmware version available : ");
  Serial.println(latestFv);

  // Check if the latest version is installed
  Serial.println();
  if (fv >= latestFv) {
    Serial.println("Check result: PASSED");
  } else {
    Serial.println("Check result: NOT PASSED");
    Serial.println(" - The firmware version on the shield do not match the");
    Serial.println("   version required by the library, you may experience");
    Serial.println("   issues or failures.");
  }
  
  delay(1000);
  WiFi.end();
  delay(1000);
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,LOW);
  mcp.digitalWrite(4,LOW);
}

void loop() {

}
