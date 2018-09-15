void calcWind() {
  calcWindGust();
  calcWindSpeedAvg();
  calcWindDirAverage();
  calcWindDistance();
}

  /****************************************/
  /* Calculation MIN/MAX windspeed - 24h  */
  /****************************************/
void calcWindGust() {
  if(windGustRec > windGust) {
    windGust = windGustRec;
  }
}

  /****************************************/
  /* Calculation of average windspeed 10m */
  /****************************************/
void calcWindSpeedAvg() {
  windSpeedArray[windSpeedArrayCounter] = windSpeed;
  windSpeedArrayCounter++;
  
  if(windSpeedArrayCounter == 10) {
    windSpeedArrayCounter = 0;
  }
  avgWindSpeed10m = 0;
  for (byte i = 0; i<10; i++) {
    avgWindSpeed10m += *(windSpeedArray + i); // = windSpeedArray[i]
  }
  avgWindSpeed10m /= 10;
}

  /****************************************/
  /* Calculation of the wind Distance     */
  /****************************************/
void calcWindDistance() {
  double distance = windSpeed * 16.6667; // convert windspeed from km/h to meter/min as we have a logging interval of 1 minute
  windRun += distance;  // in meters
  windRunKm = windRun/1000.0;
}

  /****************************************/
  /* Calculation of average windDir 1h    */
  /****************************************/
void calcWindDirAverage() {
  double sinMean,cosMean,arctanMean = 0;
  windDirArray[windDirArrayCounter] = windDir;
  windDirArrayCounter++;

  if (windDirArrayCounter == 60) {                       // Every 60min the counter is reset. The values stay in the array and are overwritten --> running sum
    windDirArrayCounter = 0;
  }

  // reset the mean Sin and Cos
  sinMean = 0;
  cosMean = 0;
  // Calculate the sum of all the Sin and Cos in the array (60 elements)
  for(byte i=0; i<60; i++) {
    sinMean += sin(*(windDirArray + i) *PI / 180);
    cosMean += cos(*(windDirArray + i) *PI / 180);
  }
  // divide by amount of measurements to get the mean value
  sinMean /= 60;
  cosMean /= 60;
  // the mean wind direction is the arctan of sin/cos --> 60 measurements = 60 minutes (and converted from radians to degrees)
  arctanMean = atan2(sinMean,cosMean) * 180 / PI;
  arctanMean += 360;
  avgWindDir = (int)arctanMean%360; // calculate arctanMeanResult
}
