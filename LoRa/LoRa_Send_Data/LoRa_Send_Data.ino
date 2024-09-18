/*
This script goes with the Electrorex tutorials on using the LoRa modem on the Panther Logger
It measures a DS18B20 temperature sensor and battery voltage,
then sends the data to ubidots using the LoRa modem and The Things Network.
Free accounts at Ubidots and The Things Network are required.
See tutorials in the Electrorex learning center at electrorex.io
 */

/********************************************************************************************************************/
/***************************************CHANGE THESE SETTINGS PRIOR TO DEPLOYMENT************************************/
//Can't send data too quickly to The Things Network as it violates fair use policy
//Make the interval several minutes long, like at least three minutes
//A simple delay will be used to introduce a gap between transmissions
//Change interval as necessary in minutes
int Interval = 5;

//Change these to specific values for your device
char DEVEUI[] = "AT+DEVEUI=AC1F09FFFE0C5D79";
char APPEUI[] = "AT+APPEUI=AC1F09FFF9151720";
char APPKEY[] = "AT+APPKEY=AC1F09FFFE0C5D79AC1F09FFF9151720";

//We only have one DS18B20 sensor in this tutorial, but we can replicate it a few times
#define NSENSORS 3 //Number of DS18B temperature sensors to measure

//Add the temp chain node addresses in order. Here we just have one sensor address, but we are replicating it three times
byte Address[NSENSORS][8] = {
  {0x28, 0xD7, 0xA8, 0xE2, 0x08, 0x00, 0x00, 0x39},
  {0x28, 0xD7, 0xA8, 0xE2, 0x08, 0x00, 0x00, 0x39},
  {0x28, 0xD7, 0xA8, 0xE2, 0x08, 0x00, 0x00, 0x39}
};

/************************************END OF SETTINGS NEEDING TO BE CHANGED*******************************************/
/********************************************************************************************************************/
//Libraries needed
#include <OneWire.h>
#include "Adafruit_MCP23X17.h"
#include <avr/dtostrf.h>

Adafruit_MCP23X17 mcp;

//Pin 10 and 12 are RX and TX for UART multiplexer communications
#define PIN_SERIAL2_RX       (34ul)                                           // Pin description number for PIO_SERCOM on D12
#define PIN_SERIAL2_TX       (36ul)                                           // Pin description number for PIO_SERCOM on D10
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)                                  // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)                                // SERCOM pad 3

Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
// Interrupt handler for SERCOM1
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

#define SerialAT Serial2

char result_char[5000];
int Check; //If 1 then an expected response was given by modem, otherwise if 0 unexpected or modem failed to give response
//Function to send AT commands to modem and listen for expected response, error code, or timeout after so many seconds
String sendAT(const char *toSend, const char *toCheck1, const char *toCheck2, unsigned long milliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  SerialAT.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    SerialAT.flush();
    Serial.flush();
    if (SerialAT.available()) {
      char c = SerialAT.read();
      Serial.write(c);
      result += c;  // append to the result string
      unsigned int Len = result.length();
      result.toCharArray(result_char, Len);
      if (strstr(result_char, toCheck1) || strstr(result_char, toCheck2))
      {
        milliseconds = 0;
        Check = 1;
      }
      else {
        Check = 0; 
      }
    }
  }
  return result;
  Serial.println(result);
  Serial.println("");
}

float mvolts; //millivolts read from any analog pin
float Batv;
float Temps[NSENSORS]; //array of floats for temperature data

void setup() {
  delay(3000); //short delay to get serial monitor up
  Serial.begin(115200);
  SerialAT.begin(115200);
  Serial.println("Setting up");
  Wire.begin();
  mcp.begin_I2C(); 
  
  //Turn on LEDs 2, 3, and 4 to let us know its in setup
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,HIGH);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,HIGH);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,HIGH);

  //Turn on 3.3V rail
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,HIGH); 
  delay(2000); //Let things turn on
  
  //Setup UART communications with LoRa modem and setup modem with initial AT commands
  setupLoRaUART();
  setupLoRa();
  
  //Turn off all indicator LEDs
  mcp.pinMode(8,OUTPUT);
  mcp.digitalWrite(8,LOW);
  delay(100);
  mcp.pinMode(9,OUTPUT);
  mcp.digitalWrite(9,LOW);
  delay(100);
  mcp.pinMode(10,OUTPUT);
  mcp.digitalWrite(10,LOW);
  delay(100);
  
  Serial.println("Finished Setting up. Waiting for first interval.");
  Serial.println("*********************************************************"); 
}

void loop() {
    Serial.println("-------------------------------------------------------------------------");
    Serial.println("Next interval. Reading sensors");
    delay(100);
    mcp.digitalWrite(8,HIGH); //turn on green LED to let us know its measuring sensors. All others off.
    delay(100);
    mcp.digitalWrite(9,LOW);
    delay(100);
    mcp.digitalWrite(10,LOW);
    delay(100);
    SerialAT.begin(115200); //Start serial communications with LoRa modem as it was ended at end of loop.
    
    //Read all sensors
    readBat();
    readTemps();
    mcp.digitalWrite(8,LOW); //finished reading sensors, turn green LED off
    delay(100);

    //Construct payload and send data with LoRa modem
    getPayload();
    Serial.println("Finished constructing payload. Getting ready to send data");
    mcp.digitalWrite(9,HIGH); //sending data, second red LED on
    delay(100);
    sendLoRa();
    Serial.println("Finished attemping to send data via LoRa");
    delay(100);
    Serial.println("Requesting and setting time on the RTC");
    delay(100);
    mcp.digitalWrite(9,LOW); //finished sending data, second red LED off
    delay(100);
    
    Serial.println("******************RESULTS*******************");
    Serial.print("Battery Voltage = "); Serial.println(Batv);
    Serial.print("Temperature 1 = "); Serial.println(Temps[0]);
    Serial.print("Temperature 2 = "); Serial.println(Temps[1]);
    Serial.print("Temperature 3 = "); Serial.println(Temps[2]);
    Serial.println("********************************************");
    Serial.println("");
    Serial.print("Finished. Waiting for next interval. Stopping for "); 
    Serial.print(Interval);
    Serial.println(" minutes"); 
    Serial.println("-------------------------------------------------------------------------");
    delay(Interval*1000*60);
}
