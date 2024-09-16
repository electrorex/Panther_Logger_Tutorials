/*Read Turner chlorophyll sensor at predetermined number of times with a predetermined delay between readings.
 * Fill an array with the results and calculate mean, median, min and max on the array
 * -T.R. Miller, Feb 26, 2024
 */

#include "Adafruit_MCP23X17.h"
Adafruit_MCP23X17 mcp;

//What analog pin to read sensor?
int ChlPin = A1;
int PhyPin = A0;
int NReads = 100;
int DelayReads = 100;

//How many readings to get on the analog pin?
const int N = 100;

//How long to delay between readings in milliseconds? NOTE: N x D = time it will take to make the measurement
const int D = 100;

//Variables needed. These will be available after running the anRead function.
float anAvg;
float anMedian;
float anMax;
float anMin;
float anSD;
float anER;
float anCV;
float GainVal;
float ChlAvg;
float ChlMedian;
float ChlMin;
float ChlMax;
float ChlSD;
float ChlER;
float ChlCV;
float ChlGain;

float PhyAvg;
float PhyMedian;
float PhyMin;
float PhyMax;
float PhySD;
float PhyER;
float PhyCV;
float PhyGain;

float mvolts; //millivolts read from any analog pin
//Function to read voltage on some analog pin and output in millivolts
float readVolts(int pin) {
  analogReadResolution(12);
  mvolts = analogRead(pin);
  mvolts *= 2;
  mvolts *= (3.3 / 4096);
  mvolts *= 1000;
  return mvolts;
}

//Function used by qsort for sorting array
int sort_desc(const void *cmp1, const void *cmp2){
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  return a > b ? -1 : (a < b ? 1 : 0);
}

//Function anRead to read an analog sensor multiple times (N), calculate average (anAvg), median (anMedian), min (anMin) and max (anMax)
//pin = analog pin for which the sensor is connected
//N = number of times to read the sensor
//D = delay in milliseconds between readings (D x N = time required to make measurement)
//After running this function the mean, median, min and max are available in anAvg, anMedian, anMin and anMax, respectively
void anRead(int pin, int N, int D) {
  float an[N];
  Serial.print("Reading sensor "); Serial.print(N); Serial.print(" times");
  for (int i = 0; i < N; ++i) {
    an[i] = readVolts(pin);
    delay(D);
    Serial.print(".");
  }
  
  //Remove first value 
  int N2 = N-1;
  float an2[N2];
  for (int i = 0; i < N2; ++i) {
    an2[i] = an[i+1];
  }

  //Remove last value
  int N3 = N2 - 1;
  float an3[N3];
  for (int i = 0; i < N3; ++i) {
    an3[i] = an2[i];
  }

  Serial.println("Array Received:");
  for (int i = 0; i < N3; ++i) {
    Serial.print(an3[i]);
    Serial.print(",");
  }
  Serial.println("");
  
  //Sort the array
  //int an_length = sizeof(an) / sizeof(an[0]);
  qsort(an2, N3, sizeof(an2[0]), sort_desc);

  //Calculate median
  int Center = N3/2;
  if ((N3 % 2) == 0){
    Center = Center - 1;
    anMedian = (an2[Center] + an2[Center+1])/2; 
  } else {
    anMedian = (an2[Center]);
  }

  //Get min and max from sorted array
  anMax = an2[0];
  anMin = an2[N3-1];

  //Calculate average
  for (int i = 0; i < N3; i++){
    anAvg += an2[i];
  }
  anAvg = anAvg/(N3-1);

  float Var;
  for (int i=0; i<N3; i++){
    Var += ((an3[i]-anAvg)*(an3[i]-anAvg));
  }
  
  anSD = sqrt(Var/N3);
  anER = anSD/sqrt(N3);
  anCV = anSD/anAvg*100;
}

