  /****************************************/
  /* subroutines for the statistics       */
  /* recalculate every time a Zigbee send */
  /* occurs, which is by default 1min     */
  /* Every 6s new data is added, so       */
  /* always stats of 10 measurements      */
  /****************************************/
  void calcStats() {
    avgWindSpeed = windSpeedStats.average();
    windGust = windSpeedStats.maximum();
    avgTempAir = tempAirStats.average();
    avgRHAir = RHAirStats.average();
    avgSolarRad = solarRadiationStats.average();
    avgUVRad = UVRadiationStats.average();
    avgLoadCurrent = loadCurrentStats.average();
    avgLoadVoltage = loadVoltageStats.average();
  }

  void initStats() {
    noInterrupts();
    rainCount = 0;
    interrupts();
    windSpeedStats.clear();        // clear the array for the windspeed statistics
    tempAirStats.clear();          // clear the array for the Air Temperature statistics
    RHAirStats.clear();            // clear the array for the Air Humidity statistics
    solarRadiationStats.clear();   // clear the array for the Solar Radiation Statistics
    UVRadiationStats.clear();      // clear the array for the UV Radiation Statistics
    loadCurrentStats.clear();      // clear the array for the Fan Load current Statistics
    loadVoltageStats.clear();      // clear the array for the Fan Load voltage Statistics
  }

  void updateStats() {
    if(statsUpdated == 0) {
      tempAirStats.add(tempAir);
      RHAirStats.add(RHAir);
      windSpeedStats.add(windSpeed);
      solarRadiationStats.add(solarRad);
      UVRadiationStats.add(uvRad);
      calcWindDirAvg(); // running avg 1 minute
      loadCurrentStats.add(loadCurrent);
      loadVoltageStats.add(loadVoltage);
      
      statsUpdated = 1;
      counter++;
    }
  }

  void calcWindDirAvg() {
    double sinMean,cosMean,arctanMean = 0;
    windDirArray[windDirArrayCounter] = windDir;
    windDirArrayCounter++;
  
    if (windDirArrayCounter == 24) {                       // Every (24 * 2.5 = 60s) the counter is reset. The values stay in the array and are overwritten --> running sum
      windDirArrayCounter = 0;
    }
  
    // reset the mean Sin and Cos
    sinMean = 0;
    cosMean = 0;
    // Calculate the sum of all the Sin and Cos in the array (60 elements)
    for(byte i=0; i<24; i++) {
      sinMean += sin(*(windDirArray + i) *PI / 180);
      cosMean += cos(*(windDirArray + i) *PI / 180);
    }
    // divide by amount of measurements to get the mean value
    sinMean /= 24;
    cosMean /= 24;
    // the mean wind direction is the arctan of sin/cos and converted from radians to degrees
    arctanMean = atan2(sinMean,cosMean) * 180 / PI;
    arctanMean += 360;
    avgWindDir = (int)arctanMean%360; // calculate arctanMeanResult
  }
