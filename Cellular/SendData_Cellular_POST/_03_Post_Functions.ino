//Function to make the data string to post. Update as necessary
void MakeDataString(){ 
  sprintf(DataString, "{\"temperature\": %.2f,\"battery\": %.2f}\r\n\r\n",                                   
          AirTemp,
          Batv                                                                                      
          );
  Serial.print("Data String = "); Serial.println(DataString);
}

void MakePayload(){
  String DataStringS = DataString;
  sprintf(ContentLength,"Content-Length: %d",DataStringS.length());
  
  //Put together the payload with headers and data string. End each line with new line carraige return \r\n
  strcpy(Payload,"POST "); strcat(Payload, Destination); strcat(Payload," HTTP/1.1"); strcat(Payload,"\r\n"); //POST
  strcat(Payload, "Host: "); strcat(Payload, Host); strcat(Payload, "\r\n"); //Host
  strcat(Payload, "Accept: */*"); strcat(Payload, "\r\n"); //Accept
  strcat(Payload, ContentLength); strcat(Payload, "\r\n"); //Content length
  strcat(Payload, "Content-Type: "); strcat(Payload, ContentType); strcat(Payload, "\r\n"); //Content type
  strcat(Payload, "User-Agent: "); strcat(Payload, UserAgent); strcat(Payload, "\r\n"); //User agent
  strcat(Payload, "Cache-Control: no-cache"); strcat(Payload, "\r\n"); //cache control
  strcat(Payload, "Connection: keep-alive"); strcat(Payload, "\r\n"); //keep alive
  strcat(Payload, Token); strcat(Payload, "\r\n"); //token
  strcat(Payload, "\r\n"); //required new line
  strcat(Payload, DataString); strcat(Payload, "\r\n\r\n");
}

void getPayloadCommand(){
  String PayloadS = Payload; 
  int PayloadLength = PayloadS.length();
  sprintf(PostCommand, "AT+QHTTPPOST=%d,120,120",PayloadLength);
}

void geturlCommand(){
  String urlS = url; 
  int urlLength = urlS.length();
  sprintf(urlCommand, "AT+QHTTPURL=%d,80",urlLength);
}

void setupPOST(){
  MakeDataString();
  MakePayload();
  geturlCommand();
  getPayloadCommand();
}
  
void PostData(){
  sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 15000);
  delay(5000);
  GetQCSQ();
  delay(100);
  GetQCSQ();
  delay(100);
  sendAT("ATE1", "\r\nOK", "\r\nERROR", 1000); 
  delay(100);
  sendAT("AT+CMEE=2", "\r\nOK", "\r\nERROR", 1000);  //Set error reporting
  delay(100);
  sendAT("AT+IFC=2,2", "\r\nOK", "\r\nERROR", 1000);   //set flow control
  
 //Wait for registration
  WaitReg();
  delay(100);
  sendAT("AT+CREG=2;+CGREG=2;+CEREG=2", "\r\nOK", "\r\nERROR", 1000); //registration status
  delay(100);
  sendAT("AT+CCLK?", "\r\nOK", "\r\nERROR", 2000);  //get the time
  delay(100);
  GetQCSQ();                                          //get cell signal
  sendAT("AT+QHTTPCFG=\"contextid\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(100);
  sendAT("AT+QHTTPCFG=\"responseheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(100);
  sendAT("AT+QHTTPCFG=\"requestheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(100);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 6000); //Setup GPRS communication
  delay(100);
  sendAT("AT+QICSGP=1,1,\"hologram\",\"\",\"\",1", "\r\nOK", "\r\nERROR", 5000); //Set the APN
  delay(100);
  sendAT("AT+QIACT=1", "\r\nOK", "\r\nERROR", 10000);
  delay(100);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 30000); //Setup GPRS communication
  delay(100);
  sendAT(urlCommand, "\r\nCONNECT", "\r\nERROR", 8000);
  SerialAT.print(url);
  Serial.println(url);
  sendAT("","\r\nOK","\r\nERROR", 30000);
  delay(100);
  sendAT(PostCommand, "\r\nCONNECT", "\r\nERROR", 60000);
  SerialAT.print(Payload);
  Serial.println(Payload);
  sendAT("","+QHTTPPOST","\r\nERROR", 30000);
  sendAT("AT+QHTTPREAD=60", "\r\n+QHTTPREAD: 0", "\r\nERROR", 10000);
  delay(100);
  sendAT("AT+QIDEACT=1", "\r\nOK", "\r\nERROR", 2000);
  delay(100);
}
