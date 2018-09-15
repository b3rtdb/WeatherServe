#include <Wire.h>
#include <SPI.h>
#include <AS3935.h>
#include <XBee.h>
#include <Statistic.h>
#include <ClosedCube_LPS25HB.h>


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
  /* Declarations for AS3935              */
  /****************************************/
  void printAS3935Registers();
  byte SPItransfer(byte sendByte);
  void AS3935Irq();
  volatile int AS3935IrqTriggered;
  #define IRQpin 2
  #define CSpin 48
  AS3935 AS3935(SPItransfer,CSpin,IRQpin);
  int strokeDistance=0;
  byte lightningDetected=0;

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/  
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array, start with 1 cause of the node ident 2
  
  uint8_t payload[23];               // array of length 22, 0-22, to send the sensor values as a nmea string
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
  /* State Machine Declarations           */
  /****************************************/
  byte state = 0;
  byte counter = 0;
  #define REQUEST_RATE 6000          // in milliseconds - Sample rate, 6000 default (6s)
  unsigned long lastupdate = 0;      // timer value when last measure update was done

  byte error = B00000000;            // 1 = LPS25HB error, 2 = PMSensor Error, 4= AS3935 Tuning NOK, 8 = AS3935 Noise level too high, 16 = AS3935 Disturber detected

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
  }
  
  if (counter > 10) {  // transition from state 2 -> 3
    state = 3;
  }
  
  if ( ( millis()-lastupdate ) > REQUEST_RATE ) { // transition from state 2 -> 1
    state = 1;
  }
}
    


