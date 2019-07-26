  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {
    for(int i=0;i<2;i++) {
      dataStringComplete = 0;
      dataString = "";
      if(i==1) {
        Serial2.print("\r"); // trigger measurement of NO2 Sensor
        dataString = Serial2.readStringUntil('\n');
        if(dataString == "") error = error | B00000001; // something is wrong with the NO2 sensor
        else error = error & B11111110;
        
      }
      else if(i==2) {
        Serial3.print("\r"); // trigger measurement of O3 Sensor
        dataString = Serial3.readStringUntil('\n');
        if(dataString == "") error = error | B00000010; // something is wrong with the O3 sensor
        else error = error & B11111101;
      }
      else break;
      parseData(i);
    }
    state = 2;   // goto state 2
    statsUpdated = 0;
  }

  /****************************************/
  /* Parse Data From Sensors              */  
  /****************************************/
  void parseData(byte sensor) {
    long Concentration;
    String SensorSerialNo;
    long Temperature;
    long Humidity;
    
    int idx1 = dataString.indexOf(',');
    SensorSerialNo = dataString.substring(0, idx1);
    int idx2 = dataString.indexOf(',', idx1 + 1);
    String S_gas = dataString.substring(idx1 + 2, idx2);
    Concentration = S_gas.toInt();
    int idx3 = dataString.indexOf(',', idx2 + 1);
    String S_temp = dataString.substring(idx2 + 2, idx3);
    Temperature = S_temp.toInt();
    int idx4 = dataString.indexOf(',', idx3 + 1);
    String S_humi = dataString.substring(idx3 + 2, idx4);
    Humidity = S_humi.toInt();
    
    switch (sensor) {
      case 1: SensorSerialNO2 = SensorSerialNo;
              NO2ppb = Concentration;
              TemperatureNO2 = Temperature;
              HumidityNO2 = Humidity;
              break;
      case 2: SensorSerialO3 = SensorSerialNo;
              O3ppb = Concentration;
              TemperatureO3 = Temperature;
              HumidityO3 = Humidity;
              break;
      default: break;
    }
  }
