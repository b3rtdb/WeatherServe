/*
 * PRESSURE: I²C, D21 (SCL), D20 (SDA)
 * SUN HOURS SENSOR: D2 (interrupt 4)
 * XBEE: Serial1 on pins D19 (RX) and D18 (TX)
 * SPS30: Serial2 on pins D17 (RX) and D16 (TX)
 * DEBUG: Serial on pins D0 (RX) and D1 (TX)
*/

#include <XBee.h>
#include <Statistic.h>
#include <ClosedCube_LPS25HB.h>
#include <TimerOne.h>


  /****************************************/
  /* LPS25HB Pressure Sensor              */
  /****************************************/
  ClosedCube_LPS25HB lps25hb;
  float pressure = 0;

  /****************************************/
  /* SPS30 Laser Particle Sensor          */
  /****************************************/
  const uint8_t buf_start[8]={0x7e, 0x00, 0x00, 0x02, 0x01, 0x03, 0xf9, 0x7e};  // start command
  const uint8_t buf_stop[6]={0x7e, 0x00, 0x01, 0x00, 0xfe, 0x7e};               // stop command
  const uint8_t buf_read[6]={0x7e, 0x00, 0x03, 0x00, 0xfc, 0x7e};               // read command
  const uint8_t buf_rst[6]={0x7e, 0x00, 0xD3, 0x00, 0x2c, 0x7e};                // reset command
  const uint8_t buf_clean[6]={0x7E, 0x00, 0x56, 0x00, 0xA9, 0x7E};              // clean now command
  int PM01Value=0;          //define PM1.0 value of the air detector module
  int PM2_5Value=0;         //define PM2.5 value of the air detector module
  int PM10Value=0;          //define PM10 value of the air detector module

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/  
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array, start with 1 cause of the node ident 2
  
  uint8_t payload[19];               // array of length 19, 0-18
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
  Statistic PM01Stats;
  Statistic PM2_5Stats;
  Statistic PM10Stats;
  Statistic tempSurfaceStats;
  float avgPressure;
  float avgPM01;
  float avgPM2_5;
  float avgPM10;
  float avgTempSurface;

  byte statsUpdated = 0;

  /****************************************/
  /* System Declarations                  */
  /****************************************/
  byte counter = 0;
  volatile byte state = 0;
  volatile unsigned int timerCount = 0; // used to determine 2.5sec timer count
  byte error = B00000000;               // 1 = LPS25HB error, 2 = SPS30 Error

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
    
