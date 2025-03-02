/*This script will send data to Thinger.io using the Panther Logger's WiFi modem
 * This script goes with the Electrorex tutorial:
 * "Using the Panther Logger (Episode 3): Sending data with WiFi" 
 * The script uses the HTTP POST method to send data to the Thinger HTTP API
 */
#include <SPI.h>
#include <WiFi101.h>
#include <driver/source/nmasic.h>
#include <Panther.h>
#include "driver/include/m2m_periph.h"

Panther ptr;

float AirTemp;
float Batv;
float mvolts;                                                                                      

WiFiClient client;
char ssid[] = "Sawadii5";                                                                             //network SSID (name)
char pass[] = "SongkhlaSidRex";                                                                       // network password 
char ThingerToken[] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEZXZpY2VDYWxsYmFja19UZWxlbG9nZ2VyIiwic3ZyIjoidXMtZWFzdC5hd3MudGhpbmdlci5pbyIsInVzciI6InRyZXhtaWxsZXIifQ.jpl6k4pc1yFL8YdHOFtk9FQMDIhX0eeyLLf29Kyt72A";
char server[] = "backend.thinger.io";                                                                 //Get this from the Thinger device callback overview page

int status = WL_IDLE_STATUS; 
char ThingerString[500];                                                                              //character array to hold the POST data

void Thinger() {                                                                                      //Make a function to send data to the Thinger.io
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
  }
  
  while (status != WL_CONNECTED) {                                                                    //Attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);                                                                  //Connect to WPA/WPA2 network:
    // wait 
    delay(5000);
    Serial.print("Status = "); Serial.println(status);
    }
                                                                                                      //Make the payload
  sprintf(ThingerString, "{\"temperature\": %.2f,\"battery\": %.2f}",                                 //Add additional sensor name with a comma after %.2f and before bracket
          AirTemp,
          Batv                                                                                        //Add additonal sensor float value with a comma after Batv and then the float object name
          );
  Serial.print(F("Payload to send = ")); Serial.println(ThingerString);       
  Serial.print(F("Connecting to ")); Serial.println(server);

  if (client.connect(server, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    Serial.println("");
    delay(1000);                                                                                      //could make this delay longer to wait for connection
    
    client.println("POST /v3/users/trexmiller/devices/Telelogger/callback/data HTTP/1.1");            //This is the header information. Must be carefully coded. Practice with postman!
    client.println("Content-Type: application/json");
    client.println("Host: backend.thinger.io");
    client.println("User-Agent: panther/1.0");
    client.println("Cache-Control: no-cache");
    client.println("Accept: */*");
    client.println("Connection: keep-alive");
    client.print("Authorization: "); client.println(ThingerToken);
    String Data = ThingerString;
    client.print("Content-Length: "); client.println(Data.length());                                  //NOTE: There needs to be a space after the colon. Likely true for all headings
    client.println("");
    client.println(ThingerString);
    delay(2000);                                                                                      //Necessary delay here to wait for server response to fill up
    client.flush();                                                                                   
    Serial.println("Reading response from Thinger server:");                                          //Note, we can parse out the time from this. Will do so in a different tutorial
    while (client.available()) {                                                                      //Response should include
      char c = client.read();
      Serial.write(c);
    }
    Serial.println("");
    client.flush();                                                                                   //this seems to be necessary to prevent buffer issue (https://github.com/arduino-libraries/WiFi101/issues/118)
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

void setup() {
  delay(5000);
  Serial.begin(115700);                                                                                          
  Serial.println("Using WiFi modem to send data to Thinger.io using HTTP POST.");       // Print a welcome message
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
  Batv = ptr.bat();
  Serial.print(F("Battery Voltage = ")); Serial.println(Batv);
  AirTemp = ptr.pTemp();                                                                        //Make up some temperature data
  Serial.println(F("Sending sensor data to ThingSpeak with the WiFi modem"));
  Thinger();                                                                              //Send the data with our Thinger function
  Serial.println(F(""));
  Serial.println(F("Finished sending sensor data to ThingsBoard with the WiFi modem"));
  Serial.println(F("Waiting 20 seconds to repeat"));
  Serial.println(F(""));                                                                         
  delay(20000);  //Data is sent and now lets give a delay before next measurements and data send transmission
}

