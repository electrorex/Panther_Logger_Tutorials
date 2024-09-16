//Variables to hold sensor data
float Batv;
float mvolts;
float AirTemp;   
float Humidity;
float BaroPressure;
float WindSpeed;
int WindDirection;                        //this is an int so field 5 is formatted as %d
float DewPoint;
float WindChill;

float readVolts(int pin) {                                                                          //Make a function to read voltage on some analog pin and output in millivolts
  analogReadResolution(12);                                                                         //Set analog read resolution to 12 (4096 steps)
  mvolts = analogRead(pin); 
  mvolts *= 2;
  mvolts *= (3.3 / 4096);
  mvolts *= 1000;
  return mvolts;
}
                                                                                                    //Function to read Battery voltage on pin A4 on Telelogger
void readBat() {
  Batv = readVolts(A4)/1000;                                                                        //Read battery on A4 and convert millivolts to volts
}


//Function to read sensors or sensor functions. Update as necessary
void readSensors(){
  AirTemp = random(0,30);
  Humidity = random(0,100);
  BaroPressure = random(900,1200);
  WindSpeed = random(0,50);
  WindDirection = random(0,360);
  DewPoint = random(0,30);
  WindChill = random(0,30);
  readBat();
}
