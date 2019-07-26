  /****************************************/
  /* subroutines for the statistics       */
  /* recalculate every time a Zigbee send */
  /* occurs, which is by default 1min     */
  /* Every 6s new data is added, so       */
  /* always stats of 10 measurements      */
  /****************************************/
  void calcStats() {
      avgNO2 = NO2Stats.average();
      avgO3 = O3Stats.average();
  }
  
  void initStats() {
    NO2Stats.clear();
    O3Stats.clear();
  }

  void updateStats() {
    if(statsUpdated == 0) {
      NO2Stats.add(NO2ppb);
      O3Stats.add(O3ppb);
      
      statsUpdated = 1;
      counter++;
    }
  }
