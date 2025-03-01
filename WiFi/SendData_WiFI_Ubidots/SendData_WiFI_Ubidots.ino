/*This script will send data to Ubidots using the Electrorex Panther Logger's's WiFi modem
* This script goes with the Electrorex tutorials, https://electrorex.io/pantherlogger-tutorials
* - T. Rex Miller, 2025-03-01
*/

#include <SPI.h>
#include <WiFi101.h>
#include "Panther.h"
#include "driver/include/m2m_periph.h"
#include "driver/include/m2m_wifi.h"

Panther ptr;  

WiFiClient client;
int status = WL_IDLE_STATUS;                                                                       //Instantiate panther library class                                                                          

//Enter user specific information below. See comments for explanations
char ssid[] = "Sawadii5";                                                             //network SSID (name)
char pass[] = "SongkhlaSidRex";                                                       // network password                                                      
char Token[] = "BBUS-ykRndbAMiq8hXLvWKMUPASIN6oRSxw";                                 //Get a token from Ubidots
char Device[] = "pantherlogger";                                                      //This is the name you give your device at Ubidots

void ubidots(char * Data, char * Token, char * Device, char * ssid, char * pass){
  status = 0;
  while (status != WL_CONNECTED) {                                                                  //Attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);                                                                //Connect to WPA/WPA2 network:
    delay(5000);
    Serial.print("Status = "); Serial.println(status);
    }
    
  if(client.connect("industrial.api.ubidots.com", 80)) {
      Serial.print("connected to ");
      Serial.println(client.remoteIP());
      Serial.println("");
      delay(1000);    

    client.print("POST /api/v1.6/devices/"); client.print(Device); client.println(" HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Host: industrial.api.ubidots.com");
    client.println("User-Agent: pantherlogger/1.0");
    client.println("Cache-Control: no-cache");
    client.println("Accept: */*");
    client.println("Connection: keep-alive");
    client.print("X-Auth-Token: "); client.println(Token);
    String DataS = Data;
    client.print("Content-Length: "); client.println(DataS.length());                                 //NOTE: There needs to be a space after the colon. Likely true for all headings
    client.println("");                                                                              //This new line between header and body (data message) is necessary
    client.println(Data);                                                                       //Send the data!
    delay(2000);                                                                                     //Necessary delay here to wait for server response to fill up
    client.flush();                                                                                   
    Serial.println("Reading response from Ubidots server:");                                         //Note, we can parse out the time from this. Will do so in a different tutorial
    while (client.available()) {                                                                     //Response should include
      char c = client.read();
      Serial.write(c);
    }
    Serial.println("");
    client.flush();                                                                                  //this seems to be necessary to prevent buffer issue (https://github.com/arduino-libraries/WiFi101/issues/118)
    client.stop();
  } else {
    Serial.println("connection failed");
  }
}

void wincOn(){
  ptr.set3v3(LOW);
  ptr.mcpMode(15, OUTPUT);
  ptr.mcpWrite(15, LOW);
  delay(100);
  ptr.set3v3(HIGH);
  delay(100);
  ptr.mcpWrite(15, HIGH);
}

//Make a float for each data variable we are measuring with sensors
float AirTemp;
float Batv; 
float Humid;

void readSensors(){
  AirTemp = ptr.pTemp();                                                  //Read onboard temperature sensor
  Batv = ptr.bat();                                                       //Read battery voltage
  Humid = ptr.pHumid();                                                   //Read onboard humidity sensor
}

//Use sprintf function to put float variables in a json formatted character string
char Data[400];
void dataPrep(){
  sprintf(Data, "{\"temperature\": %.2f,\"battery\": %.2f,\"humidity\": %.2f}\r\n\r\n",                                   
          AirTemp,
          Batv,
          Humid                                                                                      
          );
}

void setup() {
  delay(3000);                                                        //Short delay to get serial monitor up
  Serial.begin(115700);                                               //Start serial port                                                                     
  Serial.println(F("Setting up"));                                    //Print a start message so we know it is working
  Serial.println();                                                   //Line break, give a space
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
  ptr.LED(1,HIGH);                                                     //Turn on LED1 to indicate script is running
  delay(1000);
  Serial.println(F("Setup is finished"));                             //Indicate setup is finished
}

void loop() {
  readSensors();
  dataPrep();
  ubidots(Data, Token, Device, ssid, pass);
  //Need a delay, there are limitations by Ubidots on how fast data can be sent
  //With a STEM account, in general do not send data more than once per ten minutes
  //However, in testing, it is fine to send a few data points every 10 or 20 seconds.
  delay(40000);                                                                                                   //In subsequent tutorial we will show how to time the loop so it runs at predtermined rates.
}
