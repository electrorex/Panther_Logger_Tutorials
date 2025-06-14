/*
This script is used to program the Panther Logger II to read a 
TriSonica 550P anemometer meteorological station made by Licor. 

Connections to Panther Logger
Brown wire ---> 5V
Red wire ---> GND
Black wire ---> GND
Blue wire ---> RS232 TX
Yellow wire ---> RS232 RX
*/

#include "Panther.h"

Panther ptr;

//Panther Logger's multiplexer is on Serial2 created from digital pins 10 and 12
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

#define ss Serial2 //Alias Serial2 as ss

char WindData[500];
float TriS;
float TriD;
float TriU;
float TriV;
float TriW;
float TriT;
float TriH;
float TriP;
float TriI;
float TriR;
float TriM;

void readTrisonic(){
  //Set UART MUX to correct port, RS232-1
  ptr.selectSerial2(1);
  delay(1000);
  while(ss.available() > 0){
    char t = ss.read(); // make sure buffer is clear
  }
    String WindReceived = ss.readStringUntil('\n');
    WindReceived.toCharArray(WindData,500);
    char * strtokIndx; 
    const char delimiters[] = ", ";
    strtokIndx = strtok(WindData,delimiters); 
    TriS = atof(strtok(NULL," "));
    TriD = atof(strtok(NULL," "));
    TriU = atof(strtok(NULL," "));
    TriV = atof(strtok(NULL," "));
    TriW = atof(strtok(NULL," "));
    TriT = atof(strtok(NULL," "));
    TriH = atof(strtok(NULL," "));
    TriP = atof(strtok(NULL," "));
    TriI = atof(strtok(NULL," "));
    TriR = atof(strtok(NULL," "));
    TriM = atof(strtok(NULL," "));

    Serial.print("Wind Speed (m/s) = "); Serial.println(TriS);
    Serial.print("Wind Direction (degrees) = "); Serial.println(TriD);
    Serial.print("Wind U Axis (westerly m/s) = "); Serial.println(TriU);
    Serial.print("Wind V Axis (southerly m/s) = "); Serial.println(TriV);
    Serial.print("Wind W Axis = "); Serial.println(TriW);
    Serial.print("Air Temp (C) = "); Serial.println(TriT);
    Serial.print("Humidity (%) = "); Serial.println(TriH);
    Serial.print("Pressure (hPa) = "); Serial.println(TriP);
    Serial.print("Pitch (degrees) = "); Serial.println(TriI);
    Serial.print("Roll (degrees) = "); Serial.println(TriR);
    Serial.print("Heading (degrees) = "); Serial.println(TriM);
    Serial.println("");
}

void setup() {
  delay(2000); //wait a bit to get serial monitor up
  Serial.begin(115200);
  Serial.println("Setting up");
  ss.begin(115200);
  ptr.begin();
  ptr.set3v3(LOW);
  ptr.set12v(LOW);
  ptr.LEDs(LOW);
  Serial.println("Finished setting up");
}

void loop() {
  readTrisonic();
  delay(4000);
}
