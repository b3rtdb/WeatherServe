  /****************************************/
  /* subroutines for the statistics       */
  /* recalculate every time a Zigbee send */
  /* occurs, which is by default 1min     */
  /* Every 1s new data is added, so       */
  /* always stats of 60 measurements      */
  /****************************************/
  void calcStats() {
      avgNO2 = NO2Stats.average();
      avgO3 = O3Stats.average();
      Serial.print("avg NO2: ");
      Serial.println(avgNO2);
      Serial.print("avg O3: ");
      Serial.println(avgO3);
      Serial.println("-----");
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
