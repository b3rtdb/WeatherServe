/********************************************
 * Calculate the Airquality                 *
 * PM10, PM2,5, NO2 and O3                  *
 ********************************************/
void calcAirQuality() {
  particulateMatter();
  pollutants();
}

/********************************************
 * Calculate the avg PM10 and PM2,5         *
 * over 24 hours                            *
 ********************************************/
void particulateMatter() {
  pmCounter++;
  if(pmCounter == 10) {
    pmCounter = 0;
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


/********************************************
 * Calculate the avg NO2 and O3 with an     *
 * exponential averaging filter             *
 ********************************************/
void pollutants() {
  avgNO2prev = avgNO2;
  avgNO2 = alpha * NO2ppb + (100 - alpha) * avgNO2prev;

  avgO3prev = avgO3;
  avgO3 = alpha * O3ppb + (100 - alpha) * avgO3prev;
}
