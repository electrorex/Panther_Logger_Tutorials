

//Function to make the data string to post. Update as necessary
void makeDataString(){ 
  sprintf(DataString, "{\"temperature\": %.2f,\"battery\": %.2f}\r\n\r\n",                                   
          AirTemp,
          Batv                                                                                      
          );
}

char Header[1000];
void makeHeader(){
  String DataStringS = DataString;
  sprintf(ContentLength,"Content-Length: %d",DataStringS.length());
  //Make header, end each line with new line carraige return \r\n
  strcpy(Header,"POST "); strcat(Header, Destination); strcat(Header," HTTP/1.1"); strcat(Header,"\r\n"); //POST
  strcat(Header, "Host: "); strcat(Header, Host); strcat(Header, "\r\n"); //Host
  strcat(Header, "Accept: */*"); strcat(Header, "\r\n"); //Accept
  strcat(Header, ContentLength); strcat(Header, "\r\n"); //Content length
  strcat(Header, "Content-Type: "); strcat(Header, ContentType); strcat(Header, "\r\n"); //Content type
  strcat(Header, "User-Agent: "); strcat(Header, UserAgent); strcat(Header, "\r\n"); //User agent
  strcat(Header, "Cache-Control: no-cache"); strcat(Header, "\r\n"); //cache control
  strcat(Header, "Connection: close"); strcat(Header, "\r\n"); //close connection
  strcat(Header, Token); strcat(Header, "\r\n"); //token
}
