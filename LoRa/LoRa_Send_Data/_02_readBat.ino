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

