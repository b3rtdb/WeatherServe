  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    pinMode(fanPin, OUTPUT);      // sets the fanpin as output
    pinMode(errorPin, OUTPUT);    // sets the errorpin as output
    pinMode(rainInterruptPin, INPUT);
    pinMode(windSpeedInterruptPin, INPUT); 
    
    Wire.begin();                 // Setup the I2C
    Serial.begin(115200);         // Baudrate 115200 for debug HW serial to PC
    Serial1.begin(9600);          // Baudrate 9600 for Zigbee Wireless interface
    xbee.setSerial(Serial1);

    ina219.begin();
    ina219.setCalibration_16V_400mA();
  
    digitalWrite(fanPin,1);       // Set the Fars fan ON
    digitalWrite(errorPin,0);     // Set the error led OFF

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

    initStats();                  // Clear the statistics Arrays
    state = 1;
    flashLed(1, 50);
  }

  /****************************************/
  /* Routines to flash error led          */  
  /****************************************/
  void flashLed(int times, int wait) {
    for (int i = 0; i < times; i++) {
      digitalWrite(errorPin, HIGH);
      delay(wait);
      digitalWrite(errorPin, LOW);
  
      if (i + 1 < times) {
        delay(wait);
      }
    }
  }

  /****************************************/
  /* Interrupt routine for rainsensor     */
  /* Count rain gauge bucket tips         */
  /****************************************/
  void rainIRQ() {
      rainCount++; 
  } 

  /****************************************/
  /* Interrupt routine for windspeed      */
  /* count ROTATIONS                      */
  /****************************************/
  void windRotationIRQ() {
      rotations++; 
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
