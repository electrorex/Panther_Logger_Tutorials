//This tab contains all the code needed to measure the temperature sensor

#define ONE_WIRE_BUS 6 //Temp chain on digital pin 6
OneWire oneWire(ONE_WIRE_BUS);

void readTemps() {
  unsigned int TempsInterval = 3000*NSENSORS; //Give sensor 3 seconds per node
      for (int i = 0; i <= NSENSORS; i++) {
        byte present = 0;
        byte type_s;
        byte data[12];
        byte addr[8];
        
        byte rawtemp[12];
        oneWire.reset();
        oneWire.select(Address[i]);
        oneWire.write(0x44, 1);
        delay(2000);     // required delay
        present = oneWire.reset();
        oneWire.select(Address[i]);
        oneWire.write(0xBE);         // Read Scratchpad
        for (int j = 0; j < 9; j++) {           // we need 9 bytes
          rawtemp[j] = oneWire.read();
        }
        int16_t raw = (rawtemp[1] << 8) | rawtemp[0];
        if (type_s) {
          raw = raw << 3;
          if (rawtemp[7] == 0x10) {
            raw = (raw & 0xFFF0) + 12 - rawtemp[6];
          }
        } else {
          byte cfg = (rawtemp[4] & 0x60);
          if (cfg == 0x00) raw = raw & ~7;
          else if (cfg == 0x20) raw = raw & ~3;
          else if (cfg == 0x40) raw = raw & ~1;
        }
        Temps[i] = (float)raw / 16.0;
      }
}