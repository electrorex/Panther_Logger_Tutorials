void printResults(){
  debugln(F(""));
  debugln(F("*********************************************************"));
  debug(F("System Time: "));   
  debug(F("20")); debug(rtc.getYear()); debug(F("-"));
  debug(rtc.getMonth()); debug(F("-"));
  debug(rtc.getDay()); debug(F(" "));
  debug(rtc.getHours()); debug(F(":"));
  debug(rtc.getMinutes()); debug(F(":"));
  debug(rtc.getSeconds()); debugln();
  debug("Epoch time = "); debugln(rtc.getEpoch());
  debug(F("Battery Voltage (V) = ")); debugln(Batv);
  debugln(F("*********************************************************"));
  debugln(F(""));
}
