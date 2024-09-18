void setupLoRaUART(){
  mcp.pinMode(11,OUTPUT); 
  delay(100);
  mcp.digitalWrite(11,LOW);
  delay(100);
  mcp.pinMode(12,OUTPUT); 
  delay(100);
  mcp.digitalWrite(12,LOW);
  delay(100);
  mcp.pinMode(13,OUTPUT);
  delay(100);
  mcp.digitalWrite(13,LOW);
  delay(100  );
  while(SerialAT.available() > 0){
    char t = SerialAT.read(); // make sure buffer is clear
  }
}

void setupLoRa(){
  //hard reset of the module
  mcp.pinMode(14,OUTPUT);
  mcp.digitalWrite(14,LOW);
  delay(1000); 
  mcp.pinMode(14,OUTPUT);
  mcp.digitalWrite(14,HIGH); 
  delay(1000);
  sendAT("AT", "OK", "AT_ERROR", 1000); //Wake up
  delay(100);
  sendAT("AT", "OK", "AT_ERROR", 1000); //Wake up
  delay(100);
  sendAT("AT+LPMLVL=1", "OK", "AT_ERROR", 5000); //AT+LPMLVL set sleep mode to 1 so UART can wake it up
  delay(100);
  sendAT("AT+NJM=1", "OK", "AT_ERROR", 5000); //Set network join mode to OTAA
  delay(100);
  sendAT("AT+NWM=1", "LoRaWAN.", "AT_ERROR", 10000); //Set to LoRaWAN mode
  delay(2000);
  sendAT("AT", "OK", "AT_ERROR", 1000); //Wake up
  delay(100);
  sendAT("AT+PNM=1", "OK", "AT_ERROR", 5000); //Set to public network mode
  delay(100);
  sendAT("AT+CFM=0", "OK", "\r\nAT_ERROR", 2000); //Do not require confirmation of message sent
  delay(100);
  sendAT("AT+CLASS=A", "OK", "\r\nAT_ERROR", 2000); //Set to class A for lowest power
  delay(100);
  sendAT("AT+BAND=5", "OK", "\r\nAT_ERROR", 2000); //Set band to US915
  delay(100);
  sendAT("AT+MASK=0002", "OK", "\r\nAT_ERROR", 2000); //Set channel mask to 2
  delay(100);
  sendAT("AT+ADR=1", "OK", "\r\nAT_ERROR", 2000); //Turn on adaptive data rate
  delay(100);
  sendAT("AT+DR=1", "OK", "\r\nAT_ERROR", 2000); //Set data rate, but this is managed by ADR
  delay(100);
  sendAT("AT+RETY=2", "OK", "\r\nAT_ERROR", 2000); //Transmission retries set to 2
  delay(100);
  sendAT(DEVEUI, "OK", "\r\nERROR", 2000);
  delay(100);
  sendAT(APPEUI, "OK", "\r\nERROR", 2000);
  delay(100);
  sendAT(APPKEY, "OK", "\r\nERROR", 2000); 
  delay(100);
  sendAT("AT+JOIN=1:0:7:2", "+EVT:JOINED", "\r\nERROR", 60000);
  delay(2000);
}

void sendLoRa(){
  sendAT("AT", "WAKE_UP", "AT_ERROR", 1000); //Wake up
  delay(100);
  sendAT("AT", "OK", "AT_ERROR", 1000); //Wake up
  delay(100);
  sendAT("AT", "OK", "AT_ERROR", 1000); //Wake up
  delay(100);
  sendAT("AT+LPM=0", "OK", "AT_ERROR", 5000); //Turn low power mode off
  delay(100);
  sendAT("AT+TIMEREQ=1", "OK", "ERROR", 8000); //Requrie device to get time from server
  delay(100);
  sendAT(Payload,"OK", "ERROR", 8000); //Send the data
  delay(100);
  sendAT("AT+LPM=1", "OK", "AT_ERROR", 5000); //AT+LPM=1 allow it to sleep automatically
  delay(1000);
}
