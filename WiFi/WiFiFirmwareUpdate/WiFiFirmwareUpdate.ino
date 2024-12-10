/*
  This is a modification of the FirmwareUpdater script in the WiFi101 library 
  to make it work with the Panther Logger board.
*/

#include <WiFi101.h>
#include <spi_flash/include/spi_flash.h>
#include "Adafruit_MCP23X17.h"
#include "driver/include/m2m_periph.h"

Adafruit_MCP23X17 mcp;

typedef struct __attribute__((__packed__)) {
  uint8_t command;
  uint32_t address;
  uint32_t arg1;
  uint16_t payloadLength;

  // payloadLenght bytes of data follows...
} UartPacket;

static const int MAX_PAYLOAD_SIZE = 1024;

#define CMD_READ_FLASH        0x01
#define CMD_WRITE_FLASH       0x02
#define CMD_ERASE_FLASH       0x03
#define CMD_MAX_PAYLOAD_SIZE  0x50
#define CMD_HELLO             0x99

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mcp.begin_I2C();
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,LOW); 
  delay(100);
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,LOW); 
  delay(100);
  mcp.pinMode(4,OUTPUT);
  mcp.digitalWrite(4,HIGH);
  delay(100);
  mcp.pinMode(15, OUTPUT);
  mcp.digitalWrite(15,HIGH); 
  
  WiFi.setPins(9,7,3,-1);
  //CS, IRQ, RST

  m2m_periph_gpio_set_dir(M2M_PERIPH_GPIO6,1);
  //m2m_periph_gpio_set_val(M2M_PERIPH_GPIO6,0);
  
  nm_bsp_init();
  if (m2m_wifi_download_mode() != M2M_SUCCESS) {
    Serial.println(F("Failed to put the WiFi module in download mode"));
    while (true)
      ;
  }
}

void receivePacket(UartPacket *pkt, uint8_t *payload) {
  // Read command
  uint8_t *p = reinterpret_cast<uint8_t *>(pkt);
  uint16_t l = sizeof(UartPacket);
  while (l > 0) {
    int c = Serial.read();
    if (c == -1)
      continue;
    *p++ = c;
    l--;
  }

  // Convert parameters from network byte order to cpu byte order
  pkt->address = fromNetwork32(pkt->address);
  pkt->arg1 = fromNetwork32(pkt->arg1);
  pkt->payloadLength = fromNetwork16(pkt->payloadLength);

  // Read payload
  l = pkt->payloadLength;
  while (l > 0) {
    int c = Serial.read();
    if (c == -1)
      continue;
    *payload++ = c;
    l--;
  }
}

// Allocated statically so the compiler can tell us
// about the amount of used RAM
static UartPacket pkt;
static uint8_t payload[MAX_PAYLOAD_SIZE];

void loop() {
  receivePacket(&pkt, payload);

  if (pkt.command == CMD_HELLO) {
    if (pkt.address == 0x11223344 && pkt.arg1 == 0x55667788)
      Serial.print("v10000");
  }

  if (pkt.command == CMD_MAX_PAYLOAD_SIZE) {
    uint16_t res = toNetwork16(MAX_PAYLOAD_SIZE);
    Serial.write(reinterpret_cast<uint8_t *>(&res), sizeof(res));
  }

  if (pkt.command == CMD_READ_FLASH) {
    uint32_t address = pkt.address;
    uint32_t len = pkt.arg1;
    if (spi_flash_read(payload, address, len) != M2M_SUCCESS) {
      Serial.println("ER");
    } else {
      Serial.write(payload, len);
      Serial.print("OK");
    }
  }

  if (pkt.command == CMD_WRITE_FLASH) {
    uint32_t address = pkt.address;
    uint32_t len = pkt.payloadLength;
    if (spi_flash_write(payload, address, len) != M2M_SUCCESS) {
      Serial.print("ER");
    } else {
      Serial.print("OK");
    }
  }

  if (pkt.command == CMD_ERASE_FLASH) {
    uint32_t address = pkt.address;
    uint32_t len = pkt.arg1;
    if (spi_flash_erase(address, len) != M2M_SUCCESS) {
      Serial.print("ER");
    } else {
      Serial.print("OK");
    }
  }
}

