void refreshDisplay() {
  lastUpdateRefresh = millis();
  CleO.Start();

  /****************************************/
  /* Display Temperatures                 */
  /****************************************/
  char bufTempAir[lengthOfFloat+1];
  dtostrf(tempAir, lengthOfFloat, 1, bufTempAir);
  CleO.StringExt(FONT_MEDIUM, 67, 55, colors2[7], MM, 0, 0, bufTempAir);

  char bufMinTempAir[lengthOfFloat+1];
  dtostrf(minTempAir, lengthOfFloat, 1, bufMinTempAir);
  CleO.StringExt(FONT_SANS_1, 143, 42, colors2[0], MR, 0, 0, bufMinTempAir);

  char bufMaxTempAir[lengthOfFloat+1];
  dtostrf(maxTempAir, lengthOfFloat, 1, bufMaxTempAir);
  CleO.StringExt(FONT_SANS_1, 143, 72, colors2[1], MR, 0, 0, bufMaxTempAir);

  /****************************************/
  /* Display Solar Radiation              */
  /****************************************/
  char bufSolarRad[lengthOfFloat+1];
  dtostrf(solarRad, lengthOfFloat, 0, bufSolarRad);
  CleO.StringExt(FONT_SANS_2, 367, 55, colors2[5], MM, 0, 0, bufSolarRad);

  char bufUvIndex[lengthOfFloat+1];
  dtostrf(uvIndex, lengthOfFloat, 1, bufUvIndex);
  CleO.StringExt(FONT_SANS_2, 431, 55, colors3[AQIuv], MM, 0, 0, bufUvIndex);

  /****************************************/
  /* Display Air Humidity  & ETday        */
  /****************************************/
  char bufRHAir[lengthOfFloat+1];
  dtostrf(RHAir, lengthOfFloat, 0, bufRHAir);
  if(RHAir > 99.9) CleO.StringExt(FONT_SANS_2, 418, 150, colors2[0], MM, 0, 0, "100");
  else CleO.StringExt(FONT_SANS_2, 418, 140, colors2[0], MM, 0, 0, bufRHAir);

  char bufETday[lengthOfFloat+1];
  dtostrf(ETday, lengthOfFloat, 1, bufETday);
  CleO.StringExt(FONT_SANS_2, 418, 165, colors2[0], MM, 0, 0, bufETday);

  /****************************************/
  /* Display Rain                         */
  /****************************************/
  char bufRainDay[3];
  dtostrf(totalRain24h, 4, 1, bufRainDay);
  CleO.StringExt(FONT_SANS_2, 217, 42, colors2[6], MM, 0, 0, bufRainDay);
  
  char bufRainIntensity[3];
  dtostrf(rainIntensity, 4, 1, bufRainIntensity);
  CleO.StringExt(FONT_SANS_2, 272, 42, colors2[6], MM, 0, 0, bufRainIntensity);

  /****************************************/
  /* Display Pressure, trend & prediction */
  /****************************************/
  char bufPressure[7];
  dtostrf(pressure/100, 6, 1, bufPressure);
  CleO.StringExt(FONT_SANS_2, 59, 165, colors2[3], MM, 0, 0, bufPressure);

  switch(trend) {
    case 0: CleO.StringExt(FONT_TINY, 59, 143, colors2[3], MM, 0, 0, "Steady");
      break;
    case 1: CleO.StringExt(FONT_TINY, 59, 143, colors2[3], MM, 0, 0, "Falling");
      break;
    case 2: CleO.StringExt(FONT_TINY, 59, 143, colors2[3], MM, 0, 0, "Rising");
      break;
  }
  

  /****************************************/
  /* Display Particulate Matter           */
  /****************************************/
  char bufPM2_5[lengthOfFloat+1];
  dtostrf(avgPm25_24h, lengthOfFloat, 1, bufPM2_5);   // avgPm25_24h
  CleO.StringExt(FONT_SANS_2, 418, 240, colors[AQI25], MM, 0, 0, bufPM2_5);

  char bufPM10[lengthOfFloat+1];
  dtostrf(avgPm10_24h, lengthOfFloat, 1, bufPM10);  // avgPm10_24h
  CleO.StringExt(FONT_SANS_2, 418, 265, colors[AQI10], MM, 0, 0, bufPM10);
  
  /****************************************/
  /* Display Wind Direction, Windspeed    */
  /****************************************/
  CleO.NeedleWidth(12);
  CleO.NeedleColor(PURPLE);
  CleO.NeedleR1R2(windCx, windCy, r0 - 19, r0 - 4, windDir);

  CleO.NeedleWidth(12);
  CleO.NeedleColor(GREEN);
  CleO.NeedleR1R2(windCx, windCy, r0 - 19, r0 - 4, avgWindDir);

  char bufWind[lengthOfFloat+1];
  dtostrf(windSpeed, lengthOfFloat, 1, bufWind);
  CleO.StringExt(FONT_SANS_2, windCx, 170, colors2[1], MM, 0, 0, bufWind);

  char bufWindMax[3];
  dtostrf(maxWindSpeed, 2, 0, bufWindMax);
  CleO.StringExt(FONT_SANS_2, 180, 150, colors2[1], MM, 0, 0, bufWindMax);

  char bufWindRun[3];
  dtostrf(windRunKm, 3, 0, bufWindRun);
  CleO.StringExt(FONT_SANS_2, 140, 150, colors2[1], MM, 0, 0, bufWindRun);

  /****************************************/
  /* Display Lightning                    */
  /****************************************/
  if (lightningDetected == 1) {
   char bufLightning[lengthOfFloat+1];
   dtostrf(lightningDistance, lengthOfFloat, 0, bufLightning);
   CleO.StringExt(FONT_SANS_2, 265, 305, colors2[7], MM, 0, 0, bufLightning);
   CleO.StringExt(FONT_TINY, 280, 305, colors2[7], MM, 0, 0, "km");
  }
  else {
   CleO.StringExt(FONT_TINY, 273, 305, colors2[8], MM, 0, 0, "NO");
  }

  /****************************************/
  /* Display Sunshine Hours               */
  /****************************************/
  char bufsunShine[lengthOfFloat+1];
  dtostrf(sunHoursDec, lengthOfFloat, 1, bufsunShine);
  CleO.StringExt(FONT_TINY, 225, 255, colors2[7], MM, 0, 0, bufsunShine);
  CleO.StringExt(FONT_TINY, 245, 255, colors2[7], MM, 0, 0, "h");

  /****************************************/
  /* Display Sunrise & Sunset times       */
  /****************************************/
   char bufHRising[lengthOfFloat+1];
   dtostrf(hRising, lengthOfFloat, 0, bufHRising);
   char bufHSetting[lengthOfFloat+1];
   dtostrf(hSetting, lengthOfFloat, 0, bufHSetting);
   char bufMRising[lengthOfFloat+1];
   dtostrf(mRising, lengthOfFloat, 0, bufMRising);
   char bufMSetting[lengthOfFloat+1];
   dtostrf(mSetting, lengthOfFloat, 0, bufMSetting);
   CleO.StringExt(FONT_TINY, 304, 305, colors2[11], MM, 0, 0, bufHRising);
   CleO.StringExt(FONT_TINY, 309, 305, colors2[11], MM, 0, 0, ":");
   if(mRising < 10) {
    CleO.StringExt(FONT_TINY, 314, 305, colors2[11], ML, 0, 0, "0");
    CleO.StringExt(FONT_TINY, 329, 305, colors2[11], MM, 0, 0, bufMRising);
   }
   else {
    CleO.StringExt(FONT_TINY, 321, 305, colors2[11], MM, 0, 0, bufMRising);
   }

   CleO.StringExt(FONT_TINY, 344, 305, colors2[11], MM, 0, 0, bufHSetting);
   CleO.StringExt(FONT_TINY, 354, 305, colors2[11], MM, 0, 0, ":");
   if(mSetting < 10) {
    CleO.StringExt(FONT_TINY, 358, 305, colors2[11], ML, 0, 0, "0");
    CleO.StringExt(FONT_TINY, 373, 305, colors2[11], MM, 0, 0, bufMSetting);
   }
   else {
    CleO.StringExt(FONT_TINY, 365, 305, colors2[11], MM, 0, 0, bufMSetting);
   }
   


  /****************************************/
  /* Display Moon Phase                   */
  /****************************************/
  char bufmoonPhase[lengthOfFloat+1];
  dtostrf(moonPhase, lengthOfFloat, 0, bufmoonPhase);
  CleO.StringExt(FONT_TINY, 220, 305, colors2[10], MM, 0, 0, bufmoonPhase);
  CleO.StringExt(FONT_TINY, 240, 305, colors2[10], MM, 0, 0, "%");

  switch(moonPhaseNumber) {
    case 1: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "New Moon");
      break;
    case 2: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "Waxing Crescent");
      break;
    case 3: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "First Quarter");
      break;
    case 4: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "Waxing Gibbous");
      break;
    case 5: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "Full Moon");
      break;
    case 6: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "Waning Gibbous");
      break;
    case 7: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "Last Quarter");
      break;
    case 8: CleO.StringExt(FONT_TINY, mes2Cx, mes3Cy, GRAY, MM, 0, 0, "Waning Crescent");
      break;
  }

  /****************************************/
  /* Display Prediction                   */
  /****************************************/
  switch(zambrettiNumber) {
    case 1: CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "Settled fine (1)");
      break;
    case 2: CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "Fine weather (2)");
      break;
    case 3: CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "Becoming fine (3)");
      break;
    case 4: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Fine, becoming");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "less settled (4)");
      break;
    case 5: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Fine, possible");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "showers (5)");
      break;
    case 6: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Fairly fine,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "improving (6)");
      break;
    case 7: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Fairly fine,possible");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "showers early (7)");
      break;
    case 8: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Fairly fine,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "showery later (8)");
      break;
    case 9: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Showery early,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "improving (9)");
      break;
    case 10: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Changeable,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "mending (10)");
      break;
    case 11: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Fairly fine,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "showers likely (11)");
      break;
    case 12: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Rather unsettled");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "clearing later (12)");
      break;
    case 13: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Unsettled, probably");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "improving (13)");
      break;
    case 14: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Showery, bright");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "intervals (14)");
      break;
    case 15: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "showery, becoming");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "less settled (15)");
      break;
    case 16: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Changeable,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "some rain (16)");
      break;
    case 17: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Unsettled, short");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "fine intervals (17)");
      break;
    case 18: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Unsettled,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "rain later (18)");
      break;
    case 19: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Unsettled,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "some rain (19)");
      break;
    case 20: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Mostly very,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "unsettled (20)");
      break;
    case 21: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Occasional rain,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "worsening (21)");
      break;
    case 22: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Rain at times,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "very unsettled (22)");
      break;
    case 23: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Rain at frequent,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "intervals (23)");
      break;
    case 24: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Rain, very");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "unsettled (24)");
      break;
    case 25: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Stormy,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "may improve (25)");
      break;
    case 26: CleO.StringExt(FONT_TINY, mes2Cx, mes1Cy, GRAY, MM, 0, 0, "Stormy,");
            CleO.StringExt(FONT_TINY, mes2Cx, mes2Cy, GRAY, MM, 0, 0, "much rain (26)");
      break;
  }

  /****************************************/
  /* Display Wireless Node Status         */
  /****************************************/
  switch(onlineFlagDue) {
    case 0:
      CleO.StringExt(FONT_TINY, 340, 260, GRAY, MM, 0, 0, "1");
      CleO.StringExt(FONT_TINY, 355, 260, GRAY, MM, 0, 0, "2");
    break;

    case 1:
      CleO.StringExt(FONT_TINY, 340, 260, YELLOW, MM, 0, 0, "1");
      CleO.StringExt(FONT_TINY, 355, 260, YELLOW, MM, 0, 0, "2");
    break;

    case 2:
      switch(onlineFlagWSN1) {
        case 0: CleO.StringExt(FONT_TINY, 340, 260, GRAY, MM, 0, 0, "1");
        break;
        case 1: CleO.StringExt(FONT_TINY, 340, 260, RED, MM, 0, 0, "1");
        break;
        case 2: CleO.StringExt(FONT_TINY, 340, 260, GREEN, MM, 0, 0, "1");
        break;
      }

      switch(onlineFlagWSN2) {
        case 0: CleO.StringExt(FONT_TINY, 355, 260, GRAY, MM, 0, 0, "2");
        break;
        case 1: CleO.StringExt(FONT_TINY, 355, 260, RED, MM, 0, 0, "2");
        break;
        case 2: CleO.StringExt(FONT_TINY, 355, 260, GREEN, MM, 0, 0, "2");
        break;
      }
    break;
  }

  /* Display completed screen frame */
  CleO.Show();
}
