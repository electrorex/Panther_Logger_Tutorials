//Variables to hold sensor data
float Batv;
float mvolts;
float AirTemp;   
float Humidity;
float BaroPressure;
float WindSpeed;
int WindDirection;                        
float DewPoint;
float WindChill;

//Function to read sensors or sensor functions. Update as necessary
void readSensors(){
  AirTemp = ptr.pTemp();
  Humidity = ptr.pHumid();
  BaroPressure = random(900,1200);
  WindSpeed = random(0,50);
  WindDirection = random(0,360);
  DewPoint = random(0,30);
  WindChill = random(0,30);
  Batv = ptr.bat();
}
