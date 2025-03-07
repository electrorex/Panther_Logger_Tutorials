/*
This example code will read information from the Quectel BG96 modem on the 
Electrorex Panther Logger, including geolocation information.
*/

#include <Panther.h>

Panther ptr;

//The Panther Logger communicates with the cell modem on Serial 1
#define SerialAT Serial1 

//Variables needed to hold information from the modem
char result_char[2000];
int RSI;
int RegStatus;
char* CSQ1;
char* CSQ2;
int Year;
int Month;
int Day;
int Hour;
int Minute;
char Sec[10];
int Second;
char CCLK[100];
char Date[10];
char Time[10];

char* QCSQ1;
char* QCSQ2;
char* QCSQ3;
char* QCSQ4;
char* QCSQ5;
char* QCSQ6;
char* Network;
int RSRP;
int RSRQ;

char* HDOP1;
float HDOP;

char* GEO1;
float GEO;

char* UTC1;
float UTC;

char* Lat1;
float Lat;
float LatF;

char* Lon1;
float Lon;
float LonF;

char* Hdop1;
float Hdop;

char* Alt1;
float Alt;

char* Fix1;
int Fix;

char* Sat1;
int Sat;

float COG;

//Function to send commands to the modem and wait to recieve expected responses
int Check;
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

//Convert latitude location information from modem to a floating point number
float convertLat(float x){
  int Degrees = ((int)x)/100;
  float Mins = x - (float)(Degrees*100);
  float CoordOut = (float)(Degrees + Mins/60.0);
  return(CoordOut);
}

//Convert longitude location information from modem to a floating point number
float convertLon(float x){
  int Degrees = ((int)x)/100;
  float Mins = x - (float)(Degrees*100);
  float CoordOut = (float)(Degrees + Mins/60.0) *-1;
  return(CoordOut);
}

//Function to reset the modem on nRESET pin 5
void ModemReset(){
 Serial.println("Resetting modem");
 pinMode(2,OUTPUT);
 digitalWrite(2,LOW);
 delay(1000);
 pinMode(2,INPUT);
 Serial.println("Modem is reset");
}

//Function to turn off modem and back on again on its on/off pin 20
//This should be used sparingly!
void ModemWakeup(){
 Serial.println("Waking up modem");
 pinMode(5,OUTPUT);
 digitalWrite(5,LOW);
 delay(600);
 pinMode(5,INPUT);
 Serial.println("Modem is awake!");
 delay(2000);
}

//Perform functions to setup the cell modem and provision onto the network for the first time
//This allows some time for the modem to provision, search the area for available networks and ensure good signal strength
//One should watch this process carefully in new remote locations to gauge whether successful communications will occur
void SetupCell(){ 
 ModemWakeup();
 delay(100);
 ModemReset();
 delay(100);
 sendAT("AT", "\r\nOK", "\r\nERROR", 2000);
 delay(100);
 sendAT("AT", "\r\nOK", "\r\nERROR", 2000);
 
 Check = 0;
 sendAT("AT", "\r\nOK", "\r\nERROR", 2000);
 if(Check = 0){
  ModemReset(); 
  }  

 delay(100);
 sendAT("AT+CFUN=1,1", "\r\nAPP RDY", "\r\nERROR", 10000); //restart modem to full functionality
 
 delay(100);
 sendAT("AT+CMEE=0", "\r\nOK", "\r\nERROR", 10000); 

 delay(100);
 sendAT("AT+IFC=2,2", "\r\nOK", "\r\nERROR", 10000);  
 
 delay(100);
 sendAT("AT&W", "\r\nOK", "\r\nERROR", 10000); 
 
 delay(100);
 sendAT("AT+QCFG=\"nwscanseq\"", "\r\nOK", "\r\nERROR", 10000);
  
 delay(100);
 sendAT("AT+COPS=0", "\r\nOK", "\r\nERROR", 10000);

 delay(100);
 //This command will force the modem to search for available cell providers in the area
 //This could take several minutes.
 sendAT("AT+COPS=?", "\r\nOK", "\r\nERROR", 60000*5);

 delay(100);
 //This command sets the modem to automatically join the network.
 sendAT("AT+COPS=0","\r\nOK", "\r\nERROR", 60000);

 //Uncomment this particular sendAT command to block a certain carrier (i.e. add it to the FPLMN list). 
 //For example, some carriers may not provide adequate support for NB-IoT/CatM1 modems (like T-Mobile)
 //Need to run pass through script first and run AT+COPS=? to find carrier MCC and MNC
 //Follow AT command notes for BG96 to translate to proper PLMN for that carrier and insert into X's below 
 //See further notes at electrorex.io learing center
 //delay(1000);
 //Put T-Mobile on FPLMN list
 //sendAT("AT+CRSM=214,28539,0,0,12,\"130062FFFFFFFFFFFFFFFFFF\"","\r\nOK", "\r\nERROR", 60000);

 delay(100);
 sendAT("AT+CTZU=3", "\r\nOK", "\r\nERROR", 10000);
 
 delay(100);
 SerialAT.println("");
 SerialAT.println("");
 sendAT("AT+CTZU?", "\r\nOK", "\r\nERROR", 10000);

 
 delay(100);
 SerialAT.println("");
 SerialAT.println("");
 sendAT("AT+CREG=2;+CGREG=2;+CEREG=2", "\r\nOK", "\r\nERROR", 10000); 
 
 delay(100);
 SerialAT.println("");
 SerialAT.println("");
 //Get the time. If the time is correct then the modem has successfully joined the network
 sendAT("AT+CCLK?", "\r\nOK", "\r\nERROR", 60000);
 
}

