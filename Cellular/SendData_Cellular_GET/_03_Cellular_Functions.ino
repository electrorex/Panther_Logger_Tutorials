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


//In the functions below we parse the comma separated strings given by the modem
//We store them all in individual strings, although we don't have to store them all

//Registration status is given as an integer.
int CEREG; //EPS, 3G and LTE registration
int CGREG; //GPRS, mainly 2G registration
int CREG; //CSD, mainly for 2G and 3G registration

//Get CSD registration status
//The modem returns the following... +CREG: 2,5,"4301","4A00F09",8
// 2 = means automatic registration enabled
// 5 = registered and roaming. Hologram and soracom will always be roaming.
// "4301" = location area code in hexadecimal format
// "4A00F09" = Four-byte GERAN/E-UTRAN cell ID in hexadecimal format
//Below we use the strtok function to split the modem response by colon and commas into tokens (string chunks)
void GetCREG() { 
  sendAT("AT+CREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":,"); //split response by colon and comma
  strtok(NULL,","); //Move to next chunk, we won't save the first one
  CREG = atoi(strtok(NULL,",")); //This is the registration status so we put it into the CREG char
}

//Get GPRS registration status
void GetCGREG() { 
  sendAT("AT+CGREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":,");
  strtok(NULL,",");
  CGREG = atoi(strtok(NULL,","));
}

//Get EPS (LTE) registration status
void GetCEREG() { 
  sendAT("AT+CEREG?", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":,");
  strtok(NULL,",");
  CEREG = atoi(strtok(NULL,","));
}

//Get cellular LTE signal strength and quality
char* Network;
int RSRP;
int RSRQ;
void GetQCSQ() { 
  sendAT("AT+QCSQ", "\r\nOK", "\r\nERROR", 60000);
  delay(1000);
  strtok(result_char,":");
  Network=strtok(NULL,",");
  strtok(NULL,",");
  RSRP = atoi(strtok(NULL,","));
  strtok(NULL,",");
  RSRQ = atoi(strtok(NULL,","));
}

//Function to wait for the modem to get registered
//This will run at the beginning of a deployment
//We will give it 20 minutes to get registered
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
    //If we get registered then exit the while loop
    if(CREG==5 || CREG==1 || CGREG==5 || CGREG==1 || CEREG==5 || CEREG==1){
      RegInterval = 0;
    }
  }
}

//Major function to setup the modem for deployment. Runs in the setup function
void SetupCell(){ 
 ModemWakeup();
 ModemReset();
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 20000);
 delay(1000);
 sendAT("ATE", "\r\nOK", "\r\nERROR", 1000); 
 delay(1000);
 sendAT("AT+QCFG=\"nwscanseq\"", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+CREG?;+CGREG?;+CEREG?", "\r\nOK", "\r\nERROR", 1000);
 delay(1000);
 sendAT("AT+CREG=2;+CGREG=2;+CEREG=2", "\r\nOK", "\r\nERROR", 1000); //Set to enable network registration and location information
 delay(1000); 
 sendAT("AT+COPS=?", "\r\nOK", "\r\nERROR", 1200000); //Search for cell networks in the area
 delay(1000);
 sendAT("AT+COPS=0", "\r\nOK", "\r\nERROR", 5000); //Automatically select any available network
 delay(1000);
 //Uncomment the below AT command to block T-Mobile. 
 //sendAT("AT+CRSM=214,28539,0,0,12,\"130062FFFFFFFFFFFFFFFFFF\"","\r\nOK", "\r\nERROR", 60000);
 delay(1000);
 sendAT("AT+CTZU=3", "\r\nOK", "\r\nERROR", 1000); //Set timezone
 delay(1000);
 char APNAT[200];
 sprintf(APNAT,"AT+QICSGP=1,1,\"%s\",\"\",\"\",1",APN);
 sendAT(APNAT, "\r\nOK", "\r\nERROR", 5000); //Set the APN of our provider
 delay(1000);
 WaitReg(); //Wait for the modem to register onto the network

 delay(1000);
 sendAT("AT+CCLK?", "\r\nOK", "\r\nERROR", 2000); //Request the time
}