//function to read the Turner 7F Phyorophyll fluorometer
void readPhy() {
  unsigned int PhyNowTime = millis();
  unsigned int PhyInterval = 35000; //Give sensor up to 35 seconds to get data otherwise move on
  while (PhyNowTime + PhyInterval > millis()){
    Serial.println("Starting measurements with the Turner phycocyanin fluorometer");
    mcp.pinMode(7, OUTPUT);
    mcp.digitalWrite(7, HIGH); //Turn on 12VS rail
    delay(100);

    //Set gain to 100X
    mcp.pinMode(0, INPUT); //To be floating for 100X gain
    mcp.pinMode(1, OUTPUT); //Grounded for 100X gain
    mcp.digitalWrite(1, LOW);
    GainVal = 0.01; //will multiply by 0.01 to normalize the reading by gain value of 100X
    delay(100);  //required after changing gains
    Serial.println("Setting sensor to 100X Gain");
    anRead(PhyPin,NReads,DelayReads);

    //auto gain
    if (anMedian < 4400) {
      PhyAvg = anAvg * GainVal;
      PhyMedian = anMedian * GainVal;
      PhyMin = anMin * GainVal;
      PhyMax = anMax * GainVal;
      PhySD = anSD * GainVal;
      PhyER = anER * GainVal;
      PhyCV = anCV * GainVal;
      PhyGain = GainVal;
      break;
    } else {
      //decrease gain to 10X
      Serial.println("Signal too high. Decreasing gain to 10X and will repeat measurements");
      Serial.println("");
      mcp.pinMode(0, OUTPUT);
      mcp.pinMode(1, INPUT);
      mcp.digitalWrite(0, LOW);
      delay(100);
      GainVal = 0.1;
      //get average
      anRead(PhyPin,NReads,DelayReads);
      PhyAvg = anAvg * GainVal;
      PhyMedian = anMedian * GainVal;
      PhyMin = anMin * GainVal;
      PhyMax = anMax * GainVal;
      PhySD = anSD * GainVal;
      PhyER = anER * GainVal;
      PhyCV = anCV * GainVal;
      PhyGain = GainVal;
    }

    if (anMedian > 4400) {
      //decrease gain to 1X
      Serial.println("Signal still too high. Decreasing gain value to 1X and will repeat measurements again");
      Serial.println("");
      GainVal = 1;
      mcp.pinMode(0, INPUT);
      mcp.pinMode(1, INPUT);
      delay(100);
      anRead(PhyPin,NReads,DelayReads);
      PhyAvg = anAvg * GainVal;
      PhyMedian = anMedian * GainVal;
      PhyMin = anMin * GainVal;
      PhyMax = anMax * GainVal;
      PhySD = anSD * GainVal;
      PhyER = anER * GainVal;
      PhyCV = anCV * GainVal;
      PhyGain = GainVal;
    }
    break;
  }
  mcp.pinMode(7, OUTPUT);
  mcp.digitalWrite(7, LOW); //Turn off 12VS rail
  delay(100);
}

