  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    Serial.begin(115200);     // Baudrate 115200 for debug HW serial to PC
    Serial1.begin(9600);      // Baudrate 9600 for Zigbee Wireless interface

    xbee.setSerial(Serial1);
    lps25hb.begin(0x5D);

    sps30Start();

    attachInterrupt(digitalPinToInterrupt(sunHoursInterruptPin), sunHoursIRQ, RISING);

    // Setup the timer interupt to request sensor data
    Timer1.initialize(500000);    // µs (0,5s)
    Timer1.attachInterrupt(TimerIRQ);

    noInterrupts();
    timerCount = 0;
    sunCount = 0;
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

  /****************************************/
  /* Interrupt routine for sunHour sensor */
  /* One Count for each 36seconds         */
  /****************************************/
  void sunHoursIRQ() {
      sunCount++; 
  } 

  /**************************************/
  /* Start SPS30 Sensor (takes 12 sec)  */
  /**************************************/
  void sps30Start() {
    Serial2.begin(115200,SERIAL_8N1);  // Baudrate 115200 for SPS30 PM sensor
    Serial2.write(buf_rst,6);          // reset
    delay(1000);
    Sps30Read();
    Serial2.write(buf_start,8);        // start
    delay(1000);
    Sps30Read();
    Serial2.write(buf_clean,6);        // clean
    delay(10000); // cleaning takes 10 seconds
    
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
