void calcTempHum() {
  calcMinMaxT();
  calcApparentT();
  calcDewpoint();
  calcFrostpoint();
}

  /****************************************/
  /* Calculation of Min/Max Temp - 24h    */
  /****************************************/
void calcMinMaxT() {
    if(tempAir < minTempAir) {
      minTempAir = tempAir;
    }
    if(tempAir > maxTempAir) {
      maxTempAir = tempAir;
    }
}

  /****************************************/
  /* Calculation of HeatIndex & Windchill */
  /****************************************/
  void calcApparentT() { // = Heat index combined with windchill temp
    float tempFahr = (((tempAir *9)/5) +32); // Fahrenheit
    float windchillT = (13.12 + (0.6215 * tempAir) - (11.37 * pow(avgWindSpeed10m,0.16)) + (0.3965 * tempAir * pow(avgWindSpeed10m,0.16))); //windchill temp
    float A = (((-10.3 + (1.1 * tempFahr) + (0.047 * RHAir))- 32) *5 / 9);
    float B = ((-42.379 + (2.04901523*tempFahr) + (10.14333127*RHAir) - (0.22475541*tempFahr*RHAir) - (0.00683783*tempFahr*tempFahr) - (0.05481717*RHAir*RHAir) + (0.00122874*tempFahr*tempFahr*RHAir) + (0.00085282*tempFahr*RHAir*RHAir) - (0.00000199*tempFahr*tempFahr*RHAir*RHAir) - 32) *5 / 9);

    if(tempFahr <= 50 && avgWindSpeed10m > 4.8) apparentT = windchillT;
    else if(A < 79) apparentT = A;
    else if(RHAir <= 13 && (tempFahr >= 80 && tempFahr <= 112)) apparentT = B - ((((13-RHAir)/4) * sqrt((17 - abs(tempFahr - 95))/17)- 32) *5 / 9);
    else if(RHAir > 85 && (tempFahr >= 80 && tempFahr <= 87)) apparentT = B + (((0.02 * (RHAir-85) * (87-tempFahr))- 32) *5 / 9);
    else apparentT = B;
  }

  /****************************************/
  /* Calc of dewpoint                     */
  /****************************************/
  void calcDewpoint() {
    float H = 0;
    H = (log10(RHAir)-2.0)/0.4343+(17.62*tempAir)/(243.12+tempAir);
    dewPoint = 243.12*H/(17.62-H);
  }

  /****************************************/
  /* Calc of frostpoint                   */
  /****************************************/
  void calcFrostpoint() {
    float H = 0;
    float dewPointKelvin = dewPoint + kelvinzero;
    float tempAirKelvin = tempAir + kelvinzero;
    H = dewPointKelvin - tempAirKelvin + (2671.02 / ((2954.61/tempAirKelvin) + 2.193665 * log10(tempAirKelvin) - 13.3448));
    frostPoint = H - kelvinzero;
  }
