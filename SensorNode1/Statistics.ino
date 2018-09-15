  /****************************************/
  /* subroutines for the statistics       */
  /* recalculate every time a Zigbee send */
  /* occurs, which is by default 1min     */
  /* Every 6s new data is added, so       */
  /* always stats of 10 measurements      */
  /****************************************/
  void calcStats() {
    avgWindSpeed = windSpeedStats.average();
    avgWindDirection = windDirectionStats.average();
    avgTempAir = tempAirStats.average();
    avgRHAir = RHAirStats.average();
    avgSolarRad = solarRadiationStats.average();
    avgUVRad = UVRadiationStats.average();
  }

  void initStats() {
    rainCount = 0;
    windSpeedStats.clear();        // clear the array for the windspeed statistics
    windDirectionStats.clear();    // clear the array for the wind direction statistics
    tempAirStats.clear();          // clear the array for the Air Temperature statistics
    RHAirStats.clear();            // clear the array for the Air Humidity statistics
    solarRadiationStats.clear();   // clear the array for the Solar Radiation Statistics
    UVRadiationStats.clear();      // clear the array for the UV Radiation Statistics
  }

  void updateStats() {
    if(statsUpdated == 0) {
      tempAirStats.add(tempAir);
      RHAirStats.add(RHAir);
      windSpeedStats.add(windSpeed);
      windDirectionStats.add(windDirection);
      solarRadiationStats.add(solarRad);
      UVRadiationStats.add(uvRad);
      
      statsUpdated = 1;
      counter++;
    }
  }
