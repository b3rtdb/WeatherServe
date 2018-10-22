  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    
    Wire.begin();             // Setup the I2C
    Serial.begin(115200);     // Baudrate 115200 for debug HW serial to PC
    Serial1.begin(9600);      // Baudrate 9600 for Zigbee Wireless interface
    Serial2.begin(9600);      // Baudrate 9600 for Laser Particle Sensor
    Serial2.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
    xbee.setSerial(Serial1);
    configAS3935();
    lps25hb.begin(0x5D);

    // Setup the timer interupt to request sensor data
    Timer1.initialize(500000);    // µs (0,5s)
    Timer1.attachInterrupt(TimerIRQ);

    noInterrupts();
    timerCount = 0; 
    interrupts();
  
    state = 1;
    initStats();              // Clear the statistics Arrays
  }

  /****************************************/
  /* Transfer data via SPI                */
  /****************************************/
  byte SPItransfer(byte sendByte) {
    return SPI.transfer(sendByte);
  }


  /****************************************/
  /* Config AS3935                        */
  /****************************************/
  void configAS3935() {
    SPI.begin();
    SPI.setDataMode(SPI_MODE1);
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setBitOrder(MSBFIRST);
    AS3935.reset();
    if(!AS3935.calibrate()) {
      Serial.println("Tuning out of range");
      error = error | B00000100;
    }
    else error = error & B11111011;
  
    outputCalibrationValues();
    recalibrate();
  
    AS3935.setNoiseFloor(2); // 0..7         (the higher, the higher the threshold will be, also depends on indoor/outdoor setting)
    AS3935.setSpikeRejection(2); // 0..11    (the higher, the more robust for disturbers, but also less efficient for detection)
    AS3935.setWatchdogThreshold(3); // 0..10 (the higher, the more robust for disturbers, but also less efficient for detection)
    AS3935.setMinimumLightnings(1); // 0..3  (values respectivily 1, 5, 9, 16)
    
    outputCalibrationValues();
    recalibrate();
  
    //AS3935.setIndoors();
    AS3935.setOutdoors();
  
    //AS3935.disableDisturbers();  
    AS3935.enableDisturbers();
  
    printAS3935Registers();
    AS3935IrqTriggered = 0;
    attachInterrupt(0,AS3935Irq,RISING);
  }

  /****************************************/
  /* Calibration Values - DEBUG           */
  /****************************************/
  void outputCalibrationValues() {
     // output the frequencies that the different capacitor values set:
    delay(50);
    Serial.println();
    for (byte i = 0; i <= 0x0F; i++) {
      int frequency = AS3935.tuneAntenna(i);
      Serial.print("tune antenna to capacitor ");
      Serial.print(i);
      Serial.print("\t gives frequency: ");
      Serial.print(frequency);
      Serial.print(" = ");
      long fullFreq = (long) frequency*160;  // multiply with clock-divider, and 10 (because measurement is for 100ms)
      Serial.print(fullFreq,DEC);
      Serial.println(" Hz");
      delay(10);
    }
  }

  /****************************************/
  /* Calibration Routine                  */
  /****************************************/
  void recalibrate() {
    delay(50);
    Serial.println();
    int calCap = AS3935.getBestTune();
    Serial.print("antenna calibration picks value:\t ");
    Serial.println(calCap);
    delay(50);
  }

  /****************************************/
  /* Print the registers of the AS3935    */
  /****************************************/
  void printAS3935Registers() {
    int noiseFloor = AS3935.getNoiseFloor();
    int spikeRejection = AS3935.getSpikeRejection();
    int watchdogThreshold = AS3935.getWatchdogThreshold();
    int minLightning = AS3935.getMinimumLightnings();
    Serial.print("Noise floor is: ");
    Serial.println(noiseFloor,DEC);
    Serial.print("Spike rejection is: ");
    Serial.println(spikeRejection,DEC);
    Serial.print("Watchdog threshold is: ");
    Serial.println(watchdogThreshold,DEC); 
    Serial.print("Minimum Lightning is: ");
    Serial.println(minLightning,DEC);   
  }

  /****************************************/
  /* Interrupt routine for Request        */
  /* sensor data TIMER                    */
  /****************************************/
  void TimerIRQ() {
    timerCount++;
    if(timerCount == 5)  {          // Timer interrupt every 2.5 seconds  5 x 0,5s = 2,5s
      state = 1;                    // transition from state 2 -> 1
      timerCount = 0;
    }
  }


  /**************************************/
  /* Check LPS25HB Sensor               */
  /**************************************/
  void checkLps25hb() {
    byte lpsIdent = 0x00;
    if(lps25hb.whoAmI() == 0xBD) {
      error = error & B11111110;
    }
    else {
      error = error | B00000001;
    }
  }
