  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {  
    tempAir = SHT2x.GetTemperature();
    RHAir = SHT2x.GetHumidity();
    windSpeed = getWindSpeed();
    windDir = getWindDirection();
    solarRad = getSolarRadiation();
    uvRad = getUVRadiation();
    getFanPower();
    
    state = 2;   // goto state 2
    statsUpdated = 0;
  }
  
  /**************************************/
  /* check for Errors  (1x per minute)  */
  /**************************************/
  void checkErrors() {
    if(avgTempAir == -273 || avgRHAir == 0) {
      error = error | B00000001;
      SHT2x.resetSHT();
    }
    else error = error & B11111110;

    if(avgLoadCurrent < currentLow || avgLoadCurrent > currentHigh) {
      error = error | B00000010;
    }
    else error = error & B11111101;

    if(avgLoadVoltage < voltageLow || avgLoadVoltage > voltageHigh) {
      error = error | B00000100;
    }
    else error = error & B11111011;
  }


  /***************************************************/
  /* Calculate routine for the Windspeed in km/h     */
  /***************************************************/
  float getWindSpeed() {
    float windSpeedCalc = 0.0;
    noInterrupts(); // Rotations is set by interrupt, so interrupt needs to be disabled to read it
    windSpeedCalc = rotations * 0.9 * 1.60934; // V (mph) = P(2.25/2.5) = P * 0.9 * conv mph to kmh
    rotations = 0;  // Reset count for next sample
    interrupts();
    return windSpeedCalc;
  }


  /***************************************************/
  /* Calculate routine for the Wind Direction in °   */
  /***************************************************/
  int getWindDirection() {
    int rawValue = 0;
    int Direction = 0;
    rawValue = analogRead(windDirectionPin);
    Direction = map(rawValue, 0, 1023, 0, 359);
    return Direction;
  }

  /****************************************/
  /* Calculation Routine to convert       */
  /* the sensor voltage to a W/m2 value   */
  /* taking a sensor deviation of +/- 12% */
  /* into account depending on Airtemp vs */
  /* the reference temp of 25°C           */
  /****************************************/
  float getSolarRadiation() {
    float rawValue = 0.0;
    float result = 0.0;
    float compensation = 0.0;
    rawValue = analogRead(solarRadiationPin);     // analog read value 0-1023
    result = (rawValue * 5 * 1800)/(1024 * 3);    // Convert from 5V input range to 3V output from sensor, max sensor value is 1800W/m2
  
    compensation = (0.12 * abs(refTemp - tempAir) * result)/100;  // 0.12% compensation per °C difference from ref temp of 25°C
  
    if(refTemp >= tempAir) {   // add the compensation if tempAir is lower or equal to 25°C (if equal, compensation will be 0)
      result += compensation;
    }
    if(refTemp < tempAir) {
      result -= compensation;
    }
    
    return result;
  }

  /***************************************************/
  /* Calculate routine for the uv Sensor in UVI      */
  /***************************************************/
  float getUVRadiation() {
    float rawValue = 0.0;
    float result = 0.0;
    rawValue = analogRead(uvRadiationPin);
    result = (rawValue * 5 * 16)/(1024 * 2.5);    // Convert from 5V input range to 0-2.5V output from sensor, max sensor value is UVI 16
    return result;
  }

  /***************************************************/
  /* Calculate routine for the FAN Current Sensor    */
  /***************************************************/
  void getFanPower() {
    float current_mA = 0.0;
    float shuntvoltage = 0.0;
    float busvoltage = 0.0;
    current_mA = ina219.getCurrent_mA();
    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    loadVoltage = busvoltage + (shuntvoltage / 1000);
    loadCurrent = (int)current_mA;
  }