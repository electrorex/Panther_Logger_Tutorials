/*This script is a template for sending data to an internet data base using the Panther Logger board and
 * the Airgain Quectel BG96 modem using HTTP Post commands.
 *Todd Miller March 6, 2023
*/
#include <Panther.h>
#include <OneWire.h>
#define SerialAT Serial1

Panther ptr;

//String variables to post data that need to be changed
char url[] = "https://industrial.api.ubidots.com";
char Host[] = "industrial.api.ubidots.com:443";
char Token[] = "X-Auth-Token: XXXXXXXXXXXXXXXXXXXXXXXXXXX";                                                                  
char Destination[] = "/api/v1.6/devices/pantherlogger";
char ContentLength[30]; 
char UserAgent[] = "PantherLogger/1.0";    
char ContentType[] = "application/json"; 
char DataString[900];      
char urlCommand[200]; 
char PostCommand[200];                                        
char Payload[1000]; //Make this large enough to hold expected post data plus ~300 to 500 for header.

void setup() {
  delay(5000);
  Serial.println("Setting up Panther Logger to send data to Ubidots");
  Serial.begin(115700); 
  SerialAT.begin(115700);
  ptr.begin(); 

  //Turn on red LED so we know board has power
  ptr.LED(1,HIGH);
  ptr.set12v(LOW);
  ptr.set3v3(HIGH);

  //Turn off all other LEDs to start
  ptr.LED(2,LOW);
  ptr.LED(3,LOW);
  ptr.LED(4,LOW);
  
  SetupCell();
  Serial.println("Setup complete");
}

void loop() {
  ptr.LED(2,HIGH); //Turn on LED2
  Serial.println("Starting a data transmission");
  Serial.println("Reading sensors");
  ptr.LED(3,HIGH);
  ptr.set12v(HIGH);
  delay(3000);
  readSensors(); 
  ptr.set12v(LOW);
  delay(1000);
  Serial.println("Posting sensor data");
  ptr.LED(3,LOW); //Turn off LED3
  ptr.LED(4,HIGH); //Turn on LED4
  setupPOST();
  PostData();
  ptr.LED(4,LOW); //Turn off LED4
  ptr.LED(2,LOW); //TUrn off LED2
  Serial.println("Finished data transmission");
  Serial.println("*****************************************************************************");
  delay(10000);
}
