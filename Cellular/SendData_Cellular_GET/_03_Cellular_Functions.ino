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
 delay(3000);
 pinMode(2,INPUT);
 sendAT("", "\r\nAPP RDY", "\r\nAPP RDY", 8000);
 Serial.println("Modem is reset");
}

void ModemWakeup(){
 Serial.println("Waking up modem");
 pinMode(5,OUTPUT);
 digitalWrite(5,LOW);
 delay(1000);
 pinMode(5,INPUT);
 delay(1000);
 sendAT("", "\r\nAPP RDY", "\r\nAPP RDY", 8000);
 Serial.println("Modem is awake!");
}


int CEREG; 
int CGREG; 
int CREG;

void GetCREG() { 
  SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":,");
  strtok(NULL,",");
  CREG = atoi(strtok(NULL,","));
}

void GetCGREG() { 
  SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CGREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":,");
  strtok(NULL,",");
  CGREG = atoi(strtok(NULL,","));
}

void GetCEREG() { 
SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CEREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":,");
  strtok(NULL,",");
  CEREG = atoi(strtok(NULL,","));
}

char* QCSQ1;
char* QCSQ2;
char* QCSQ3;
char* QCSQ4;
char* QCSQ5;
char* QCSQ6;
char* Network;
int RSRP;
int RSRQ;
void GetQCSQ() { 
  delay(1000); 
  SerialAT.println("");
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT+QCSQ", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":");
  Network=strtok(NULL,",");
  strtok(NULL,",");
  RSRP=atoi(strtok(NULL,","));
  strtok(NULL,",");
  RSRQ=atoi(strtok(NULL,","));
}

void WaitReg(int waittime){
  unsigned long RegMillis = millis();
  unsigned long RegInterval = waittime;
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

void SetupCell(){ 
 ModemReset();
 ModemWakeup();
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 10000);
 delay(1000);
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
 sendAT("AT+QCFG=\"nwscanseq\"", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+COPS=0", "\r\nOK", "\r\nERROR", 5000);
 delay(1000);
 sendAT("AT+CRSM=214,28539,0,0,12,\"130062FFFFFFFFFFFFFFFFFF\"","\r\nOK", "\r\nERROR", 60000);
 delay(1000);
 sendAT("AT+CTZU=3", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+CTZU?", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+QICSGP=1,1,\"hologram\",\"\",\"\",1", "\r\nOK", "\r\nERROR", 5000); 
 delay(1000);
 WaitReg(60*1000*30);
 delay(1000);
 sendAT("AT+CREG=2;+CGREG=2;+CEREG=2", "\r\nOK", "\r\nERROR", 1000); 
 delay(1000);
 sendAT("AT+CCLK?", "\r\nOK", "\r\nERROR", 2000);
}
