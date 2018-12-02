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

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  RHAir = floatValue;
  if(RHAir > 100) RHAir = 100;       // RHAir at pos 5

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  windSpeed = floatValue;            // windspeed at pos 9

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  windGustRec = floatValue;          // windgust at pos 13

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  windDir = floatValue;              // winddir at pos 17
  if(windDir < windDirOffset) {
    windDir += 360;
  }
  windDir -= windDirOffset;

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  solarRad = (int)floatValue;        // solarrad at pos 21

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  uvIndex = floatValue;              // uv Index at pos 25

  arrayOffsetRX+= 4;
  rainSensorCount = rx.getData(arrayOffsetRX);    // raincount at pos 29

  arrayOffsetRX++;
  errorWSN1 = rx.getData(arrayOffsetRX);          // errorbyte WSN1 at pos 30

  flashLed(onboardLed, 1, 50);
}

  
  /****************************************/
  /* Get Data of Wireless Sensor Node 2   */
  /****************************************/
void getNode2Data() {
  onlineFlagWSN2 = 2;
  lastupdateWSN2 = millis();
  
  float floatValue = 0;

  b2f();
  floatValue = u.fval;
  pressure = (unsigned long)((floatValue*100)+pressCorrection);              // Pressure at pos 1

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  if(floatValue < 100) {
    PM01 = (int)floatValue;                  // PM1.0 at pos 5
  }

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  if(floatValue < 100) {
    PM25 = (int)floatValue;                 // PM2.5 at pos 9
  }

  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  if(floatValue < 300) {
    PM10 = (int)floatValue;                  // PM10 at pos 13
  }
  
  arrayOffsetRX+= 4;
  b2f();
  floatValue = u.fval;
  lightningDistance = (byte)floatValue;            // Lightning Distance at pos 17

  arrayOffsetRX+= 4;
  lightningDetected = rx.getData(arrayOffsetRX);    // Lightning Detected at pos 21

  arrayOffsetRX++;
  errorWSN2 = rx.getData(arrayOffsetRX);          // errorbyte WSN2 at pos 22

  flashLed(onboardLed, 2, 50);
}

  /****************************************/
  /* Broadcast Data to Display Node       */
  /****************************************/
void xbeeTx() {     // max payloadXbee without fragmentation is 84 bytes
  arrayOffsetTX = 0;
  payloadXbee[arrayOffsetTX] = 9;   // Coordinator node identifier, 9, pos 0
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = onlineFlagWSN1;    // WSN 1 status pos 1
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = onlineFlagWSN2;    // WSN 2 status pos 2
  arrayOffsetTX++;

  u.fval = tempAir;
  f2b();      // tempAir pos 3-6
  u.fval = maxTempAir;
  f2b();      // Max Temp Air pos 7-10
  u.fval = minTempAir;
  f2b();      // Min Temp Air pos 11-14
  payloadXbee[arrayOffsetTX] = (byte)RHAir; // RHAir pos 15
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = (byte)windRunKm; // WindRun pos 16
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = (byte)(sunHoursDec*10); // Sunhours Decimal pos 17
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = (byte)(ETday*10); // ETday pos 18
  arrayOffsetTX++;
  u.fval = avgWindSpeed10m;
  f2b();      // Current Windspeed pos 19-22
  u.fval = windGust;
  f2b();      // WindGust pos 23-26
  u.fval = windDir;
  f2b();      // Current Wind Direction pos 27-30
  u.fval = avgWindDir;
  f2b();      // 60min Average Wind Direction pos 31-34

  u.fval = pressure;    // Pressure in Pa
  f2b();      // pressure pos 35-38
  u.fval = avgPm25_24h;
  f2b();      // Particulate Matter 2,5 pos 39-42
  u.fval = avgPm10_24h;
  f2b();      // Particulate Matter 10 pos 43-46
  
  payloadXbee[arrayOffsetTX] = lightningDetected;   // Lightning Detected pos 47
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = lightningDistance;
  arrayOffsetTX++;                                  // Lightning Distance pos 48

  u.fval = rainIntensity;
  f2b();      // Rain Intensity pos 49-52
  u.fval = totalRain24h;
  f2b();      // Total Rain Today pos 53-56

  u.fval = apparentT;
  f2b();      // Apparent Temperature pos 57-60
  u.fval = solarRad;
  f2b();      // Solar Radiation pos 61-64
  payloadXbee[arrayOffsetTX] = (byte)(avgUV10m*10); // uvIndex pos 65
  arrayOffsetTX++;

  payloadXbee[arrayOffsetTX] = zambrettiNumber; // Zambretti Forecast Number pos 66
  arrayOffsetTX++;
  
  payloadXbee[arrayOffsetTX] = (byte)(moonPhase*100); // Moon phase (illuminated) pos 67
  arrayOffsetTX++;
  
  payloadXbee[arrayOffsetTX] = moonPhaseNumber;
  arrayOffsetTX++;                          // Moon phase number to determine phase pos 68
  
  payloadXbee[arrayOffsetTX] = (byte)hRising;   // sunrise hour pos 69
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = (byte)mRising;   // sunrise minute pos 70
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = (byte)hSetting;  // sunset hour pos 71
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = (byte)mSetting;  // sunset minute pos 72
  arrayOffsetTX++;
  
  payloadXbee[arrayOffsetTX] = trend;           // air pressure trend pos 73

  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = errorWSN1;       // Error byte of WSN1 pos 74
  arrayOffsetTX++;
  payloadXbee[arrayOffsetTX] = errorWSN2;       // Error byte of WSN1 pos 75

  xbee.send(zbTx);
  flashLed(onboardLed, 5, 50);
}


  /****************************************************/
  /* Function to convert 4 bytes into a float         */  
  /****************************************************/
void b2f() {
    for (int i=0;i<4;i++){
      u.b[i] = rx.getData(arrayOffsetRX+i);
    }
}

  /****************************************************/
  /* Function to convert a float into 4 bytes         */  
  /****************************************************/
void f2b() {
    for (int i=0;i<4;i++){
      payloadXbee[i+arrayOffsetTX]=u.b[i];
    }
    arrayOffsetTX +=4;
}
