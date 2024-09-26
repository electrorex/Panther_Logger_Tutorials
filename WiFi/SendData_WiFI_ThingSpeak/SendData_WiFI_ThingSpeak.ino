/*This script will send data to ThingSpeak using the Panther Logger's WiFi modem
 * This script goes with the Electrorex tutorial:
 * "Using the Panther Logger (Episode 3): Sending data with WiFi" 
 * 
 * This is a modification of the original script to disable WiFi modem and sleep processor
 */

#include <SPI.h>
#include <WiFi101.h>
#include "Adafruit_MCP23X17.h"
#include "driver/include/m2m_periph.h"
#include <Adafruit_SleepyDog.h>

Adafruit_MCP23X17 mcp;


//Create objects to hold mock sensor data. 
//Most sensor data will likely be a floating point decimal
//Some sensor data may be fine as an integer (int)
float AirTemp;
float Humidity;
float BaroPressure;
float WindSpeed;
int WindDirection;
float DewPoint;
float WindChill;
float mvolts; 
float Batv;

//Function to read voltage on some analog pin and output in millivolts
float readVolts(int pin) {
  analogReadResolution(12); //Set to highest resolution
  mvolts = analogRead(pin);
  mvolts *= 2;
  mvolts *= (3.3 / 4096);
  mvolts *= 1000;
  return mvolts;
}

//Function to read Battery voltage on pin A4 on Panther Logger
void readBat() {
  Batv = readVolts(A4)/1000; //Read battery on A4 and convert millivolts to volts
}

WiFiClient client;
char ssid[] = "Sawadii5";        //network SSID (name)
char pass[] = "xxxxlassss";    // network password 

int status = WL_IDLE_STATUS; 

//Create C-Stings (character arrays) to hold necessary items for communication
char server[] = "api.thingspeak.com";             //server address we will connect to at ThingSpeak
char ThingString[500];                            //character array to hold the url and data... this is the GET url
char ThingAPIKey[] = "WQ64EH9Y16U37490";          //String to hold the api key we get from ThingSpeak. Change to your key, gotten from the channel settings page

//Make a function to put the data into a character array containing the GET url
//The GET url contains the address to send the data, the key and the data that goes into each field in the channel
//We called the character array to hold our GET url ThingString. 
void ThingSpeak() {
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
  }

  
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 
    
    delay(5000);
    Serial.print("Status = "); Serial.println(status);
    }

  sprintf(ThingString, "GET https://api.thingspeak.com/update.json?api_key=%s&field1=%.2f&field2=%.2f&field3=%.2f&field4=%.2f&field5=%d&field6=%.2f&field7=%.2f&field8=%.2f",
          ThingAPIKey,                            //This is a String so we use %s
          AirTemp,                                //this is a float so field one is formatted with %.2f for two decimals. If you need more significant figures then use %.3f or &.4f... etc
          Humidity,
          BaroPressure,
          WindSpeed,
          WindDirection,                          //this is an int so field 5 is formatted as %d
          DewPoint,
          WindChill,
          Batv
         );
  Serial.print(F("Payload to send = ")); Serial.println(ThingString);       
  Serial.print(F("Connecting to ")); Serial.println(server);

  //Make a connection to api.thingspeak.com with the connect function
  if (client.connect(server, 80)) {

    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    Serial.println("");
    delay(100);                                    //could make this delay longer to wait for connection
  } else {
    Serial.println("connection failed");
  }

  //If we are connected then we can send the data
    client.println(ThingString); //send the GET url
    delay(1000); //Wait a second before reading server response

    //It is not necessary to read the server response and in some cases you may not want to wait to do that, but for now helpful to see how it is working.
    Serial.println("Reading response from ThingSpeak");
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    Serial.println("");
    client.flush(); //this seems to be necessary to prevent buffer issue (https://github.com/arduino-libraries/WiFi101/issues/118)
    client.stop(); //End the transmission
}

//Function to sleep
int SleepIntervals = 10;
void Sleep(){
  for (int i = 0; i < SleepIntervals; ++i) {
      Watchdog.sleep(6000);
  } 
}

void setup() {
  delay(5000);
  Serial.begin(115700);
  
  // Print a welcome message
  Serial.println("Panther Logger send data with WiFi to ThingSpeak.");
  Serial.println();

  //Do the setup functions necessary to power on the WiFi modem
  Wire.begin();
  mcp.begin_I2C();

  //Turn off 3.3V rail
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,LOW); 
  delay(100);

  //Set enable WiFi pin low
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,LOW); 
  delay(100);

  //Turn on 3.3V switched rail and thus, the WiFi modem (ensure dip switch is on)
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,HIGH);
  delay(1000);

  //Set enable WiFi pin high to enable the module
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,HIGH); 
  delay(1000);

    //Tell WiFi101 library the WiFi pins needed for communications on the Panther Logger
  WiFi.setPins(9,7,3,-1);
  //CS, IRQ, RST, Enable
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  
  
  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1); //This sets GPIO pin 6 on WiFi module to output, which is tied to green LED for WiFi Power
  delay(100);
  m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0); //This sets GPIO pin 6 to ground and the LED glows

  Serial.println("Setup is finished");
}

void loop() {
  Serial.begin(115700);
  //Enable the modem after it was disabled at end of loop
  mcp.digitalWrite(15,HIGH); 
  delay(1000);

  //Force WiFi begin
  //Note, eventhough this is called in ThingSpeak function while WL_CONNECTED (below) this still is necessary in my tests
  WiFi.begin(ssid, pass);
  delay(5000); //Wait for connect, might need to be adjusted
  
  Serial.println(F("Reading sensor data"));
  //Make up some data and fill it into the reserved variables
  //In subsequent tutorials we will read actual sensor data and send it
  //For now, we will just send fake data, except for battery voltage. We will measure that here first with readBat function
  readBat();
  Serial.print(F("Battery Voltage = ")); Serial.println(Batv);
  AirTemp = 30;
  Humidity = random(0,100);
  BaroPressure = random(900,1200);
  WindSpeed = random(0,40);
  WindDirection = random(0,360);
  DewPoint = random(0,35);
  WindChill = random(0,35);
  
  Serial.println(F("Sending sensor data to ThingSpeak with the WiFi modem"));
  //Send the data with our ThingSpeak function
  ThingSpeak();

  Serial.println(F(""));
  Serial.println(F("Finished sending sensor data to ThingSpeak with the WiFi modem"));
  Serial.println(F("Waiting 20 seconds to repeat"));
  Serial.println(F(""));
  
  //End WiFi connections and disable the modem to save power.
  WiFi.end();
  mcp.digitalWrite(15,LOW); 
  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,0);
  //delay(60000); 
  Sleep();
}