//function to read the Turner 7F chlorophyll fluorometer
void readChl() {
  unsigned int ChlNowTime = millis();
  unsigned int ChlInterval = 35000; //Give sensor up to 35 seconds to get data otherwise move on
  while (ChlNowTime + ChlInterval > millis()){
    Serial.println("Starting measurements with the Turner chlorophyll fluorometer");
    mcp.pinMode(7, OUTPUT);
    mcp.digitalWrite(7, HIGH); //Turn on 12VS rail
    delay(100);

    //Set gain to 100X
    mcp.pinMode(0, INPUT); //To be floating for 100X gain
    mcp.pinMode(1, OUTPUT); //Grounded for 100X gain
    mcp.digitalWrite(1, LOW);
    GainVal = 0.01; //will multiply by 0.01 to normalize the reading by gain value of 100X
    delay(100);  //required after changing gains
    Serial.println("Setting sensor to 100X Gain");
    anRead(ChlPin,NReads,DelayReads);

    //auto gain
    if (anMedian < 4400) {
      ChlAvg = anAvg * GainVal;
      ChlMedian = anMedian * GainVal;
      ChlMin = anMin * GainVal;
      ChlMax = anMax * GainVal;
      ChlSD = anSD * GainVal;
      ChlER = anER * GainVal;
      ChlCV = anCV * GainVal;
      ChlGain = GainVal;
      break;
    } else {
      //decrease gain to 10X
      Serial.println("Signal too high. Decreasing gain to 10X and will repeat measurements");
      Serial.println("");
      mcp.pinMode(0, OUTPUT);
      mcp.pinMode(1, INPUT);
      mcp.digitalWrite(0, LOW);
      delay(100);
      GainVal = 0.1;
      //get average
      anRead(ChlPin,NReads,DelayReads);
      ChlAvg = anAvg * GainVal;
      ChlMedian = anMedian * GainVal;
      ChlMin = anMin * GainVal;
      ChlMax = anMax * GainVal;
      ChlSD = anSD * GainVal;
      ChlER = anER * GainVal;
      ChlCV = anCV * GainVal;
      ChlGain = GainVal;
    }

    if (anMedian > 4400) {
      //decrease gain to 1X
      Serial.println("Signal still too high. Decreasing gain value to 1X and will repeat measurements again");
      Serial.println("");
      GainVal = 1;
      mcp.pinMode(0, INPUT);
      mcp.pinMode(1, INPUT);
      delay(100);
      anRead(ChlPin,NReads,DelayReads);
      ChlAvg = anAvg * GainVal;
      ChlMedian = anMedian * GainVal;
      ChlMin = anMin * GainVal;
      ChlMax = anMax * GainVal;
      ChlSD = anSD * GainVal;
      ChlER = anER * GainVal;
      ChlCV = anCV * GainVal;
      ChlGain = GainVal;
    }
    break;
  }
  mcp.pinMode(7, OUTPUT);
  mcp.digitalWrite(7, LOW); //Turn off 12VS rail
  delay(100);
}

void setup() {
  Serial.begin(115200);
  while(!Serial); //Wait to get serial monitor up
  Serial.println("Setting up");
  Wire.begin();
  mcp.begin_I2C(0x20);
  mcp.pinMode(4, OUTPUT);
  mcp.digitalWrite(4, HIGH); //Turn on 3VS rail
  mcp.pinMode(7, OUTPUT);
  mcp.digitalWrite(7, HIGH); //Turn off 12VS rail
  Serial.println("Setup complete");
}

void loop() {
  readChl();
  readPhy();
  Serial.println("***************************************");
  Serial.print("Chl Gain = "); Serial.println(ChlGain);
  Serial.print("Chl Mean = "); Serial.println(ChlAvg);
  Serial.print("Chl Median = "); Serial.println(ChlMedian);
  Serial.print("Chl Minimum = "); Serial.println(ChlMin);
  Serial.print("Chl Maximum = "); Serial.println(ChlMax);
  Serial.print("Chl Standard Deviation = "); Serial.println(ChlSD);
  Serial.print("Chl Standard Error = "); Serial.println(ChlER);
  Serial.print("Chl Coefficient of Variation = "); Serial.println(ChlCV);
  Serial.println("----------------------------------------");
  Serial.print("Phy Gain = "); Serial.println(PhyGain);
  Serial.print("Phy Mean = "); Serial.println(PhyAvg);
  Serial.print("Phy Median = "); Serial.println(PhyMedian);
  Serial.print("Phy Minimum = "); Serial.println(PhyMin);
  Serial.print("Phy Maximum = "); Serial.println(PhyMax);
  Serial.print("Phy Standard Deviation = "); Serial.println(PhySD);
  Serial.print("Phy Standard Error = "); Serial.println(PhyER);
  Serial.print("Phy Coefficient of Variation = "); Serial.println(PhyCV);
  Serial.println("***************************************");
  Serial.println("");
  mcp.digitalWrite(7, LOW);
  delay(5000);
}
