//Function to make the data string to post. Update as necessary
void MakeDataString(){ 
  sprintf(DataString, "{\"temperature\": %.2f,\"battery\": %.2f}\r\n\r\n",                                   
          AirTemp,
          Batv                                                                                      
          );
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
  MakeDataString();
  MakePayload();
  geturlCommand();
  getPayloadCommand();
}
  
void PostData(){
  sendAT(urlCommand, "\r\nCONNECT", "\r\nERROR", 8000);
  SerialAT.print(url);
  Serial.println(url);
  delay(1000);
  sendAT("","\r\nOK","\r\nERROR", 30000);
  delay(1000);
  sendAT(PostCommand, "\r\nCONNECT", "\r\nERROR", 8000);
  SerialAT.print(Payload);
  Serial.println(Payload);
  sendAT("", "\r\nOK", "\r\nERROR", 5000);
  delay(6000);
  sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
  delay(1000);
  sendAT("AT+QHTTPREAD=60", "+QHTTPREAD: 0", "\r\nERROR", 10000);
  delay(1000);
  //sendAT("AT+QIDEACT=1", "\r\nOK", "\r\nERROR", 2000);
  delay(1000);
}
