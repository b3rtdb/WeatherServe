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
  /* Laser Particle Sensor                */
  /****************************************/
  #define LENG 31   //0x42 + 31 bytes equal to 32 bytes
  unsigned char buf[LENG];
 
  int PM01Value=0;          //define PM1.0 value of the air detector module
  int PM2_5Value=0;         //define PM2.5 value of the air detector module
  int PM10Value=0;          //define PM10 value of the air detector module

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/  
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array, start with 1 cause of the node ident 2
  
  uint8_t payload[18];               // array of length 18, 0-17, to send the sensor values as a nmea string
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
  /* Statistics Declarations              */
  /****************************************/
  Statistic pressureStats;
  Statistic PM01Stats;
  Statistic PM2_5Stats;
  Statistic PM10Stats;
  float avgPressure;
  float avgPM01;
  float avgPM2_5;
  float avgPM10;

  byte statsUpdated = 0;

  /****************************************/
  /* System Declarations                  */
  /****************************************/
  byte counter = 0;
  volatile byte state = 0;
  volatile unsigned int timerCount = 0; // used to determine 2.5sec timer count
  byte error = B00000000;               // 1 = LPS25HB error, 2 = PMSensor Error, 4= AS3935 Tuning NOK, 8 = AS3935 Noise level too high, 16 = AS3935 Disturber detected

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
    default: state=1; break;
  }
  
  if (counter > 24) {  // transition from state 2 -> 3
    state = 3;
  }
}
    
