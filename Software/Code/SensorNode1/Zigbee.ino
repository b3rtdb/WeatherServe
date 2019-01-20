  /****************************************************/
  /* Routine for TRANSMITTING sensordata in API mode  */  
  /****************************************************/
  void zigbeeTransmit() {
    calcStats();
    checkErrors();

    // Header
    payload[0] = 1;   // wireless sensor node identifier, WSN1

    u.fval = avgTempAir;
    f2b();      // tempAir pos 1-4
    u.fval = avgRHAir;
    f2b();      // RHAir pos 5-8
    u.fval = avgWindSpeed;
    f2b();      // avgWindSpeed pos 9-12
    u.fval = windGust;
    f2b();      // windGust pos 13-16
    u.fval = avgWindDir;
    f2b();      // avgWindDir pos 17-20
    u.fval = avgSolarRad;
    f2b();      // avgSolarRad pos 21-24
    u.fval = avgUVRad;
    f2b();      // avgUVRad pos 25-28

    noInterrupts(); //rainCount is set by interrupt, so interrupt needs to be disabled to read it
    payload[arrayOffset] = rainCount;       // this is a byte (pos 29)
    interrupts();
    arrayOffset++;
    payload[arrayOffset] = error;           // this is a byte (pos 30)
   
    xbee.send(zbTx);
    flashLed(1, 100);
    checkTxErrors();
    arrayOffset = 1;
  }


  /****************************************************/
  /* Function to convert float into 4 bytes           */  
  /****************************************************/
  void f2b() {
    for (int i=0;i<4;i++){
      payload[i+arrayOffset]=u.b[i];
    }
    arrayOffset +=4;
  }


  /****************************************************/
  /* Function to check for Xbee TX Errors             */  
  /****************************************************/
  void checkTxErrors() {
    if (xbee.readPacket(500)) {
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          flashLed(5, 50);
          //Serial.println("Xbee transmit success");
          initStats();
          counter = 0;
          state = 1;
        } else {
          flashLed(3, 500);
          //Serial.println("remote XBee did not receive our packet");
        }
      }
    }
    else if (xbee.getResponse().isError()) {
      //Serial.println("Error reading packet.  Error code: ");
      //Serial.println(xbee.getResponse().getErrorCode());
    }
    else {
      flashLed(2, 50);
      //Serial.println("local XBee did not provide a timely TX Status Response");
    }
  }
