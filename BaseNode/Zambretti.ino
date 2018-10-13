/********************************************
 * Calculate the pressure trend             *
 * Calculate the Zambretti Forecast         *
 ********************************************/
void calcPrediction() {
  if(pressureArrayCounter == 0 && pressure3hCounter == 0) {
    pressureArray[pressureArrayCounter] = pressure;   // at midnight, both counters are reset, current pressure at pos 0
    pdif = pressureArray[pressureArrayCounter] - pressureArray[pressureArrayCounter+7];   // at midnight, calculate pdif between 0h today and 21h yesterday (0:0h, 1:3h, 2:6h, 3:9h, 4:12h, 5:15h, 6:18h, 7:21h)

    if(pdif <= 30 && pdif >= -30) trend = 0;  // STEADY, difference < 0,3 hPa/3h
    else if(pdif < -30) trend = 1;            // FALLING, difference > -0,3 hPa/3h
    else if(pdif > 30) trend = 2;             // RISING, difference > +0,3 hPa/3h

    calcZambretti();
  }
  
  pressure3hCounter++;

  if(pressure3hCounter == (180*(60.0/logInterval))){
    pressureArrayCounter++; // move up one position
    pressureArray[pressureArrayCounter] = pressure;
    pdif = pressureArray[pressureArrayCounter] - pressureArray[pressureArrayCounter-1];
    pressure3hCounter = 0;

    if(pdif <= 30 && pdif >= -30) trend = 0; // STEADY, difference < 0,3 hPa/3h
    else if(pdif < -30) trend = 1; // FALLING, difference > -0,3 hPa/3h
    else if(pdif > 30) trend = 2; // RISING, difference > +0,3 hPa/3h

    calcZambretti();
  }
}

  /****************************************/
  /* Calculation of the Zambretti Number  */
  /****************************************/
void calcZambretti() {

  float Ffactor = 0;
  byte FfactorClip = 0;
  int zambrettiPressCorr = 0;
  zambrettiPressCorr = calcPressureCorrection();

  if(trend == 2) {  // rising
    if(month >= 4 && month <= 9) zambrettiPressCorr+=320;
    Ffactor = 0.1740 * ((float)(103140 - zambrettiPressCorr)/100);
    FfactorClip = min(max( (int)(Ffactor + 0.5),0),13); // length of LUT-1 = 13
    zambrettiNumber = LutRising[FfactorClip];
  }

  if(trend == 1) {  // falling
    if(month >= 4 && month <= 9) zambrettiPressCorr-=320;
    Ffactor = 0.1553 * ((float)(102995 - zambrettiPressCorr)/100);
    FfactorClip = min(max( (int)(Ffactor + 0.5),0),9); // length of LUT-1 = 9
    zambrettiNumber = LutFalling[FfactorClip];
  }

  if(trend == 0) {  // steady
    Ffactor = 0.2314 * ((float)(103081 - zambrettiPressCorr)/100);
    FfactorClip = min(max( (int)(Ffactor + 0.5),0),16); // length of LUT-1 = 16
    zambrettiNumber = LutSteady[FfactorClip];
  }
}

  /****************************************/
  /* Calculation of Pressure Correction   */
  /****************************************/
int calcPressureCorrection() {

  unsigned long pressureNormalized = 0;
  signed int PressCorr = 0;
  byte windNumber = 0;
  pressureNormalized = (95000 + ((105000 - 95000) * (pressure - baroBottom) / (baroTop - baroBottom)));
  
  if(avgWindSpeed10m > 1) {
    windNumber = calcWindNumber();
    PressCorr = zambrettiPressCorrArray[windNumber];
    PressCorr += pressureNormalized;
  }
  else PressCorr = pressureNormalized;
  return PressCorr;
}

  /****************************************/
  /* Calculation of the WindDirNumber     */
  /****************************************/
byte calcWindNumber(){
  byte windNumber = 0;
  if(windDir >= 348.75 || windDir < 11.25) windNumber = 0;   //N
  if(windDir >= 11.25 && windDir < 33.75) windNumber = 1;    //NNE
  if(windDir >= 33.75 && windDir < 56.25) windNumber = 2;    //NE
  if(windDir >= 56.25 && windDir < 78.75) windNumber = 3;    //ENE
  if(windDir >= 78.75 && windDir < 101.25) windNumber = 4;   //E
  if(windDir >= 101.25 && windDir < 123.75) windNumber = 5;  //ESE
  if(windDir >= 123.75 && windDir < 146.25) windNumber = 6;  //SE
  if(windDir >= 146.25 && windDir < 168.75) windNumber = 7;  //SSE
  if(windDir >= 168.75 && windDir < 191.25) windNumber = 8;  //S
  if(windDir >= 191.25 && windDir < 213.75) windNumber = 9;  //SSW
  if(windDir >= 213.75 && windDir < 236.25) windNumber = 10; //SW
  if(windDir >= 236.25 && windDir < 258.75) windNumber = 11; //WSW
  if(windDir >= 258.75 && windDir < 281.25) windNumber = 12; //W
  if(windDir >= 281.25 && windDir < 303.75) windNumber = 13; //WNW
  if(windDir >= 303.75 && windDir < 326.25) windNumber = 14; //NW
  if(windDir >= 326.25 && windDir < 348.75) windNumber = 15; //NNW
  return windNumber;
}

