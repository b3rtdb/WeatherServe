void getXbeeData() {
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      
      nodeIdent = rx.getData(0);    // identifier at pos 0
      if (nodeIdent == 9) {   // Coordinator
        onlineFlagDue = 2;
        lastUpdateDue = millis();
        getDueData();
        processData();
      }
    }
  }
}

void getDueData() {                        // Due status pos 0
    arrayOffset = 1;
    onlineFlagWSN1 = rx.getData(arrayOffset);   // WSN 1 status pos 1
    arrayOffset++;
    onlineFlagWSN2 = rx.getData(arrayOffset);   // WSN 2 status pos 2
    arrayOffset++;
    onlineFlagWSN3 = rx.getData(arrayOffset);   // WSN 3 status pos 3
    arrayOffset++;
            
    b2f();
    floatStore = u.fval;
    tempAir = floatStore;              // tempAir at pos 4-7
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    maxTempAir = floatStore;           // Max tempAir at pos 8-11
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    minTempAir = floatStore;           // Min tempAir at pos 12-15
    arrayOffset+= 4;
    
    RHAir = rx.getData(arrayOffset);        // RHAir at pos 16
    arrayOffset++;
    windRunKm = rx.getData(arrayOffset);    // windRunKm at pos 17
    arrayOffset++;
    sunHoursDec = rx.getData(arrayOffset);  // sunHoursDec at pos 18
    sunHoursDec /= 10;
    arrayOffset++;
    ETday = rx.getData(arrayOffset);        // ETday at pos 19
    ETday /= 10;
    arrayOffset++;
    
    b2f();
    floatStore = u.fval;
    windSpeed = floatStore;            // windspeed at pos 20-23
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    maxWindSpeed = floatStore;         // wind gust at pos 24-27
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    windDir = floatStore;              // winddir at pos 28-31
    windDir += 180;
    if(windDir > 360) windDir -= 360;
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    avgWindDir = floatStore;           // avg winddir at pos 32-35
    avgWindDir += 180;
    if(avgWindDir > 360) avgWindDir -= 360;
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    pressure = floatStore;             // pressure at pos 36-39
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    avgPm25_24h = floatStore;          // avg PM2,5 at pos 40-43
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    avgPm10_24h = floatStore;          // avg PM10 at pos 44-47
    arrayOffset+= 4;

    b2f();
    floatStore = u.fval;
    rainIntensity = floatStore;             // Rain Intensity at pos 48-51
    arrayOffset+= 4;
    
    b2f();
    floatStore = u.fval;
    totalRain24h = floatStore;              // Total Rain Today at pos 52-55
    arrayOffset+= 4;

    b2f();
    floatStore = u.fval;
    tempFeelslike = floatStore;             // feelslike temp at pos 56-59
    arrayOffset+= 4;

    b2f();
    floatStore = u.fval;
    solarRad = floatStore;                  // Solar Rad at pos 60-63
    arrayOffset+= 4;

    uvIndex = rx.getData(arrayOffset);      // uvIndex at pos 64
    uvIndex /= 10;
    arrayOffset++;

    b2f();
    floatStore = u.fval;
    avgNO2 = floatStore;                    // avgNO2 at pos 65-68
    arrayOffset+= 4;

    b2f();
    floatStore = u.fval;
    avgO3 = floatStore;                     // avgO3 at pos 69-72
    arrayOffset+= 4;

    zambrettiNumber = rx.getData(arrayOffset);  // Zambretti Forecast Number pos 73
    arrayOffset++;
    
    moonPhase = rx.getData(arrayOffset);        // Moon phase (illuminated) pos 74
    arrayOffset++;

    moonPhaseNumber = rx.getData(arrayOffset);  // Moon phase number pos 75
    arrayOffset++;
    
    hRising = rx.getData(arrayOffset);       // sunrise hour pos 76
    arrayOffset++;
    mRising = rx.getData(arrayOffset);       // sunrise minute pos 77
    arrayOffset++;
    hSetting = rx.getData(arrayOffset);      // sunset hour pos 78
    arrayOffset++;
    mSetting = rx.getData(arrayOffset);      // sunset minute pos 79
    arrayOffset++;
    trend = rx.getData(arrayOffset);         // air pressure trend pos 80
    
    arrayOffset++;
    errorWSN1 = rx.getData(arrayOffset);     // Error byte of WSN1 pos 81
    arrayOffset++;
    errorWSN2 = rx.getData(arrayOffset);     // Error byte of WSN2 pos 82
    arrayOffset++;
    errorWSN2 = rx.getData(arrayOffset);     // Error byte of WSN3 pos 83
}


  /****************************************************/
  /* Function to convert 4 bytes into float           */  
  /****************************************************/
