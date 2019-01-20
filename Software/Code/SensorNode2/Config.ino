  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    Serial.begin(115200);     // Baudrate 115200 for debug HW serial to PC
    Serial1.begin(9600);      // Baudrate 9600 for Zigbee Wireless interface
    Serial2.begin(9600);      // Baudrate 9600 for Laser Particle Sensor
    Serial2.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
    xbee.setSerial(Serial1);
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
    if(lps25hb.whoAmI() != 0xBD) {
      error = error | B00000001;
    }
    else {
      error = error & B11111110;
    }
  }
