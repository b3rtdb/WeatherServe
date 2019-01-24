  /****************************************/
  /* subroutines for the statistics       */
  /* recalculate every time a Zigbee send */
  /* occurs, which is by default 1min     */
  /* Every 6s new data is added, so       */
  /* always stats of 10 measurements      */
  /****************************************/
  void calcStats() {
      avgPressure = pressureStats.average();
      avgPM01 = PM01Stats.average();
      avgPM2_5 = PM2_5Stats.average();
      avgPM10 = PM10Stats.average();
      avgTempGnd = tempGndStats.average();
      avgTempSurface = tempSurfaceStats.average();
  }
  
  void initStats() {
    pressureStats.clear();
    PM01Stats.clear();
    PM2_5Stats.clear();
    PM10Stats.clear();
    tempGndStats.clear();
    tempSurfaceStats.clear();
  }

  void updateStats() {
    if(statsUpdated == 0) {
      pressureStats.add(pressure);
      PM01Stats.add(PM01Value);
      PM2_5Stats.add(PM2_5Value);
      PM10Stats.add(PM10Value);
      tempGndStats.add(tempGnd);
      tempSurfaceStats.add(tempSurface);
      
      statsUpdated = 1;
      counter++;
    }
  }
