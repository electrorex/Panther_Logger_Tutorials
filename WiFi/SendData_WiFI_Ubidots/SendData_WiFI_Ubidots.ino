/*This script will send data to Ubidots using the Panther Logger's WiFi modem
 * This script goes with the Electrorex tutorial:
 * "Using the Panther Logger (Episode 3): Sending data with WiFi" 
 * The script uses the HTTP POST method to send data to the Ubidots HTTP API
 */

//Use as few libraries as possible
#include <SPI.h>
#include <WiFi101.h>
#include "Adafruit_MCP23X17.h"
#include "driver/include/m2m_periph.h"

Adafruit_MCP23X17 mcp;

float AirTemp;
float Batv;
float mvolts;                                                                                      

float readVolts(int pin) {                                                                          //Make a function to read voltage on some analog pin and output in millivolts
  analogReadResolution(12);                                                                         //Set analog read resolution to 12 (4096 steps)
  mvolts = analogRead(pin); 
  mvolts *= 2;
  mvolts *= (3.3 / 4096);
  mvolts *= 1000;
  return mvolts;
}
                                                                                                    //Function to read Battery voltage on pin A4 on Panther Logger
void readBat() {
  Batv = readVolts(A4)/1000;                                                                        //Read battery on A4 and convert millivolts to volts
}

WiFiClient client;

//Enter user specific information below. See comments for explanations
char ssid[] = "Sawadii5";                                                                        //network SSID (name)
char pass[] = "SongkhlaSidRex";                                                                        // network password 
char server[] = "industrial.api.ubidots.com";                                                       //Ubidots server. Always check to make sure this has not changed
char UbiToken[] = "BBUS-ykRndbAMiq8hXLvWKMUPASIN6oRSxw";                                                            //Get a token from the device page after creating a device in your profile at Ubidots
char UbiDeviceName[] = "PantherLogger";                                                                //This is the name you give your device at Ubidots when you create a new device.

int status = WL_IDLE_STATUS; 
char UbiString[500];                                                                                //character array to hold the POST data

void Ubidots() {                                                                                    //Make a function to send data to the Thinger.io
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
  }

  
  while (status != WL_CONNECTED) {                                                                  //Attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);                                                                //Connect to WPA/WPA2 network:
    // wait 
    delay(5000);
    Serial.print("Status = "); Serial.println(status);
    }
                                                                                                    //Make the payload
  sprintf(UbiString, "{\"temperature\": %.2f,\"battery\": %.2f}",                                   //Add additional sensor name with a comma after %.2f and before bracket
          AirTemp,
          Batv                                                                                      //Add additonal sensor float value with a comma after Batv and then the float object name
          );
  Serial.print(F("Payload to send = ")); Serial.println(UbiString);       
  Serial.print(F("Connecting to ")); Serial.println(server);

  if (client.connect(server, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    Serial.println("");
    delay(1000);                                                                                     //could make this delay longer to wait for connection


    /*Below is the header information.
     *If you run into trouble testing this in postman and requestchatcher is highly recommended 
     */
    client.print("POST /api/v1.6/devices/"); client.print(UbiDeviceName); client.println(" HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Host: industrial.api.ubidots.com");
    client.println("User-Agent: Panther Logger/1.0");
    client.println("Cache-Control: no-cache");
    client.println("Accept: */*");
    client.println("Connection: keep-alive");
    client.print("X-Auth-Token: "); client.println(UbiToken);
    String Data = UbiString;
    client.print("Content-Length: "); client.println(Data.length());                                 //NOTE: There needs to be a space after the colon. Likely true for all headings
    client.println("");                                                                              //This new line between header and body (data message) is necessary
    client.println(UbiString);                                                                       //Send the data!
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

void setup() {
  delay(5000);
  Serial.begin(115700);                                                                                          
  Serial.println("Using WiFi modem to send data to Ubidots using HTTP POST.");                        //Print a welcome message
  Serial.println();
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  Wire.begin();                                                                                       //Do the setup functions necessary to power on the WiFi modem
  mcp.begin_I2C();
  mcp.pinMode(4,OUTPUT);                                                                              //Turn off 3.3V rail
  mcp.digitalWrite(4,LOW); 
  delay(1000);
  mcp.pinMode(15, OUTPUT);                                                                            //Set enable WiFi pin low
  mcp.digitalWrite(15,LOW); 
  delay(1000);
  mcp.pinMode(4,OUTPUT);                                                                              //Turn on 3.3V switched rail and thus, the WiFi modem (ensure dip switch is on)
  mcp.digitalWrite(4,HIGH);
  delay(1000);
  mcp.pinMode(15, OUTPUT);                                                                            //Set enable WiFi pin high to enable the module
  mcp.digitalWrite(15,HIGH); 
  WiFi.setPins(9,7,3,-1);                                                                             //Tell WiFi101 library the WiFi pins needed for communications on the Panther Logger
                                                                                                      //The pin order is CS, IRQ, RST, Enable
  if (WiFi.status() == WL_NO_SHIELD) {                                                                //Check for the presence of the WiFi modem:
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1);                                                        //This sets GPIO pin 6 on WiFi module to output, which is tied to green LED for WiFi Power
  delay(100);
  m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0);                                                        //This sets GPIO pin 6 to ground and the LED glows
  Serial.println("Setup is finished");
}

void loop() {
  Serial.println(F("Reading sensor data"));
  readBat();
  Serial.print(F("Battery Voltage = ")); Serial.println(Batv);
  AirTemp = 23.45;                                                                                    //Make up some temperature data
  Serial.println(F("Sending sensor data to Ubidots with the WiFi modem"));
  Ubidots();                                                                                          //Send the data with our Ubidots function
  Serial.println(F(""));
  Serial.println(F("Finished sending sensor data to Ubidots with the WiFi modem"));
  Serial.println(F("Waiting 60 seconds to repeat"));
  Serial.println(F(""));                                                                         
  delay(60000);                                                                                       //Data is sent and now lets give a delay before next measurements and data send transmission                                                                                                      //In subsequent tutorial we will show how to time the loop so it runs at predtermined rates.
}
