// Version v1.0__2019-04-10
// Version v1.1__2019-11-03 : add watchdog timer 4s

/*
 * PRESSURE: I²C, D21 (SCL), D20 (SDA)
 * SUN HOURS SENSOR: D2 (interrupt 4)
 * XBEE: Serial1 on pins D19 (RX) and D18 (TX)
 * SPS30: Serial2 on pins D17 (RX) and D16 (TX)
 * DEBUG: Serial on pins D0 (RX) and D1 (TX)
*/

#include <avr/wdt.h>
#include <XBee.h>
#include <Statistic.h>
#include <SparkFun_LPS25HB_Arduino_Library.h>
#include <TimerOne.h>
#include <sps30.h>


  /****************************************/
  /* LPS25HB Pressure Sensor              */
  /****************************************/
  LPS25HB pressureSensor;
  float pressure = 0;

  /****************************************/
  /* SPS30 Laser Particle Sensor          */
  /****************************************/
  #define SP30_COMMS SERIALPORT2
  SPS30 sps30;
  int MassPM1Value=0;         // define PM1.0 value
  int MassPM2Value=0;         // define PM2.5 value
  int MassPM4Value=0;         // define PM4 value
  int MassPM10Value=0;        // define PM10 value

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/  
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array, start with 1 cause of the node ident 2
  
  uint8_t payload[23];               // array of length 23, 0-22
  // SH + SL Address of receiving XBee (Coordinator)
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40f748cb);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

  // union to convert float to byte string
  union u_tag {
    uint8_t b[4];
    float fval;
  } u;

  /****************************************/
  /* Sunhours Sensor declarations         */
  /****************************************/
  const byte sunHoursInterruptPin = 2;
  volatile byte sunCount = 0;

  /****************************************/
  /* Statistics Declarations              */
  /****************************************/
  Statistic pressureStats;
  Statistic PM1Stats;
  Statistic PM2Stats;
  Statistic PM4Stats;
  Statistic PM10Stats;
  float avgPressure;
  float avgPM1;
  float avgPM2;
  float avgPM4;
  float avgPM10;

  byte statsUpdated = 0;

  /****************************************/
  /* System Declarations                  */
  /****************************************/
  byte counter = 0;
  volatile byte state = 0;
  volatile unsigned int timerCount = 0; // used to determine 2.5sec timer count
  byte error = B00000000;               // 1 = LPS25HB error, 2 = SPS30 Communication Error

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
  
  if (counter > 24) {  // transition from state 2 -> 3
    state = 3;
  }
}
    
