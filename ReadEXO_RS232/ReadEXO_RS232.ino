/*This sketch was written for the Electrorex Panther Logger tutorials
-Todd R. Miller, February 5, 2024
 
This sketch will read an EXO2 sonde through the YSI DCP adaptor on the Telelogger's RS232 serial port, TX2/RX2.

Wiring from YSI DCP Adaptor to the Telelogger
9-12VDC IN ---> 12VS
GND ----> GND
GND ----> GND (there are two GND ports on the DCP adaptor)
RS232-TX ----> RX2
RS232-RX ----> TX2

Follow the YSI EXO manual for how to wire the EXO sonde to the DCP adaptor

The RS232 port communicates with the second serial port on the Telelogger processor through the UART MUX.
The RS232 ports on the Telelogger are marked as RX1/TX1 and RX2/TX2 
These RS232 ports are selectable through the UART MUX with MCP GPIO pins 11, 12, and 13.
Set them high or low to select the correct port according to the table below.
 
Table. UART Multiplexer addressing
                    GPB11 (E) | GPB12 (S0) | GPB13 (S1)
                    ___________________________________
LoRa (RX0/TX0)     | LOW      |    LOW     |   LOW 
RS232-1 (RX1/TX1)  | LOW      |    HIGH    |   LOW
RS232-2 (RX2/TX2)  | LOW      |    LOW     |   HIGH
UART (RX3/TX3)     | LOW      |    HIGH    |   HIGH

The EXO is on the TX2/RX2 port so GPIO pins 11, 12 and 13 will be set low, low, and high, respectively.
Then the Telelogger can send/receive RS232 commands and data from the EXO on Serial2 of the processor.

This sketch will turn the EXO on with the switched 12VS rail of the Telelogger, then read EXO data and then turn it off
Each time the EXO is turned on, the wiper will be activated and the sketch waits 35 seconds before requesting data.
*/

//Libraries needed
#include "wiring_private.h" 
#include "Adafruit_MCP23X17.h"

Adafruit_MCP23X17 mcp;

//Make a new serial port, Serial2, on digital pins 10 and 12
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

#define ss Serial2 //Second Serial (ss) port communication with UART multiplexer

//Some variables needed to hold information from the EXO sonde
char EXOData[200];
const char * EXOFirst;
float EXOTempC;
float EXOspCond;
float EXOTDS;
float EXOSal;
float EXOpH;
float EXODepth;
float EXOBat;
float EXOTurb;
float EXOChl;
float EXOPhyco;
int EXODate;
int EXOTime;
int EXOWipePos;
float EXODOSat;
float EXODOmgl;                                                             

