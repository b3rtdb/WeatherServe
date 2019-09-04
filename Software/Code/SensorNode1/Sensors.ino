  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {  
    tempAir = SHT2x.GetTemperature();
    RHAir = SHT2x.GetHumidity();
    windSpeed = getWindSpeed();
    getExtADC();  // winddir, solar, uv
    
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
  /* Calculate routine for winddir, solar, uv        */
  /***************************************************/
  void getExtADC() {
    for(int i=0;i<3;i++) {
      float rawAdcValue = mcp320xRead(i);
      rawAdcValue -= offsetADC;
      if(rawAdcValue < 0) rawAdcValue = 0;
      rawAdcArray[i] = rawAdcValue;
    }
      windDir = map(rawAdcArray[0], 0, 3300, 0, 359);  // (3V3 max)
                  
      solarRad = rawAdcArray[1] * 0.6; // raw*(4.096V * 1800W/m² / (4096 * 3V0 max)
      float compensation = (0.12 * abs(refTemp - tempAir) * solarRad)/100;  // 0.12% compensation per °C difference from ref temp of 25°C

      if(refTemp >= tempAir) {   // add the compensation if tempAir is lower or equal to 25°C (if equal, compensation will be 0)
        solarRad += compensation;
      }
      if(refTemp < tempAir) {
        solarRad -= compensation;
      }
                  
      uvRad = rawAdcArray[2] / 156.25; // raw*(4.096V * 16 / (4096 * 2V5 max)
  }