void b2f() {
    for (int i=0;i<4;i++){
      u.b[i] = rx.getData(arrayOffset + i);
    }
}

  /****************************************************/
  /* Functions to convert avgPM 2,5µ and 10µ to an    */
  /* AQI index which will be used to color the values */  
  /****************************************************/
void processData() {
  if (avgPm25_24h <= 5) AQI25=0;
  else if ((avgPm25_24h > 5) && (avgPm25_24h <= 10)) AQI25=1;
  else if ((avgPm25_24h > 10) && (avgPm25_24h <= 15)) AQI25=2;
  else if ((avgPm25_24h > 15) && (avgPm25_24h <= 20)) AQI25=3;
  else if ((avgPm25_24h > 20) && (avgPm25_24h <= 30)) AQI25=4;
  else if ((avgPm25_24h > 30) && (avgPm25_24h <= 40)) AQI25=5;
  else if ((avgPm25_24h > 40) && (avgPm25_24h <= 50)) AQI25=6;
  else if ((avgPm25_24h > 50) && (avgPm25_24h <= 60)) AQI25=7;
  else if ((avgPm25_24h > 60) && (avgPm25_24h <= 70)) AQI25=8;
  else if (avgPm25_24h > 70) AQI25=9;

  if (avgPm10_24h <= 10) AQI10=0;
  else if ((avgPm10_24h > 10) && (avgPm10_24h <= 20)) AQI10=1;
  else if ((avgPm10_24h > 20) && (avgPm10_24h <= 30)) AQI10=2;
  else if ((avgPm10_24h > 30) && (avgPm10_24h <= 40)) AQI10=3;
  else if ((avgPm10_24h > 40) && (avgPm10_24h <= 50)) AQI10=4;
  else if ((avgPm10_24h > 50) && (avgPm10_24h <= 60)) AQI10=5;
  else if ((avgPm10_24h > 70) && (avgPm10_24h <= 100)) AQI10=6;
  else if ((avgPm10_24h > 100) && (avgPm10_24h <= 150)) AQI10=7;
  else if ((avgPm10_24h > 150) && (avgPm10_24h <= 200)) AQI10=8;
  else if (avgPm10_24h > 200) AQI10=9;

  if (uvIndex <= 2.5) AQIuv=0;
  else if ((uvIndex > 2.5) && (uvIndex <= 5.5)) AQIuv=1;
  else if ((uvIndex > 5.5) && (uvIndex <= 7.5)) AQIuv=2;
  else if ((uvIndex > 7.5) && (uvIndex <= 10.5)) AQIuv=3;
  else if (uvIndex > 10.5) AQIuv=4;

  if (rainIntensity == 0) RIR=0; // none
  else if ((rainIntensity > 0) && (rainIntensity <= 0.25)) RIR=1; // very light
  else if ((rainIntensity > 0.25) && (rainIntensity <= 1)) RIR=2; // light
  else if ((rainIntensity > 1) && (rainIntensity <= 4)) RIR=3; // moderate
  else if ((rainIntensity > 4) && (rainIntensity <= 16)) RIR=4; // heavy
  else if ((rainIntensity > 16) && (rainIntensity <= 50)) RIR=5; // very heavy
  else if (rainIntensity > 50) RIR=6; // extreme
}
