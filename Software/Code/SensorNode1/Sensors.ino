  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {  
    tempAir = SHT2x.GetTemperature();
    RHAir = SHT2x.GetHumidity();
    windSpeed = getWindSpeed();
    getExtADC();  // winddir, solar, uv
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
  /* Calculate routine for winddir, solar, uv        */
  /***************************************************/
  void getExtADC() {
    for(int i=0;i<3;i++) {
      float rawAdcValue = mcp320xRead(i);
      switch(i) {
        case 0: windDir = map(rawAdcValue, 0, 4096, 0, 359); break;
        case 1: solarRad = rawAdcValue * 0.6; // raw*(4.096V * 1800W/m² / (4096 * 3Vmax)
                float compensation = (0.12 * abs(refTemp - tempAir) * solarRad)/100;  // 0.12% compensation per °C difference from ref temp of 25°C
  
                if(refTemp >= tempAir) {   // add the compensation if tempAir is lower or equal to 25°C (if equal, compensation will be 0)
                  solarRad += compensation;
                }
                if(refTemp < tempAir) {
                  solarRad -= compensation;
                }break;
        case 2: uvRad = rawAdcValue * 0.0064; break; // raw*(4.096V * 16 / (4096 * 2,5Vmax)
      }
    }
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
