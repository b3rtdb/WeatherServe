/*
 * SHT25: I²C (0x40), D21 (SCL), D20 (SDA) [3V3 supply via converter] + D5 as Power Enable
 * WINDSPEED: D3 [5V supply] (interrupt 5)
 * WINDDIRECTION: ADC CH0 [5V supply]
 * SOLAR RADIATION: ADC CH1 [3V supply]
 * UV SENSOR: ADC CH2 [3V supply]
 * INA219: I²C (0x41), D21 (SCL), D20 (SDA) [3V3 supply]
 * RAIN SENSOR: D2 (interrupt 4)
 * XBEE: Serial1 on pins D19 (RX) and D18 (TX)
 * DEBUG: Serial on pins D0 (RX) and D1 (TX)
*/

#include <Wire.h>
#include <SHT2x.h>
#include <XBee.h>
#include <Statistic.h>
#include <TimerOne.h>
#include <Adafruit_INA219.h>
#include <SPI.h>
#include "MCP320X.h"

  /****************************************/
  /* Windspeed Sensor                     */
  /****************************************/
  const byte windSpeedInterruptPin = 3;
  volatile unsigned long rotations = 0; // cup rotation counter used in interrupt routine 
  float windSpeed, windGust = 0.0;

  /****************************************/
  /* MCP3204 ADC 4096mVref                */
  /****************************************/
  uint8_t channel = 0;
  const int PINSLAVE = 46;  // Port PL3
  const int offsetADC = 3;  // tied to GND, all channels have a RAW offset of +3 (measured)
  
  // Wind direction, CH0
  float windDir = 0.0;
  byte windDirArrayCounter = 0;

  // Solar Radiation 0-3V, CH1
  float solarRad = 0.0;
  const float refTemp = 25.0;

  // UV Radiation 0-2.5V, CH2
  float uvRad = 0.0;

  /****************************************/
  /* Read the FAN Power Sensor            */
  /****************************************/
  Adafruit_INA219 ina219(0x41);     // I2C address changed by jumper, normally has same address as SHT25
  float loadCurrent = 0.0;          // when fan is normally running, should be around 80mA
  const float currentLow = 50.0;    // below 50mA, fan is not running
  const float currentHigh = 130.0;  // above 130mA, fan is blocked
  float loadVoltage = 0.0;
  const float voltageLow = 6.3;    // below 6300mV or above 6800mV, something is wrong with the regulator or fan (should be 6500mV)
  const float voltageHigh = 6.8;
  
  /****************************************/
  /* Davis Rain Sensor                    */
  /****************************************/
  const byte rainInterruptPin = 2;
  volatile byte rainCount = 0;
  
  /****************************************/
  /* SHT25 Air T&H Sensor                 */
  /****************************************/
  const byte enableI2C = 5;
  float tempAir;
  float RHAir;

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/
  XBee xbee = XBee();
  byte arrayOffset = 1;              // used to shift values in the Tx array
  
  uint8_t payload[31];               // array of length 31, 0-30, to send the sensor values as a nmea string
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
  /* Statistics Declarations              */
  /****************************************/
  Statistic windSpeedStats;
  Statistic tempAirStats;
  Statistic RHAirStats;
  Statistic solarRadiationStats;
  Statistic UVRadiationStats;
  Statistic loadCurrentStats;
  Statistic loadVoltageStats;
  int windDirArray[24];
  float avgWindSpeed;
  float avgWindDir;
  float avgTempAir;
  float avgRHAir;
  float avgSolarRad;
  float avgUVRad;
  float avgLoadCurrent;
  float avgLoadVoltage;
  
  byte statsUpdated = 0;

  /****************************************/
  /* System Declarations                  */
  /****************************************/
  byte counter = 0;
  volatile byte state = 0;
  volatile unsigned int timerCount = 0; // used to determine 2.5sec timer count
  byte error = B00000000; // 1 = SHT error, 2 = Fan current Error, 3 = Fan voltage Error

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
      default: state=1; break;
    }
    
    if (counter > 24) {  // transition from state 2 -> 3 (24x2,5 = 60sec)
      state = 3;
    }
  }
