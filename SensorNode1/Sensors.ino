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
  
  /****************************************/
  /* Interrupt routine for rainsensor     */
  /* Count rain gauge bucket tips         */
  /****************************************/
  void rainIRQ() {
    rainCount++;
  }


  /***************************************************/
  /* Calculate routine for the Windspeed in km/h     */
  /***************************************************/
  float getWindSpeed() {
    float voltageValue = 0.0;
    float rawValue = 0.0;
    float convertedValue = 0.0;
    rawValue = analogRead(windspeedPin);        // Get a value between 0 and 1023 from the analog pin connected to the anemometer
    voltageValue = (rawValue * 5) / 1024; // Convert sensor value to actual voltage
  
    //Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
  
    if (voltageValue <= voltageMin) {
      convertedValue = 0;                            // Check if voltage is below minimum value. If so, set wind speed to zero.
    }
    else {
      convertedValue = (voltageValue - voltageMin)* windSpeedMax / (voltageMax - voltageMin); //For voltages above minimum value, use the linear relationship to calculate wind speed.
    }
    convertedValue*= 3.6;                            // Convert from m/s to km/h
    return convertedValue;
  }


  /***************************************************/
  /* Calculate routine for the Wind Direction in °   */
  /***************************************************/
  float getWindDirection() {
    float rawValue = 0.0;
    rawValue = analogRead(windDirectionPin);
    rawValue /= 1024;
    rawValue *= 360; // degrees
    return rawValue;
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
  float getFanCurrent() {
    float rawValue = 0.0;
    float result = 0.0;
    rawValue = analogRead(fanCurrentPin);
    result = rawValue;   // 185mV/A, gain (from 4.27 to 47 with potmeter), FAN = 1,8W = 150mA bij 12V. Set limit at 200mA = 37mV x gain=30 = 1110mV -> / 5000mV*1024 rawvalue = 227.
    return result;
  }

