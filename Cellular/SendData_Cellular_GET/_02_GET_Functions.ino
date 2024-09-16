//Function to make the data string to send the data via GET. Update as necessary
void MakePayload(){
  sprintf(Payload, "%s=%s&field1=%.2f&field2=%.2f&field3=%.2f&field4=%.2f&field5=%d&field6=%.2f&field7=%.2f&field8=%.2f",
          Endpoint,
          Token,                            //This is a String so we use %s
          AirTemp,                                //this is a float so field one is formatted with %.2f for two decimals. If you need more significant figures then use %.3f or &.4f... etc
          Humidity,
          BaroPressure,
          WindSpeed,
          WindDirection,                          //this is an int so field 5 is formatted as %d
          DewPoint,
          WindChill,
          Batv
         );
}

void getCommand(){
  String PayloadS = Payload; 
  int PayloadLength = PayloadS.length();
  sprintf(SendCommand, "AT+QHTTPURL=%d,80",PayloadLength);
}

void GetData(){
  MakePayload();
  getCommand();
  sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 10000);
  delay(5000);
  sendAT("AT+QHTTPCFG=\"contextid\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QHTTPCFG=\"responseheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(1000);
  sendAT("AT+QHTTPCFG=\"sslctxid\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QSSLCFG=\"seclevel\",1,0", "\r\nOK", "\r\nERROR", 5000);
  delay(1000);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 6000); //Setup GPRS communication
  delay(1000);
  sendAT("AT+QICSGP=1,1,\"soracom.io\",\"\",\"\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QIACT=1", "\r\nOK", "\r\nERROR", 10000);
  delay(1000);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 30000); //Setup GPRS communication
  delay(1000);

  WaitReg();
  
  sendAT(SendCommand, "\r\nCONNECT", "\r\nERROR", 8000);
  //SerialAT.print(Payload);
  //Serial.println(Payload);
  delay(5000);
  sendAT(Payload,"\r\nOK","\r\nERROR", 30000);
  delay(1000);
  sendAT("AT+QHTTPGET=80", "\r\n+QHTTPGET", "\r\nERROR", 30000);
  delay(1000);
  sendAT("AT+QHTTPREAD=80", "+QHTTPREAD: 0", "\r\nERROR", 10000);
  delay(1000);
  sendAT("AT+QIDEACT=1", "\r\nOK", "\r\nERROR", 2000);
  delay(1000);
}
