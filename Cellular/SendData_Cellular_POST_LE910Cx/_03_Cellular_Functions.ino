char result_char[2000]; //container to read modem responses

int Check;

//Function to send AT commands to modem and listen for expected response, error code, or timeout after so many seconds
String sendAT(const char *toSend, const char *toCheck1, const char *toCheck2, unsigned long milliseconds) {
  String result;
  Serial.print(F("Sending: "));
  Serial.println(F(toSend));
  SerialAT.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    SerialAT.flush();
    Serial.flush();
    if (SerialAT.available()) {
      char c = SerialAT.read();
      Serial.write(c);
     
      result += c;  // append to the result string
      result.toCharArray(result_char, 1000);
      if (strstr(result_char, toCheck1) || strstr(result_char, toCheck2))
      {
        milliseconds = 0;
        Check = 1;
      }
      else {
        Check = 0; 
      }
    }
  }
  Serial.println();
  return result;
  Serial.print(result);
}

void ModemReset(){
 Serial.println("Resetting modem");
 pinMode(2,OUTPUT);
 digitalWrite(2,LOW);
 delay(1000);
 pinMode(2,INPUT);
 delay(10000);
 Serial.println("Modem is reset");
}

void ModemWakeup(){
 Serial.println("Waking up modem");
 pinMode(5,OUTPUT);
 digitalWrite(5,LOW);
 delay(600);
 pinMode(5,INPUT);
 delay(10000);
 Serial.println("Modem is awake!");
}


int CEREG; 
int CGREG; 
int CREG;

char* CREG1;
char* CREG2;
char* CREG3;

void GetCREG() { 
  SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  CREG1=strtok(result_char,":,");
  CREG2=strtok(NULL,",");
  CREG3=strtok(NULL,",");
  CREG = atoi(CREG3);
}

char* CGREG1;
char* CGREG2;
char* CGREG3;
void GetCGREG() { 
  SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CGREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  CGREG1=strtok(result_char,":,");
  CGREG2=strtok(NULL,",");
  CGREG3=strtok(NULL,",");
  CGREG = atoi(CGREG3);
}

char* CEREG1;
char* CEREG2;
char* CEREG3;
void GetCEREG() { 
SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CEREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  CEREG1=strtok(result_char,":,");
  CEREG2=strtok(NULL,",");
  CEREG3=strtok(NULL,",");
  CEREG = atoi(CEREG3);
}


void WaitReg(){
  unsigned long RegMillis = millis();
  unsigned long RegInterval = 1000*60*20;
  while (millis() - RegMillis < RegInterval){
    GetCREG(); 
    delay(100);
    GetCGREG();
    delay(100);
    GetCEREG();
    delay(1000);
    Serial.print("CREG = "); Serial.println(CREG);
    Serial.print("CGREG = "); Serial.println(CGREG);
    Serial.print("CEREG = "); Serial.println(CEREG);
    if(CREG==5 || CREG==1 || CGREG==5 || CGREG==1 || CEREG==5 || CEREG==1){
      RegInterval = 0;
    }
  }
}

void setupCell(){
 //ModemWakeup();
 //ModemReset();
 delay(10000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+CFUN=1,1", "\r\nOK", "\r\nERROR", 10000);
 delay(30000);
 sendAT("ATZ0", "\r\nOK", "\r\nERROR", 5000);  
 delay(1000);
 sendAT("ATE0", "\r\nOK", "\r\nERROR", 1000); 
 delay(1000);
 sendAT("AT+CMEE=0", "\r\nOK", "\r\nERROR", 1000); 
 delay(1000);
 sendAT("AT+IFC=2,2", "\r\nOK", "\r\nERROR", 1000);   
 delay(1000);
 sendAT("AT&W", "\r\nOK", "\r\nERROR", 1000); 
 delay(1000);
 sendAT("AT+COPS=0", "\r\nOK", "\r\nERROR", 5000);
 delay(1000);
// sendAT("AT+COPS=?", "\r\nOK", "\r\nERROR", 30000);
 delay(1000);
 WaitReg();
 delay(1000);
 sendAT("AT+CGDCONT=1,\"IP\",\"hologram\"", "\r\nOK", "\r\nERROR", 5000); 
 delay(1000);
 sendAT("AT#SCFG=1,1,300,90,600,50", "\r\nOK", "\r\nERROR", 1000); 
 delay(1000);
}

void sendData(){ 
 //sendAT("AT+CFUN=1,1", "\r\nOK", "\r\nERROR", 10000);
 //delay(10000);
 //sendAT("AT+CGDCONT=1,\"IP\",\"hologram\"", "\r\nOK", "\r\nERROR", 5000); 
 //delay(1000);
 //sendAT("AT#SCFG=1,1,300,90,600,50", "\r\nOK", "\r\nERROR", 1000); 
 //delay(1000);
 sendAT("AT#SGACT=1,1", "\r\nOK", "\r\nERROR", 2000);
 delay(5000);
 sendAT("AT#HTTPCFG=1,\"industrial.api.ubidots.com\",80,1", "\r\nOK", "\r\nERROR", 60000);
 delay(5000);

 char Send[1000];
 strcpy(Send, "AT#HTTPSND=1,0,\"/api/v1.6/devices/telelogger2\",170,\"X-Auth-Token: BBUS-GqdJRJRyZz9yrWLH94R\"");
 //strcat(Send, Header);
   
 sendAT(Send, "\r\n>>>", "\r\nERROR", 20000);
 delay(5000);
 sendAT(DataString,"\r\nOK", "\r\nERROR",45000);
}
