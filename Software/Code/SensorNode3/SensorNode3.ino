/*
 * XBEE: Serial1 on pins D19 (RX) and D18 (TX)
 * NO2: Serial2 on pins D17 (RX) and D16 (TX)
 * O3: Serial3 on pins D (RX) and D (TX)
 * DEBUG: Serial on pins D0 (RX) and D1 (TX)
*/
//long = 4 bytes

#include <XBee.h>
#include <Statistic.h>
#include <TimerOne.h>

  /****************************************/
  /* SPEC Sensor NO2                      */
  /****************************************/
  String SensorSerialNO2;
  long NO2ppb;
  long TemperatureNO2;
  long HumidityNO2;
  long RhNO2;

  /****************************************/
  /* SPEC Sensor 03                       */
  /****************************************/
  String SensorSerialO3;
  long O3ppb;
  long TemperatureO3;
  long HumidityO3;
  long RhO3;

  /****************************************/
  /* SPEC Sensor General vars             */
  /****************************************/
  #define command_delay 500
  #define start_delay 2500
  String dataString = "";
  boolean dataStringComplete = 0;

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/  
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array, start with 1 cause of the node ident 2
  
  uint8_t payload[10];               // array of length 10, 0-9
  // SH + SL Address of receiving XBee (Coordinator)
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40f748cb);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

  // union to convert long to byte string
  union u_tag {
    uint8_t b[4];
    long lval;
  } u;

  /****************************************/
  /* Statistics Declarations              */
  /****************************************/
  Statistic NO2Stats;
  Statistic O3Stats;
  float avgNO2;
  float avgO3;

  byte statsUpdated = 0;

  /****************************************/
  /* System Declarations                  */
  /****************************************/
  byte counter = 0;
  volatile unsigned int timerCount = 0; // used to determine 1sec timer count
  volatile byte state = 0;
  byte error = B00000000;               // 1 = NO3 Sensor error, 2 = O3 Sensor error
/**********************************************************************
 *
 * Setup routine
 *
 *********************************************************************/
void setup(void) 
{
  configHardware();
}

    
/**********************************************************************
 *
 * Main loop
 *
 *********************************************************************/
void loop(void) {
  switch(state) {
    case 1: requestSensorData(); break;
    case 2: updateStats(); break;
    case 3: zigbeeTransmit(); break;
    default: state = 1; break;
  }
  
  if (counter > 60) {  // transition from state 2 -> 3
    state = 3;
  }
}
    
