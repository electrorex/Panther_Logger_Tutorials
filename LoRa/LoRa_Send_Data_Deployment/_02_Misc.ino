float mvolts; //millivolts read from any analog pin
float Batv;
float BatCut = 3.3;

//Function to read voltage on some analog pin and output in millivolts
float readVolts(int pin) {
  analogReadResolution(12);
  mvolts = analogRead(pin);
  mvolts *= 2;
  mvolts *= (3.3 / 4096);
  mvolts *= 1000;
  return mvolts;
}

//Function to read Battery voltage
void readBat() {
  Batv = readVolts(A4)/1000;
}

void SystemCheck(){
  readBat();
  while(Batv < BatCut){
    mcp.digitalWrite(4, LOW); //Turn off 3VS rail
    mcp.digitalWrite(7, LOW); //Turn off 12VS rail
    Watchdog.sleep(4000);
    readBat();
  }
}

void setTimeInitial(){
  rtc.setYear(Yeari);
  rtc.setMonth(Monthi);
  rtc.setDay(Dayi);
  rtc.setHours(Houri);
  rtc.setMinutes(Minutei);
  rtc.setSeconds(Secondi);
}

//int SleepIntervals  = (SleepTime*60*1000)/6000;
void GoToSleep(){
  SerialAT.end();
  for (int i = 0; i < SleepIntervals; ++i) {
      Watchdog.sleep(6000);
  }
}
