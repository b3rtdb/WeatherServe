/********************************************
 * Receive the data via Zigbee              *
 * Depending on the node,                   *
 * get another dataset                      *
 ********************************************/
void getXbeeData() {
  byte nodeIdent = 0;
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);

      nodeIdent = rx.getData(0);    // identifier at pos 0
      arrayOffsetRX = 1;

      switch(nodeIdent) {
        case 1: getNode1Data();
          break;
        case 2: getNode2Data();
          break;
        default: break;
      }
    }
  }
}

  /****************************************/
  /* Get Data of Wireless Sensor Node 1   */
  /****************************************/
void getNode1Data() {
  onlineFlagWSN1 = 2;
  lastupdateWSN1 = millis();
  
  float floatValue = 0;
       
  b2f();
  floatValue = u.fval;
  tempAir = floatValue;              // tempAir at pos 1
  
  b2f();
  floatValue = u.fval;
  RHAir = floatValue;
  if(RHAir > 100) RHAir = 100;       // RHAir at pos 5
  
  b2f();
  floatValue = u.fval;
  windSpeed = floatValue;            // windspeed at pos 9
  
  b2f();
  floatValue = u.fval;
  windGustRec = floatValue;          // windgust at pos 13
  
  b2f();
  floatValue = u.fval;
  windDir = floatValue;              // winddir at pos 17
  if(windDir < windDirOffset) {
    windDir += 360;
  }
  windDir -= windDirOffset;
  
  b2f();
  floatValue = u.fval;
  solarRad = (int)floatValue;        // solarrad at pos 21
  
  b2f();
  floatValue = u.fval;
  uvIndex = floatValue;              // uv Index at pos 25
  
  rainSensorCount = rx.getData(arrayOffsetRX);    // raincount at pos 29
  arrayOffsetRX++;
  
  errorWSN1 = rx.getData(arrayOffsetRX);          // errorbyte WSN1 at pos 30
}

  
  /****************************************/
  /* Get Data of Wireless Sensor Node 2   */
  /****************************************/
void getNode2Data() {
  onlineFlagWSN2 = 2;
  lastupdateWSN2 = millis();
  
  float floatValue = 0;

  b2f();
  floatValue = u.fval;        // Pressure at pos 1
  pressure = (unsigned long)((floatValue*100)+pressCorrection);
  
  b2f();
  floatValue = u.fval;
  PM01 = (int)floatValue;   // PM1.0 at pos 5
  
  b2f();
  floatValue = u.fval;
  PM25 = (int)floatValue;   // PM2.5 at pos 9

  b2f();
  floatValue = u.fval;
  PM4 = (int)floatValue;   // PM4 at pos 13
  
  b2f();
  floatValue = u.fval;
  PM10 = (int)floatValue;   // PM10 at pos 17
  
  sunCount = rx.getData(arrayOffsetRX);           // sunshine sensor Count at pos 21
  arrayOffsetRX++;
  
  errorWSN2 = rx.getData(arrayOffsetRX);          // errorbyte WSN2 at pos 22
}

  /****************************************/
  /* Broadcast Data to Display Node       */
  /****************************************/
void xbeeTx() {     // max payload without fragmentation is 84 bytes
  arrayOffsetTX = 0;
  payload[arrayOffsetTX] = 9;   // Coordinator node identifier, 9, pos 0
  arrayOffsetTX++;
  payload[arrayOffsetTX] = onlineFlagWSN1;    // WSN 1 status pos 1
  arrayOffsetTX++;
  payload[arrayOffsetTX] = onlineFlagWSN2;    // WSN 2 status pos 2
  arrayOffsetTX++;

  u.fval = tempAir;
  f2b();              // tempAir pos 3-6
  u.fval = maxTempAir;
  f2b();              // Max Temp Air pos 7-10
  u.fval = minTempAir;
  f2b();              // Min Temp Air pos 11-14
  
  payload[arrayOffsetTX] = (byte)RHAir;             // RHAir pos 15
  arrayOffsetTX++;
  payload[arrayOffsetTX] = (byte)windRunKm;         // WindRun pos 16
  arrayOffsetTX++;
  payload[arrayOffsetTX] = (byte)(sunHoursDec*10);  // Sunhours Decimal pos 17
  arrayOffsetTX++;
  payload[arrayOffsetTX] = (byte)(ETday*10);        // ETday pos 18
  arrayOffsetTX++;
  
  u.fval = avgWindSpeed10m;
  f2b();                    // Current Windspeed pos 19-22
  u.fval = windGust;
  f2b();                    // WindGust pos 23-26
  u.fval = windDir;
  f2b();                    // Current Wind Direction pos 27-30
  u.fval = avgWindDir;
  f2b();                    // 60min Average Wind Direction pos 31-34

  u.fval = pressure;
  f2b();                    // pressure (Pa) pos 35-38
  u.fval = avgPm25_24h;
  f2b();                    // Particulate Matter 2,5 pos 39-42
  u.fval = avgPm10_24h;
  f2b();                    // Particulate Matter 10 pos 43-46                            

  u.fval = rainIntensity;
  f2b();                    // Rain Intensity pos 47-50
  u.fval = totalRain24h;
  f2b();                    // Total Rain Today pos 51-54

  u.fval = apparentT;
  f2b();                    // Apparent Temperature pos 55-58
  u.fval = solarRad;
  f2b();                    // Solar Radiation pos 59-62
  payload[arrayOffsetTX] = (byte)(avgUV10m*10);   // uvIndex pos 63
  arrayOffsetTX++;

  payload[arrayOffsetTX] = zambrettiNumber;       // Zambretti Forecast Number pos 64
  arrayOffsetTX++;
  
  payload[arrayOffsetTX] = (byte)(moonPhase*100); // Moon phase (illuminated) pos 65
  arrayOffsetTX++;
  
  payload[arrayOffsetTX] = moonPhaseNumber;
  arrayOffsetTX++;                          // Moon phase number to determine phase pos 66
  
  payload[arrayOffsetTX] = (byte)hRising;   // sunrise hour pos 67
  arrayOffsetTX++;
  payload[arrayOffsetTX] = (byte)mRising;   // sunrise minute pos 68
  arrayOffsetTX++;
  payload[arrayOffsetTX] = (byte)hSetting;  // sunset hour pos 69
  arrayOffsetTX++;
  payload[arrayOffsetTX] = (byte)mSetting;  // sunset minute pos 70
  arrayOffsetTX++;
  
  payload[arrayOffsetTX] = trend;           // air pressure trend pos 71

  arrayOffsetTX++;
  payload[arrayOffsetTX] = errorWSN1;       // Error byte of WSN1 pos 72
  arrayOffsetTX++;
  payload[arrayOffsetTX] = errorWSN2;       // Error byte of WSN1 pos 73

  xbee.send(zbTx);
}

  /****************************************************/
  /* Function to convert 4 bytes into a float         */  
  /****************************************************/
void b2f() {
    for (int i=0;i<4;i++){
      u.b[i] = rx.getData(arrayOffsetRX+i);
    }
    arrayOffsetRX+= 4;
}

  /****************************************************/
  /* Function to convert a float into 4 bytes         */  
  /****************************************************/
void f2b() {
    for (int i=0;i<4;i++){
      payload[i+arrayOffsetTX]=u.b[i];
    }
    arrayOffsetTX +=4;
}
