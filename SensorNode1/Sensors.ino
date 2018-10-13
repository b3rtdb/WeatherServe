  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {
    lastupdate = millis(); // reset the timer
    
    tempAir = SHT2x.GetTemperature();
    RHAir = SHT2x.GetHumidity();
    windSpeed = getWindSpeed();
    windDirection = getWindDirection();
    solarRad = getSolarRadiation();
    uvRad = getUVRadiation();
    fanCurrent = getFanCurrent();

    if(tempAir == -273 || RHAir == 0) {
      error = error | B00000001;
      SHT2x.resetSHT();
    }
    else error = error & B11111110;
    
    if(fanCurrent < currentLow || fanCurrent > currentHigh) {
      error = error | B00000010;
    }
    else if(fanCurrent >= currentLow && fanCurrent <= currentHigh) {
      error = error & B11111101;
    }
    
    state = 2;   // goto state 2  
    statsUpdated = 0;
  }
  



  /***************************************************/
  /* Calculate routine for the Windspeed in km/h     */
  /***************************************************/
  float getWindSpeed() {
    float tempVal = 0.0;
    tempVal = windSpeedMph * speedConversion;
    return tempVal;
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
  int getFanCurrent() {
    float rawValue = 0.0;
    float voltage = 0.0; // 0-5V input corresponds with 0-1023 (10bit ADC), so 5/1024 = 4,883mV
    float current = 0.0;
    rawValue = analogRead(fanCurrentPin);
    voltage = rawValue * 4.883;
    current = (voltage - Vref) * sensitivity;
    return (int)current;
  }

