#include "wiring_private.h" 
#include "Adafruit_MCP23X17.h"

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

void setupLoRa(){
  mcp.pinMode(11,OUTPUT); 
    delay(1000);
  mcp.digitalWrite(11,LOW);
  delay(1000);
  mcp.pinMode(12,OUTPUT); 
    delay(1000);
  mcp.digitalWrite(12,LOW);
  delay(1000);
  mcp.pinMode(13,OUTPUT);
    delay(1000);
  mcp.digitalWrite(13,LOW);
  delay(1000);
  
  while(SerialAT.available() > 0){
    char t = SerialAT.read(); // make sure buffer is clear
  }
}

void setup() {
  delay(3000);
  Serial.begin(115200);
  SerialAT.begin(115200);
  Serial.println("Setting up");
  Wire.begin();
  mcp.begin_I2C(); 
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,HIGH); 
  delay(1000);
  mcp.pinMode(14,OUTPUT);
  mcp.digitalWrite(14,LOW); 
  delay(2000);
  mcp.digitalWrite(14,HIGH);
  
  delay(1000);
  setupLoRa();
  delay(1000);
  Serial.println("Finished Setting up"); 
}

void loop() {
    if (SerialAT.available()) {
      Serial.write(SerialAT.read());
    }
    if (Serial.available()) {
      SerialAT.write(Serial.read());
    }
}
