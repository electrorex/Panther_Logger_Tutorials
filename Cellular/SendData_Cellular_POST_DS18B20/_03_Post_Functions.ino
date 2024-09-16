//Function to make the data string to post. Update as necessary
void MakeDataString(){ 
  sprintf(DataString, "{\"temperature\": %.2f,\"battery\": %.2f}",                                   
          Temps[0],
          Batv                                                                                      
          );
}

void MakePayload(){
  String DataStringS = DataString;
  sprintf(ContentLength,"Content-Length: %d",DataStringS.length());
  //DataStringS.length()
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
  strcat(Payload, DataString); strcat(Payload, "\r\n");
}

void getPayloadCommand(){
  String PayloadS = Payload; 
  int PayloadLength = PayloadS.length();
  sprintf(PostCommand, "AT+QHTTPPOST=%d,80,80",PayloadLength);
}

void geturlCommand(){
  String urlS = url; 
  int urlLength = urlS.length();
  sprintf(urlCommand, "AT+QHTTPURL=%d,80",urlLength);
}

void setupPOST(){
  Serial.println("Making data string");
  MakeDataString();
  Serial.println("Making payload");
  MakePayload();
  Serial.println("Getting URL command");
  geturlCommand();
  getPayloadCommand();
}

char QICSGP[100];
char APN[100];
  
void PostData(){
  sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 10000);
  delay(1000);
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
  sendAT("AT+QICSGP=1,1,\"soracom.io\",\"\",\"\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(1000);
  sendAT("AT+QIACT=1", "\r\nOK", "\r\nERROR", 10000);
  delay(1000);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 30000); //Setup GPRS communication
  delay(1000);

  WaitReg();
  
  sendAT(urlCommand, "\r\nCONNECT", "\r\nERROR", 8000);
  SerialAT.print(url);
  Serial.println(url);
  delay(1000);
  sendAT("","\r\nOK","\r\nERROR", 30000);
  delay(1000);
  sendAT(PostCommand, "\r\nCONNECT", "\r\nERROR", 40000);
  SerialAT.print(Payload);
  Serial.println(Payload);
  sendAT("", "\r\nOK", "\r\nERROR", 5000);
  delay(10000);
  sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
  delay(1000);
  sendAT("AT+QHTTPREAD=60", "+QHTTPREAD: 0", "\r\nERROR", 10000);
  delay(1000);
  sendAT("AT+QIDEACT=1", "\r\nOK", "\r\nERROR", 2000);
  delay(1000);
}
