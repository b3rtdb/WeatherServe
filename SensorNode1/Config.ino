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
  
    attachInterrupt(digitalPinToInterrupt(rainInterruptPin), rainIRQ, FALLING); // configure the Interrupt for the rainsensor
  
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
