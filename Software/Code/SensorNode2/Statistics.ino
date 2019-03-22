  /****************************************/
  /* subroutines for the statistics       */
  /* recalculate every time a Zigbee send */
  /* occurs, which is by default 1min     */
  /* Every 6s new data is added, so       */
  /* always stats of 10 measurements      */
  /****************************************/
  void calcStats() {
      avgPressure = pressureStats.average();
      avgPM1 = PM1Stats.average();
      avgPM2 = PM2Stats.average();
      avgPM4 = PM4Stats.average();
      avgPM10 = PM10Stats.average();
  }
  
  void initStats() {
    noInterrupts();
    sunCount = 0;
    interrupts();
    pressureStats.clear();
    PM1Stats.clear();
    PM2Stats.clear();
    PM4Stats.clear();
    PM10Stats.clear();
  }

  void updateStats() {
    if(statsUpdated == 0) {
      pressureStats.add(pressure);
      PM1Stats.add(MassPM1Value);
      PM2Stats.add(MassPM2Value);
      PM4Stats.add(MassPM4Value);
      PM10Stats.add(MassPM10Value);
      
      statsUpdated = 1;
      counter++;
    }
  }
