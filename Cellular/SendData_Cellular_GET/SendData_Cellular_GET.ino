/*This script is a template for sending data to an internet data base using the Telelogger board and
 * the Airgain Quectel BG96 modem using HTTP Post commands.
 *Todd Miller December 4, 2023
*/
#include <Panther.h>
#define SerialAT Serial1

Panther ptr;

//String variables to post data that need to be changed
char Token[] = "XXXXXXXXXXXXXXX";                                                                  
char GetCommand[100]; 
char Payload[2000]; //Make this large enough to hold expected data plus the endpoint
char Endpoint[] = "http://api.thingspeak.com/update.json?api_key";
char SendCommand[100];

void setup() {
  delay(5000);
  Serial.println("Setting up");
  Serial.begin(115700); 
  SerialAT.begin(115700);
  ptr.begin();

  //Turn on red LED so we know board has power
  ptr.LED(1,HIGH);
  
  ptr.set12v(LOW);
  ptr.set3v3(HIGH);
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
  ptr.LED(3,HIGH); //Turn on LED3
  readSensors();
  delay(3000);
  Serial.println("Posting sensor data");
  ptr.LED(3,LOW); //Turn off LED3
  ptr.LED(4,HIGH); //Turn on LED4
  GetData();
  ptr.LED(4,LOW); //Turn off LED4
  ptr.LED(2,LOW); //TUrn off LED2
  Serial.println("Finished data transmission");
  Serial.println("*****************************************************************************");
  delay(5000);
}
