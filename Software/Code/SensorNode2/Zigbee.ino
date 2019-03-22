  /****************************************************/
  /* Routine for TRANSMITTING sensordata in API mode  */  
  /****************************************************/
  void zigbeeTransmit() {
    calcStats();
    checkLps25hb();

    // Header
    payload[0] = 2;   // wireless sensor node identifier, WSN2
        
    u.fval = avgPressure;
    f2b();      // avgPressure pos 1-4
    u.fval = avgPM01;
    f2b();      // avgPM01 pos 5-8
    u.fval = avgPM2_5;
    f2b();      // avgPM2_5 pos 9-12
    u.fval = avgPM10;
    f2b();      // avgPM10 pos 13-16

    noInterrupts(); //sunCount is set by interrupt, so interrupt needs to be disabled to read it
    payload[arrayOffset] = sunCount;       // this is a byte (pos 17)
    interrupts();
    arrayOffset++;

    payload[arrayOffset] = error;          // this is a byte (pos 18)
    
    xbee.send(zbTx);
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
          //Serial.println("Xbee transmit success");
          initStats();
          counter = 0;
          state = 1;
        } else {
          //Serial.println("remote XBee did not receive our packet");
        }
      }
    }
    else if (xbee.getResponse().isError()) {
      //Serial.println("Error reading packet.  Error code: ");
      //Serial.println(xbee.getResponse().getErrorCode());
    }
    else {
      //Serial.println("local XBee did not provide a timely TX Status Response");
    }
  }
