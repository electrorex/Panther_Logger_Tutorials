//Variables to hold sensor data
float Batv;
float AirTemp;   

//Function to read sensors or sensor functions. Update as necessary
void readSensors(){
  AirTemp = ptr.pTemp();
  Batv = ptr.bat();
}
