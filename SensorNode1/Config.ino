  /****************************************/
  /* Routines for configuration           */  
  /****************************************/
  void configHardware() {
    pinMode(fanPin, OUTPUT);      // sets the fanpin as output
    pinMode(errorPin, OUTPUT);    // sets the errorpin as output
    
    Wire.begin();                 // Setup the I2C
    Serial.begin(115200);         // Baudrate 115200 for debug HW serial to PC
    Serial1.begin(9600);          // Baudrate 9600 for Zigbee Wireless interface
    xbee.setSerial(Serial1);
  
    initStats();                  // Clear the statistics Arrays
    digitalWrite(fanPin,1);       // Set the Fars fan ON
    digitalWrite(errorPin,0);     // Set the error led OFF

    pinMode(rainInterruptPin, INPUT);
    pinMode(windSpeedInterruptPin, INPUT); 
    attachInterrupt(digitalPinToInterrupt(rainInterruptPin), rainIRQ, FALLING);   // configure the Interrupt for the rainsensor
    attachInterrupt(digitalPinToInterrupt(windSpeedInterruptPin), windRotationIRQ, FALLING);  // configure the Interrupt for the windspeedsensor

    // Setup the timer interupt 
    Timer1.initialize(500000);    // µs (0,5s)
    Timer1.attachInterrupt(windTimerIRQ);

    windSampleRequired = false;
    noInterrupts();
    TimerCount = 0; 
    Rotations = 0;
    rainCount = 0;
    interrupts();

  
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
      Rotations++; 
  }

  /****************************************/
  /* Interrupt routine for windspeed      */
  /* count TIMER                          */
  /****************************************/
  void windTimerIRQ() {
    TimerCount++;
    if(TimerCount == 5)  {  //Timer interrupt every 2.5 seconds  5 x 0,5s = 2,5s
      windSampleRequired = true;
      TimerCount = 0;
    }
  }
