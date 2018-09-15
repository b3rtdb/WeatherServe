/*
 * SHT25: I²C, D21 (SCL), D20 (SDA) [3V3 supply]
 * WINDSPEED: A0 [12V supply]
 * WINDDIRECTION: A2 [12V supply]
 * SOLAR RADIATION: A4 [3V supply]
 * UV SENSOR: A6 [3V supply]
 * FAN CURRENT SENSOR: A8
 * RAIN SENSOR: D2 (interrupt 4)
 * FAN FARS: D5 ON/OFF control (6,5V)
 * XBEE: Serial1 on pins D19 (RX) and D18 (TX)
 * DEBUG: Serial on pins D0 (RX) and D1 (TX)
*/

#include <Wire.h>
#include <SHT2x.h>
#include <XBee.h>
#include <Statistic.h>

  /****************************************/
  /* Windspeed Sensor  0-5V               */
  /****************************************/
  const int windspeedPin = A0;
  float windSpeed = 0;

  const float voltageMin = 0.02;    // Mininum output voltage from anemometer in mV.
  const float windSpeedMin = 0;     // Wind speed in meters/sec corresponding to minimum voltage
  const float voltageMax = 2.0;     // Maximum output voltage from anemometer in mV.
  const float windSpeedMax = 32.4;  // Wind speed in meters/sec corresponding to maximum voltage


  /****************************************/
  /* WindDirection Sensor  0-5V           */
  /****************************************/
  const int windDirectionPin = A2;
  float windDirection = 0;

  /****************************************/
  /* Solar Radiation Sensor  0-3V         */
  /****************************************/
  const int solarRadiationPin = A4;
  float solarRad = 0;
  const float refTemp = 25.0;

  /****************************************/
  /* UV Radiation Sensor  0-3V            */
  /****************************************/
  const int uvRadiationPin = A6;
  float uvRad = 0;

  /****************************************/
  /* Read the FAN current Sensor          */
  /****************************************/
  const int fanCurrentPin = A8;
  int currentLow = 10;
  int currentHigh = 227;
  float fanCurrent = 0;
  
  
  /****************************************/
  /* Davis Rain Sensor                    */
  /****************************************/
  const byte rainInterruptPin = 2;
  byte rainCount = 0;
  
  /****************************************/
  /* SHT25 Air T&H Sensor                 */
  /****************************************/
  float tempAir;
  float RHAir;

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array
  
  uint8_t payload[27];               // array of length 27, 0-26, to send the sensor values as a nmea string
  // SH + SL Address of receiving XBee (Coordinator)
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40f748cb); // Coordinator Address
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  ZBTxStatusResponse txStatus = ZBTxStatusResponse();

  // union to convert float to byte string
  union u_tag {
    uint8_t b[4];
    float fval;
  } u;

  /****************************************/
  /* Hardware Declarations                */
  /****************************************/
  const byte fanPin = 5;             // D5
  const byte errorPin = 52;          // D52

  /****************************************/
  /* Statistics Declarations              */
  /****************************************/
  Statistic windSpeedStats;
  Statistic windDirectionStats;
  Statistic tempAirStats;
  Statistic RHAirStats;
  Statistic solarRadiationStats;
  Statistic UVRadiationStats;
  float avgWindSpeed;
  float avgWindDirection;
  float avgTempAir;
  float avgRHAir;
  float avgSolarRad;
  float avgUVRad;
  
  byte statsUpdated = 0;
  byte error = B00000000; // 1 = SHT error, 2 = Fan Error


  /****************************************/
  /* State Machine Declarations           */
  /****************************************/
  byte state = 0;
  byte counter = 0;
  #define REQUEST_RATE 6000          // in milliseconds - Sample rate, 6000 default (6s)
  unsigned long lastupdate = 0;      // timer value when last measure update was done


/**********************************************************************
 *
 * Setup routine
 *
 *********************************************************************/
  void setup(void) {
    configHardware();           // Separate routine for all the configs
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
