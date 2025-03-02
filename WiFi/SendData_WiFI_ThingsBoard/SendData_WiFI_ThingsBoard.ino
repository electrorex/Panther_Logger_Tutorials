/*This script will send data to ThingsBoard using the Panther Logger's WiFi modem
 * This script goes with the Electrorex tutorial:
 * "Using the Panther Logger (Episode 3): Sending data with WiFi" 
 * The script uses the HTTP POST method to send data to the ThingsBoard API
 */
#include <SPI.h>
#include <WiFi101.h>
#include <driver/source/nmasic.h>
#include <Panther.h>
#include "driver/include/m2m_periph.h"

Panther ptr;

float AirTemp;
float Batv;

WiFiClient client;
char ssid[] = "Sawadii5";                                                             //network SSID (name)
char pass[] = "SongkhlaSidRex";                                                       // network password 
char server[] = "thingsboard.cloud";                                                  //The server of your ThingsBoard dashboard
char Token[] = "LuwAEKa6dcgxtS8CI88L";   

void wincOn(){
  ptr.set3v3(LOW);
  ptr.mcpMode(15, OUTPUT);
  ptr.mcpWrite(15, LOW);
  delay(100);
  ptr.set3v3(HIGH);
  delay(100);
  ptr.mcpWrite(15, HIGH);
}

int status = WL_IDLE_STATUS; 
char ThingString[500];                                                                //character array to hold the POST data

void ThingBoard() {                                                                   //Make a function to send data to the ThingsBoard
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
  }

  while (status != WL_CONNECTED) {                                                    //Attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);                                                  //Connect to WPA/WPA2 network:
    // wait 
    delay(5000);
    Serial.print("Status = "); Serial.println(status);
    }
                                                                                      //Make the payload
  sprintf(ThingString, "{temperature: %.2f,battery: %.2f}",                           //Add additional sensor name with a comma after %.2f and before bracket
          AirTemp,
          Batv                                                                        //Add additonal sensor float value with a comma after Batv and then the float object name
          );
  Serial.print(F("Payload to send = ")); Serial.println(ThingString);       
  Serial.print(F("Connecting to ")); Serial.println(server);

  if (client.connect(server, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    Serial.println("");
    delay(1000);                                                                       //could make this delay longer to wait for connection
    client.print("POST /api/v1/"); client.print(Token);client.println("/telemetry HTTP/1.1");            //This is the header information
    client.println("Host: thingsboard.cloud");                                         //Note that each header component is on a new line with println, not print
    client.println("User-Agent: telelogger/1.0");
    client.println("Content-Type:application/json");
    String Data = ThingString;
    client.print("Content-Length:");client.println(Data.length());
    client.println("");
    client.println(ThingString);
    delay(2000);                                                                        //Give a little delay here to wait for server response to fill up
    client.flush();
    Serial.println("Reading response from ThingsBoard:");                               //Note, we can parse out the time from this. Will do so in a different tutorial
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    Serial.println("");
    client.flush();                                                                     //this seems to be necessary to prevent buffer issue (https://github.com/arduino-libraries/WiFi101/issues/118)
    client.stop();
  } else {
    Serial.println("connection failed");
  }
}

void setup() {
  delay(5000);
  Serial.begin(115700);                                                                                          
  Serial.println("Using WiFi modem to send data to ThingsBoard using HTTP POST.");      // Print a welcome message
  Serial.println();
 
  ptr.begin();
  wincOn();
  WiFi.setPins(9,7,ATN,-1);                                                              //Tell WiFi101 library the WiFi pins needed for communications on the Telelogger

  if (WiFi.status() == WL_NO_SHIELD) {                                                   //Check for the presence of the WiFi modem:
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1);                                          //This sets GPIO pin 6 on WiFi module to output, which is tied to green LED for WiFi Power
  delay(100);
  m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0);                                          //This sets GPIO pin 6 to ground and the LED glows
  Serial.println("Setup is finished");
}

void loop() {
  Serial.println(F("Reading sensor data"));
  ptr.bat();
  Serial.print(F("Battery Voltage = ")); Serial.println(Batv);
  AirTemp = ptr.pTemp();                                                                        //Make up some temperature data
  Serial.println(F("Sending sensor data to ThingSpeak with the WiFi modem"));
  ThingBoard();                                                                           //Send the data with our ThingSpeak function
  Serial.println(F(""));
  Serial.println(F("Finished sending sensor data to ThingsBoard with the WiFi modem"));
  Serial.println(F("Waiting 20 seconds to repeat"));
  Serial.println(F(""));                                                                         
  delay(20000);                                                                            //Data is sent and now lets give a delay before next measurements and data send transmission
}
