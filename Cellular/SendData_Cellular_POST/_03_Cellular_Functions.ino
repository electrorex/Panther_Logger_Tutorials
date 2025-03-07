//Function to communicate AT commands to the modem and wait for expected responses
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
 sendAT("", "\r\nAPP RDY", "\r\nAPP RDY", 8000);
 Serial.println("Modem is reset");
}

void ModemWakeup(){
 Serial.println("Waking up modem");
 pinMode(5,OUTPUT);
 digitalWrite(5,LOW);
 delay(600);
 pinMode(5,INPUT);
 sendAT("", "\r\nAPP RDY", "\r\nAPP RDY", 8000);
 Serial.println("Modem is awake!");
}

int CEREG; 
int CGREG; 
int CREG;
//Get GSM registration status
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

//Get GPRS registration status
void GetCGREG() { 
  SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CGREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(100);
  strtok(result_char,":,");
  strtok(NULL,",");
  CGREG = atoi(strtok(NULL,","));
}

//Get registration status for LTE and NB-IoT
void GetCEREG() { 
  SerialAT.println("");
  delay(100);
  SerialAT.println("");
  delay(100);
  sendAT("AT+CEREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(100);
  strtok(result_char,":,");
  strtok(NULL,",");
  CEREG = atoi(strtok(NULL,","));
}

char* Network;
int RSRP;
int RSRQ;

void GetQCSQ() { 
  delay(100); 
  SerialAT.println("");
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT+QCSQ", "\r\nOK", "\r\nERROR", 60000);
  delay(100);
  strtok(result_char,":");
  Network=strtok(NULL,",");
  strtok(NULL,",");
  RSRP=atoi(strtok(NULL,","));
  strtok(NULL,",");
  RSRQ=atoi(strtok(NULL,","));
  int RSRP_Excellent = -90;
  int RSRP_Good = -105;
  int RSRP_Fair = -106;
  int RSRP_Poor = -120;

  int RSRQ_Excellent = -9;
  int RSRQ_Good = -12;
  int RSRQ_Fair = -13;

  Serial.print("RSRP = "); Serial.print(RSRP); Serial.print(", ");
    if (RSRP >= RSRP_Excellent){
      Serial.println("Excellent");
    } 
    else if (RSRP < RSRP_Excellent & RSRP >= RSRP_Good){
      Serial.println("Good");
    }
    else if (RSRP < RSRP_Fair & RSRP >= RSRP_Poor){
      Serial.println("Fair");
    }
    else if (RSRP < RSRP_Poor){
      Serial.println("Poor");
    }
    
  Serial.print("RSRQ = "); Serial.print(RSRQ); Serial.print(", ");
    if (RSRQ > RSRQ_Excellent){
      Serial.println("Excellent");
    } 
    else if (RSRQ < RSRQ_Excellent & RSRQ >= RSRQ_Good){
      Serial.println("Good");
    }
    else if (RSRQ < RSRQ_Fair){
      Serial.println("Fair");
    }
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
    delay(100);
    Serial.print("CREG = "); Serial.println(CREG);
    Serial.print("CGREG = "); Serial.println(CGREG);
    Serial.print("CEREG = "); Serial.println(CEREG);
    if(CREG==5 || CREG==1 || CGREG==5 || CGREG==1 || CEREG==5 || CEREG==1){
      RegInterval = 0;
    }
  }
}

void SetupCell(){ 
  //Startup modem with a wakeup and reset (do this only once at startup)
  ModemWakeup();
  ModemReset();
  sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
  delay(100);
  sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
  delay(100);
  sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
  delay(100);
  sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 10000);
  delay(100);

 //Get signal strength and quality a couple of times
  GetQCSQ();
  delay(500);
  GetQCSQ();
  delay(100);
  sendAT("ATE1", "\r\nOK", "\r\nERROR", 1000); 
  delay(100);
  sendAT("AT+CMEE=2", "\r\nOK", "\r\nERROR", 1000);  //Set error reporting
  delay(100);
  sendAT("AT+IFC=2,2", "\r\nOK", "\r\nERROR", 1000);   //set flow control
  delay(100);
  sendAT("AT+QCFG=\"nwscanseq\",020301", "\r\nOK", "\r\nERROR", 1000);  //Scan for CatM1 networks first
  delay(100);
  sendAT("AT+COPS=0", "\r\nOK", "\r\nERROR", 5000);  //Automatically register
  delay(100);
  sendAT("AT+COPS=?", "\r\nOK", "\r\nERROR", 120000); //Scan for networks
  delay(100);
 
 //Uncomment to prevent T-mobile connection
 //sendAT("AT+CRSM=214,28539,0,0,12,\"130062FFFFFFFFFFFFFFFFFF\"","\r\nOK", "\r\nERROR", 60000);
  delay(100);
  sendAT("AT+CTZU=3", "\r\nOK", "\r\nERROR", 1000);  //Set time zone to local time
  delay(100);
  sendAT("AT+CTZU?", "\r\nOK", "\r\nERROR", 1000);
  delay(100);
  sendAT("AT+QICSGP=1,1,\"hologram\",\"\",\"\",1", "\r\nOK", "\r\nERROR", 5000); //Set the APN
  delay(100);

 //Wait for registration
  WaitReg();
  delay(100);
  sendAT("AT+CREG=2;+CGREG=2;+CEREG=2", "\r\nOK", "\r\nERROR", 1000); //registration status
  delay(100);
  sendAT("AT+CCLK?", "\r\nOK", "\r\nERROR", 2000);  //get the time
  delay(100);
  GetQCSQ();              //get cell signal
  
  //Configure HTTP service                                      
  sendAT("AT+QHTTPCFG=\"contextid\",1", "\r\nOK", "\r\nERROR", 5000); 
  delay(100);
  sendAT("AT+QHTTPCFG=\"responseheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(100);
  sendAT("AT+QHTTPCFG=\"requestheader\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(100);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 6000); //Setup GPRS communication, start PDP context (Packet Data Protocol)
  delay(100);
  sendAT("AT+QIACT=1", "\r\nOK", "\r\nERROR", 10000);
  delay(100);
  sendAT("AT+QIACT?", "\r\nOK", "\r\nERROR", 30000); //If connected to network this will get our IP address
  delay(100);
}

