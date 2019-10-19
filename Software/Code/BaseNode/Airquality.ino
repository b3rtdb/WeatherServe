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
 * Plus conversion factors & calibration    *
 ********************************************/
void pollutants() {
  avgNO2 = alpha * NO2ppb + (1.0 - alpha) * NO2ppbPrev;
  NO2ppbPrev = NO2ppb;
  avgNO2 *= calib_no2_a;
  avgNO2 += calib_no2_b;

  avgO3 = alpha * O3ppb + (1.0 - alpha) * O3ppbPrev;
  O3ppbPrev = O3ppb;
  avgO3 *= conv_O3_ppbugm3;
  avgO3 *= calib_O3_a;
  avgO3 += calib_O3_b;
}
