//Convert data expecting up to four integer numbers (before decimal) and two numbers after decimal.
//Does not handle negative values
char HexOutFour[10];
char* hexFour(float var){ //hexHigh
    char FloatChar[10];
    dtostrf(var/100,4,4,FloatChar);  
    float VarConvert = atof(FloatChar);
    int Var1 = var/100; 
    int Var2 = (VarConvert - Var1)*100;
    int Var3 = (((VarConvert - Var1)*100)-Var2)*100;
    char HexVar[10];
    sprintf(HexVar, "%02hX%02hX%02hX",
              Var1,
              Var2,
              Var3
            );
    strcpy(HexOutFour,HexVar); 
    return(HexOutFour);
}

//Convert data expecting up to three integer numbers (before decimal) and two after decimal
//Negative values and values above "flag" are sent as the flagval
char HexOutThree[10];
char* hexThree(float var, int flag, float flagval){
      if(var<0 || var > flag){
        var = flagval;
        char HexVar[10];
        char FloatChar[10];
        dtostrf(var,3,2,FloatChar);  
        float VarConvert = atof(FloatChar);
        int16_t Var1 = VarConvert; 
        int16_t Var2 = (VarConvert - Var1)*100; 
        sprintf(HexVar, "%02hX%02hX",
              Var1,
              Var2
            );
        strcpy(HexOutThree,HexVar);  
        return(HexOutThree);
      } else {
      char HexVar[10];
      char FloatChar[10];
      dtostrf(var,3,2,FloatChar);  
      float VarConvert = atof(FloatChar);
      int16_t Var1 = VarConvert; 
      int16_t Var2 = (VarConvert - Var1)*100; 
      sprintf(HexVar, "%02hX%02hX",
              Var1,
              Var2
            );
      strcpy(HexOutThree,HexVar);  
      return(HexOutThree);
      }
}

//Convert data expecting up to three integer numbers (before decimal) and two after decimal
//This handles neagtive values with the addition of four more hexadecimals required, so use only on variables that are expected to be negative
char HexOutThreeNeg[10];
char* hexConvert1(float var){
      char HexVar[10];
      char FloatChar[10];
      dtostrf(var,3,2,FloatChar);  //Set length with 2 decimal places and first place value as whatever comes with var, which is assumed to be two digits
      float VarConvert = atof(FloatChar);
      int16_t Var1 = VarConvert; 
      int16_t Var2 = (VarConvert - Var1)*100; 
      //if(var<0){
        sprintf(HexVar, "%04hX%04hX",
              Var1,
              Var2
            );
       strcpy(HexOutThreeNeg,HexVar);  
       return(HexOutThreeNeg);
}

char DateTimeHex[400];
void getHexTime(){
  sprintf(DateTimeHex, "%02hX%02hX%02hX%02hX%02hX%02hX",
          rtc.getYear(),
          rtc.getMonth(),
          rtc.getDay(),
          rtc.getHours(),
          rtc.getMinutes(),
          rtc.getSeconds()
          );
}

char Payload[4000];
void getPayload(){
  strcpy(Payload,"AT+SEND=1:");

  hexThree(Batv,5,99);
  strcat(Payload,HexOutThree);

  hexConvert1(Temps[0]);
  strcat(Payload,HexOutThreeNeg);

  hexConvert1(Temps[1]);
  strcat(Payload,HexOutThreeNeg);

  hexConvert1(Temps[2]);
  strcat(Payload,HexOutThreeNeg);
}