//Function to read data from EXO sonde
void readEXO() {
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, HIGH); //Turn on 3VS rail to power RS232
  mcp.pinMode(7, OUTPUT);
  mcp.digitalWrite(7, HIGH); //Turn on 12VS rail

  Serial.println("Waiting for EXO to turn on");
  delay(5000); //EXO requires about 35 seconds to start, but it has already been on for at least 30 seconds while reding other sensors, so just give another 5 seconds. 

  mcp.pinMode(11,OUTPUT); 
  mcp.digitalWrite(11,LOW); //E
  delay(100);
  mcp.pinMode(12,OUTPUT); 
  mcp.digitalWrite(12,LOW); //S0
  delay(100);
  mcp.pinMode(13,OUTPUT);
  mcp.digitalWrite(13,HIGH); //S1
  delay(100);
  while(ss.available() > 0){
    char t = ss.read(); // make sure buffer is clear
  }
  
  Serial.println("Reading EXO Sonde");
  ss.print("0\r");
  delay(100);
  Serial.println(ss.readString());
  ss.print("twipeb\r");
  delay(2000);
  Serial.println(ss.readString());
  Serial.println("Wiping");  
  
  int hwipes = 1;
  unsigned int EXONowTime = millis();
  unsigned int EXOInterval = 60000; //Give sensor 60 seconds to send good data
  if(EXONowTime + EXOInterval > millis()){ 
    while(hwipes>0){
      ss.print("hwipesleft\r");
      delay(2000); 
      hwipes = ss.parseInt();
      Serial.print("Half wipes left = "); Serial.println(hwipes); 
    }

    while(ss.available() > 0){
    char t = ss.read(); // make sure buffer is clear
    }
      ss.print("data\r");
      delay(1500);
      Serial.println(ss.readString());

      ss.print("data\r");
      delay(1500);
      Serial.println(ss.readString());

      ss.print("data\r");
      delay(1500);
      Serial.println(ss.readString());

      ss.print("data\r");
      String EXOReceived1 = ss.readString();
      EXOReceived1.toCharArray(EXOData,200);
      Serial.println(EXOData);
      
      char * strtokIndx; 
    
      strtokIndx = strtok(EXOData,"\n"); 
      EXOFirst = strtokIndx;

      strtokIndx = strtok(NULL," "); 
      EXOTempC = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOspCond = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOTDS = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOSal = atof(strtokIndx); 

      strtokIndx = strtok(NULL, " ");
      EXODepth = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOBat = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOTurb = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOWipePos = atoi(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXODOSat = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXODOmgl = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOChl = atof(strtokIndx);

      strtokIndx = strtok(NULL, " ");
      EXOPhyco = atof(strtokIndx);
  }
  
  Serial.println("====================================================");
  Serial.print("Water Temperature (C) = "); Serial.println(EXOTempC);
  Serial.print("Specific Conductivity (uS/cm) = "); Serial.println(EXOspCond);
  Serial.print("Total Dissolved Solids (mg/L) = "); Serial.println(EXOTDS);
  Serial.print("Salinity (ppt) = "); Serial.println(EXOSal);
  Serial.print("Depth (m) = "); Serial.println(EXODepth);
  Serial.print("Battery Voltage (V) = "); Serial.println(EXOBat);
  Serial.print("Turbidity (NTU) = "); Serial.println(EXOTurb);
  Serial.print("Date (DDMMYY) = "); Serial.println(EXODate);
  Serial.print("Time (HHMMSS) = "); Serial.println(EXOTime);
  Serial.print("Wiper Position (V) = "); Serial.println(EXOWipePos);
  Serial.print("Dissolved Oxygen Saturation (%) = "); Serial.println(EXODOSat);
  Serial.print("Dissolved Oxygen Concentration (mg/L) = "); Serial.println(EXODOmgl);
  Serial.print("Chlorophyll = "); Serial.println(EXOChl);
  Serial.print("Phycocyanin = "); Serial.println(EXOPhyco);
  Serial.println("====================================================");

  Serial.println("Finished reading EXO Sonde");
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  
  //Start all of the serial ports
  Serial.begin(115200);
  ss.begin(9600); //default baud rate for the EXO sonde

  //Turn on red LED1 to indicate the program has started
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  
  Serial.println("Setting things up. Please wait");

  Wire.begin();
  mcp.begin_I2C(0x20);

  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, HIGH); //Turn on 3VS rail
  delay(100);
  
  mcp.pinMode(7, OUTPUT);
  //mcp.digitalWrite(7, LOW); //Turn off 12VS rail
  delay(100);
  
  mcp.pinMode(8, OUTPUT);
  mcp.digitalWrite(8, LOW); //Turn off indicator LED2
  delay(100);
  
  mcp.pinMode(9, OUTPUT);
  mcp.digitalWrite(9, LOW); //Turn off indicator LED3
  delay(100);
  
  mcp.pinMode(10, OUTPUT);
  mcp.digitalWrite(10, LOW); //Turn off indicator LED4
  delay(100);
  
  Serial.println("Setup finished");
}

void loop() {
  readEXO();
  delay(10000); //Wait ten seconds before doing another read
}
