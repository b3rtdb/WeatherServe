  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    pinMode(rainInterruptPin, INPUT);
    pinMode(windSpeedInterruptPin, INPUT); 
    
    Wire.begin();                 // Setup the I2C
    
    Serial.begin(115200);         // Baudrate 115200 for debug HW serial to PC

    mcp320xInit(SINGLE, MCP3204, PINSLAVE); // single or differential, 4 channel ADC, CS = 46
  

    attachInterrupt(digitalPinToInterrupt(rainInterruptPin), rainIRQ, FALLING);   // configure the Interrupt for the rainsensor
    attachInterrupt(digitalPinToInterrupt(windSpeedInterruptPin), windRotationIRQ, FALLING);  // configure the Interrupt for the windspeedsensor

    // Setup the timer interupt for the windspeedsensor & request sensor data
    Timer1.initialize(500000);    // µs (0,5s)
    Timer1.attachInterrupt(TimerIRQ);

    noInterrupts();
    timerCount = 0; 
    rotations = 0;
    rainCount = 0;
    interrupts();
  }


  /****************************************/
  /* Interrupt routine for rainsensor     */
  /* Count rain gauge bucket tips         */
  /****************************************/
  void rainIRQ() {
      Serial.println("rain IRQ detected"); 
  } 

  /****************************************/
  /* Interrupt routine for windspeed      */
  /* count ROTATIONS                      */
  /****************************************/
  void windRotationIRQ() {
      Serial.println("wind IRQ detected"); 
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
