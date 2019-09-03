  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {  
    tempAir = SHT2x.GetTemperature();
    Serial.print("temp: ");
    Serial.println(tempAir);
    RHAir = SHT2x.GetHumidity();
    Serial.print("rh: ");
    Serial.println(RHAir);
    getExtADC();  // winddir, solar, uv
  }

  /***************************************************/
  /* Calculate routine for winddir, solar, uv        */
  /***************************************************/
  void getExtADC() {
    for(int i=0;i<3;i++) {
      float rawAdcValue = mcp320xRead(i);
      Serial.print("rawadc nr: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print("value");
      Serial.println(rawAdcValue);
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
