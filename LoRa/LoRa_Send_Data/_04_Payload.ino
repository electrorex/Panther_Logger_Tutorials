//This handles neagtive values with the addition of four more hexadecimals required, so use only on variables that are expected to be negative
char hex1[10];
char* hexConverter1(float var){
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
       strcpy(hex1,HexVar);  
       return(hex1);
}

//Convert data expecting up to three integer numbers (before decimal) and two after decimal
//Negative values and values above "flag" are sent as the flagval
char hex2[10];
char* hexConverter2(float var, int flag, float flagval){
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
        strcpy(hex2,HexVar);  
        return(hex2);
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
      strcpy(hex2,HexVar);  
      return(hex2);
      }
}

char Payload[4000];
void getPayload(){
  strcpy(Payload,"AT+SEND=1:");

  hexConverter2(Batv,5,99);
  strcat(Payload,hex2);

  hexConverter1(Temps[0]);
  strcat(Payload,hex1);

  hexConverter1(Temps[1]);
  strcat(Payload,hex1);

  hexConverter1(Temps[2]);
  strcat(Payload,hex1);
}
