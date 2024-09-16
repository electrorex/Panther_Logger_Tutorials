/*This script is a modification of the WiFiScanNets example script from the WiFi101 library
 * It has been modified to work with the ElectroRex Panther Logger. 
 * -Todd R. Miller, November 25, 2023
 */

//libraries to include
#include <WiFi101.h> //main library for WiFi communications
#include "Adafruit_MCP23X17.h" //Adafruit library to control the MCP23017 GPIO expander on the Panther Logger board
#include "driver/include/m2m_periph.h" //driver file for WINC1500 that allows to turn on LED

Adafruit_MCP23X17 mcp;

int rssi;

WiFiClient client;

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
  Serial.begin(9600);
  
  // Print a welcome message
  Serial.println("WiFi101 scan networks.");
  Serial.println(); // A a line space

  //The wire library allows communications with I2C devices. 
  //The MCP GPIO expander is an I2C device and we need it to turn on the 3.3V power rail and the WiFi modem
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
  mcp.digitalWrite(4,HIGH);
  delay(1000);
  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1);
  delay(100);
  m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0);  
  delay(5000); 
  Serial.println("Scanning available networks...");
  WiFi.lowPowerMode();
  listNetworks();
  WiFi.end();
  mcp.digitalWrite(4,LOW);
  delay(10000);
}