void getGPS() {
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT", "\r\nOK", "\r\nERROR", 3000);
  delay(100);
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT+QCFG=\"gpio\",1,64,1,0,0,1", "\r\nOK", "\r\nERROR", 3000);
  delay(100);
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT+QCFG=\"gpio\",3,64,1,1", "\r\nOK", "\r\nERROR", 3000);
  delay(100);
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT+QCFG=\"gpio\",2,64", "\r\nOK", "\r\nERROR", 3000);
  delay(100);
  SerialAT.println("");
  SerialAT.println("");
  sendAT("AT+QGPS=1", "\r\nOK", "\r\nERROR", 5000);
  delay(100);
  SerialAT.println("");
  SerialAT.println("");
  //GPS data will be provided in NMEA format.
  sendAT("AT+QGPSCFG=\"nmeasrc\",1", "\r\nOK", "\r\nERROR", 5000);
  delay(100);

  unsigned int GPSNowTime = millis();
  unsigned int GPSInterval = 5000; //Give sensor 10 seconds to send good data
  while(GPSNowTime + GPSInterval > millis()){
    SerialAT.println("");
    SerialAT.println("");
    //request GPS data for NMEA code GGA 
    sendAT("AT+QGPSGNMEA=\"GGA\"", "\r\nOK", "\r\n+CME ERROR", 5000);
    delay(100);
  }

  //Parse the GPS data, which is comma separated.
  const char* strtokIndx;
  strtokIndx = strtok(result_char, ":");
  Serial.print("strtokIndx = "); Serial.println(strtokIndx);
  strtok(NULL, ",");
  UTC1 = strtok(NULL, ",");
  UTC = atof(UTC1);
  Serial.print("UTC = "); Serial.println(UTC);
  
  Lat1 = strtok(NULL, ",");
  Lat = atof(Lat1);
  LatF = convertLat(Lat);
  Serial.print("Lat = "); Serial.println(LatF);
  strtok(NULL, ",");

  Lon1 = strtok(NULL, ",");
  Lon = atof(Lon1);
  LonF = convertLon(Lon);
  Serial.print("Lon = "); Serial.println(LonF);
  strtok(NULL, ",");
  
  Fix1 = strtok(NULL, ",");
  Fix = atoi(Fix1);
  Serial.print("Fix = "); Serial.println(Fix);

  Sat1 = strtok(NULL, ",");
  Sat = atoi(Sat1);
  Serial.print("Sat = "); Serial.println(Sat);

  Hdop1 = strtok(NULL, ",");
  Hdop = atof(Hdop1);
  Serial.print("HDOP = "); Serial.println(Hdop);

  Alt1 = strtok(NULL, ",");
  Alt = atof(Alt1);
  Serial.print("Alt = "); Serial.println(Alt);
  
  strtok(NULL, ",");
  
  GEO1 = strtok(NULL, ",");
  GEO = atof(GEO1);
  Serial.print("GEO = "); Serial.println(GEO);
}

void setup() {
  delay(5000); //wait a bit to get serial monitor up
  Serial.begin(115200);
  SerialAT.begin(115200);
  Serial.println("Setting things up. Please wait");
  ptr.begin();
  ptr.set3v3(HIGH);
  ptr.set12v(LOW);
  ptr.LED(2,LOW);
  ptr.LED(3,LOW);
  ptr.LED(4,LOW);
}

void loop() {
  getGPS();
  delay(5000); //Wait 5 seconds before requesting new data
}
