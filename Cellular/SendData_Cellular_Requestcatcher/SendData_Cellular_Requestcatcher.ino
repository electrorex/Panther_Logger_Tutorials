#include "Adafruit_MCP23X17.h"
#define SerialAT Serial1

Adafruit_MCP23X17 mcp;

char url[] = "http://industrial.api.ubidots.com:80";                                                      
char UbiToken[] = "BBUS-8owHlSXlTWzuuqLbm50pcz9Zedi0eV";                                                                  
char UbiDeviceName[] = "telelogger2";                          

char PostString[200]; 
void MakePost(){ 
  float AirTemp = random(0,30);
  float Batv = random(3.2,4.2);
  sprintf(PostString, "{\"temperature\": %.2f,\"battery\": %.2f}\r\n",                                   
          AirTemp,
          Batv                                                                                      
          );
}

void postUbidots(){
  sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 8000);
  sendAT("AT+QHTTPCFG=\"contextid\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QHTTPCFG=\"responseheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(1000);
  sendAT("AT+QHTTPCFG=\"requestheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(1000);
  sendAT("AT+QHTTPCFG=\"sslctxid\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QSSLCFG=\"seclevel\",1,0", "\r\nOK", "\r\nERROR", 5000);
  delay(1000);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 6000); //Setup GPRS communication
  delay(1000);
  sendAT("AT+QICSGP=1,1,\"hologram\",\"\",\"\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QIACT=1", "\r\nOK", "\r\nERROR", 10000);
  delay(1000);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 30000); //Setup GPRS communication
  delay(1000);

  WaitReg();

  
  sendAT("AT+QHTTPURL=34,80", "\r\nCONNECT", "\r\nERROR", 8000);
  delay(1000);
  SerialAT.print("https://rex.requestcatcher.com:443");
  Serial.println("https://rex.requestcatcher.com:443");
  delay(2000);
  sendAT("","\r\nOK","\r\nERROR", 30000);
  delay(1000);
  sendAT("AT+QHTTPPOST=273,80,80", "\r\nCONNECT", "\r\nERROR", 8000);
  SerialAT.print("POST /test HTTP/1.1\r\n");
  SerialAT.print("Host: rex.requestcatcher.com:443\r\n");
  SerialAT.print("Accept: */*\r\n");
  SerialAT.print("Content-Length: 42\r\n");
  SerialAT.print("Content-Type: application/json\r\n");
  SerialAT.print("User-Agent: Telelogger/1.0\r\n");
  SerialAT.print("Cache-Control: no-cache\r\n");
  SerialAT.print("Connection: keep-alive\r\n");
  SerialAT.print("X-Auth-Token: BBUS-8owHlSXlTWzuuqLbm50pcz9Zedi0eV\r\n");
  SerialAT.print("\r\n");
  SerialAT.println("{\"temperature\": 23.00,\"battery\": 3.00}\r\n");

  Serial.print("POST /test HTTP/1.1\r\n");
  Serial.print("Host: rex.requestcatcher.com:443\r\n");
  Serial.print("Accept: */*\r\n");
  Serial.print("Content-Length: 42\r\n");
  Serial.print("Content-Type: application/json\r\n");
  Serial.print("User-Agent: Telelogger/1.0\r\n");
  Serial.print("Cache-Control: no-cache\r\n");
  Serial.print("Connection: keep-alive\r\n");
  Serial.print("X-Auth-Token: BBUS-8owHlSXlTWzuuqLbm50pcz9Zedi0eV\r\n");
  Serial.print("\r\n");
  Serial.println("{\"temperature\": 23.00,\"battery\": 3.00}\r\n");
  
  delay(6000);
  sendAT("AT+QHTTPREAD=60", "+QHTTPREAD: 0", "\r\nERROR", 60000);
  delay(1000);
  sendAT("AT+QIDEACT=1", "\r\nOK", "\r\nERROR", 2000);
  delay(1000);
}

void setup() {
  delay(5000);
  Serial.println("Setting up");
  Serial.begin(115700); 
  SerialAT.begin(115700); 
  Wire.begin();
  mcp.begin_I2C(0x20);
  mcp.pinMode(7,OUTPUT);
  mcp.digitalWrite(7,LOW); //Turn off 12VS rail
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, HIGH); //Turn on 3VS rail
  SetupCell();
  Serial.println("Setup complete");
}

void loop() {
  Serial.println("Starting a data transmission");
  postUbidots();
  Serial.println("Finished data transmission");
  Serial.println("*****************************************************************************");
  
  delay(5000);
}
