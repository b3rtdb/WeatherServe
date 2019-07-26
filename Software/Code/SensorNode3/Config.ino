  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    Serial.begin(115200);     // Baudrate 115200 for debug HW serial to PC
    Serial1.begin(9600);      // Baudrate 9600 for Zigbee Wireless interface
    Serial2.begin(9600);      // Baudrate 9600 for NO2 Sensor
    Serial3.begin(9600);      // Baudrate 9600 for O3 Sensor
    
    xbee.setSerial(Serial1);
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
  }

  /****************************************/
  /* Interrupt routine for Request        */
  /* sensor data TIMER                    */
  /****************************************/
  void TimerIRQ() {
    timerCount++;
    if(timerCount == 2)  {          // Timer interrupt every 1 second  2 x 0,5s = 1s
      state = 1;                    // transition from state 2 -> 1
      timerCount = 0;
    }
  }

  /**************************************/
  /* Reset NO2 & O3 Sensor              */
  /**************************************/
  void SPEC_reset(){
    Serial2.print("r");
    Serial3.print("r");
    delay(1000);
  }
