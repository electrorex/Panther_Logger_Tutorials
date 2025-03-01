/*This script is a modification of the WiFiScanNets example script from the WiFi101 library
 * It has been modified to work with the ElectroRex Telelogger. 
 * -Todd R. Miller, November 25, 2023
 */

//libraries to include
#include <WiFi101.h> //main library for WiFi communications
#include "driver/include/m2m_periph.h" //driver file for WINC1500 that allows to turn on LED
#include <Panther.h>

Panther ptr;

int rssi;

WiFiClient client;

void wincOn(){
  ptr.set3v3(LOW);
  ptr.mcpMode(15, OUTPUT);
  ptr.mcpWrite(15, LOW);
  delay(100);
  ptr.set3v3(HIGH);
  delay(100);
  ptr.mcpWrite(15, HIGH);
}

void printWiFiMac() {
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
    Serial.flush();
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
  }
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void setup() {
  delay(2000);
  Serial.begin(9600);
  
  // Print a welcome message
  Serial.println("WiFi101 scan networks.");
  Serial.println(); // A a line space

  ptr.begin();
  wincOn();

  WiFi.setPins(9,7,ATN,-1);
  //CS, IRQ, RST, Enable

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  printWiFiMac();
}

void loop() {
  delay(1000);
  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1);
  delay(100);
  m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0);  
  delay(5000); 
  Serial.println("Scanning available networks...");
  WiFi.lowPowerMode();
  listNetworks();
  WiFi.end();
  delay(10000);
}
