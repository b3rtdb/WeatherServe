  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {  
    pressure = lps25hb.readPressure();
    Sps30Read();
    
    state = 2;   // goto state 2
    statsUpdated = 0;
  }

  /****************************************/
  /* Read routine for PMsensor            */  
  /****************************************/
  void Sps30Read() {
    uint8_t buff[60] = {0};
    float PMArray[10] = {0};
    float decimal = 0.0;
    int integer, len = 0;
  
    if(Serial2.read()==0x7e) {        // Start with 0x7E
      error = error & B11111101;
      if(Serial2.read()==0) {         // Address is always 0
        Serial2.read();               // Command (don't do anything with it)
        errorCode(Serial2.read());    // State
        len=Serial2.read();           // Read data length
        int j=0;
        do {
          buff[j]=Serial2.read();
          j++;
        }
        while(buff[j-1]!=0x7e);       // Read data+checksum to the 0x7e
        
        if (len>0) {                  // Read procedure
          for(int i=0; i<(j-2);i++) { // Replace special values (byte stuffing)
            if (buff[i]==0x7d){
              if (buff[i+1]=0x5E){buff[i]=0x7E;};
              if (buff[i+1]=0x5D){buff[i]=0x7D;};
              if (buff[i+1]=0x31){buff[i]=0x11;};
              if (buff[i+1]=0x33){buff[i]=0x13;};
        
              for (int c = i + 1; c < (j-3); c++) {
                buff[c] = buff[c+1];
              }
            }
          }
          for(int i=0;i<10;i++) {
            integer = ((buff[4*i]<<24)|(buff[4*i+1]<<16)|(buff[4*i+2]<<8)|buff[4*i+3]);
            memcpy(&decimal, &integer, sizeof(integer));
            PMArray[i] = decimal;
          }
          PM01Value = PMArray[0];
          PM2_5Value = PMArray[1];
          PM10Value = PMArray[3];
        }
      }
    }
    else {
      error = error | B00000010;
    }  
  }


  void errorCode(uint8_t state) {
    switch (state) {
    case 0:
      error = error & B11111101;
      break;
    default:
      error = error | B00000010;
      break;
    }
  }
