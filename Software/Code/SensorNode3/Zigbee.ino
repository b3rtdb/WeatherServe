  /****************************************************/
  /* Routine for TRANSMITTING sensordata in API mode  */  
  /****************************************************/
  void zigbeeTransmit() {
    calcStats();

    // Header
    payload[0] = 3;   // wireless sensor node identifier, WSN3
        
    u.fval = avgNO2;
    f2b();      // avgNO2 pos 1-4
    u.fval = avgO3;
    f2b();      // avgO3 pos 5-8

    arrayOffset++;
    payload[arrayOffset] = error;          // this is a byte (pos 9)
    
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
