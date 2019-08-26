  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    Serial.begin(115200);     // Baudrate 115200 for debug HW serial to PC
 //   Serial1.begin(9600);      // Baudrate 9600 for Zigbee Wireless interface
    Serial2.begin(9600);      // Baudrate 9600 for NO2 Sensor
    Serial3.begin(9600);      // Baudrate 9600 for O3 Sensor
    
 //   xbee.setSerial(Serial1);
    Serial2.setTimeout(1500);
    Serial3.setTimeout(1500);

    // reserve 80 bytes for the dataString
    dataString.reserve(80);
    
    // Setup the timer interupt to request sensor data
    Timer1.initialize(500000);    // µs (0,5s)
    Timer1.attachInterrupt(TimerIRQ);

    noInterrupts();
    timerCount = 0;
    interrupts();
  
    state = 1;
    initStats();              // Clear the statistics Arrays

    SPEC_reset();
    //getEepromNO2();
    //getEepromO3();
  }

  /****************************************/
  /* Interrupt routine for Request        */
  /* sensor data TIMER                    */
  /****************************************/
  void TimerIRQ() {
    timerCount++;
    if(timerCount == 5)  {          // Timer interrupt every 1 second  5 x 0,5s = 2.5s
      state = 1;                    // transition from state 2 -> 1
      timerCount = 0;
    }
  }

  /**************************************/
  /* Reset NO2 & O3 Sensor              */
  /**************************************/
  void SPEC_reset(){
    Serial2.print('r');
    Serial3.print('r');
    delay(1000);
  }

  /**************************************/
  /* Get EEPROM data NO2 sensor         */
  /**************************************/
  void getEepromNO2() {
    long eepromInt[13];
    String eepromStr[5];
    float Sensitivity_Code;
    
    while (Serial2.available()) Serial2.read();
    Serial2.write('e');
    while (!Serial2.available()) {}
    String data = Serial2.readStringUntil('\n'); //Read Header

    while (!Serial2.available()) {}
    for (int i = 1; i < 14; i++) {
      data = Serial2.readStringUntil('\n');
      String subS1 = data.substring(0, data.indexOf('='));
      String subS2 = data.substring(data.indexOf('=') + 2);
      eepromInt[i] = subS2.toInt();
      Serial.print(subS1);
      Serial.print("= ");
      Serial.println(eepromInt[i]);
    }
    for (int i = 14; i < 19; i++) {
      data = Serial2.readStringUntil('\n');
      String subS1 = data.substring(0, data.indexOf('='));
      String subS2 = data.substring(data.indexOf('=') + 2);
      eepromStr[i - 14] = subS2;
      Serial.print(subS1);
      Serial.print("= ");
      Serial.println(eepromStr[i - 14]);
    }
    data = Serial2.readStringUntil('\n');
    String subS1 = data.substring(0, data.indexOf('='));
    String subS2 = data.substring(data.indexOf('=') + 2);
    Sensitivity_Code = subS2.toFloat();
    Serial.print(subS1);
    Serial.print("= ");
    Serial.println(Sensitivity_Code);
}

/**************************************/
  /* Get EEPROM data O3 sensor         */
  /**************************************/
  void getEepromO3() {
    long eepromInt[13];
    String eepromStr[5];
    float Sensitivity_Code;
    
    while (Serial3.available()) Serial3.read();
    Serial3.write('e');
    while (!Serial3.available()) {}
    String data = Serial3.readStringUntil('\n'); //Read Header

    while (!Serial3.available()) {}
    for (int i = 1; i < 14; i++) {
      data = Serial3.readStringUntil('\n');
      String subS1 = data.substring(0, data.indexOf('='));
      String subS2 = data.substring(data.indexOf('=') + 2);
      eepromInt[i] = subS2.toInt();
      Serial.print(subS1);
      Serial.print("= ");
      Serial.println(eepromInt[i]);
    }
    for (int i = 14; i < 19; i++) {
      data = Serial3.readStringUntil('\n');
      String subS1 = data.substring(0, data.indexOf('='));
      String subS2 = data.substring(data.indexOf('=') + 2);
      eepromStr[i - 14] = subS2;
      Serial.print(subS1);
      Serial.print("= ");
      Serial.println(eepromStr[i - 14]);
    }
    data = Serial3.readStringUntil('\n');
    String subS1 = data.substring(0, data.indexOf('='));
    String subS2 = data.substring(data.indexOf('=') + 2);
    Sensitivity_Code = subS2.toFloat();
    Serial.print(subS1);
    Serial.print("= ");
    Serial.println(Sensitivity_Code);
}
  
