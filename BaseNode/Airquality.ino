/********************************************
 * Calculate the Airquality PM10 & PM2,5    *
 * every 10minutes an average is made       *
 * which is used for a running 24h average  *
 ********************************************/
void calcAirQuality() {
  pm10mCounter++;
  if(pm10mCounter == 10) {
    pm10mCounter = 0;
    pm25Array[pmArrayCounter] = PM25;
    pm10Array[pmArrayCounter] = PM10;
    pmArrayCounter++;

    for (byte i = 0; i<144; i++) {
      avgPm25_24h += *(pm25Array + i); // = pm25Array[i]
      avgPm10_24h += *(pm10Array + i); // = pm10Array[i]
    }

    avgPm25_24h /= 144;
    avgPm10_24h /= 144;
  }
  
  if(pmArrayCounter == 144) {
    pmArrayCounter = 0;
  }
}
