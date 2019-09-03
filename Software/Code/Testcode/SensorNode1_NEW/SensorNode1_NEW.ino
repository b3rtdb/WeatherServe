/*
 * SHT25: I²C (0x40), D21 (SCL), D20 (SDA) [3V3 supply via converter]
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
  
  // Wind direction, CH0
  float windDir = 0.0;
  byte windDirArrayCounter = 0;

  // Solar Radiation 0-3V, CH1
  float solarRad = 0.0;
  const float refTemp = 25.0;

  // UV Radiation 0-2.5V, CH2
  float uvRad = 0.0;

  
  /****************************************/
  /* Davis Rain Sensor                    */
  /****************************************/
  const byte rainInterruptPin = 2;
  volatile byte rainCount = 0;
  
  /****************************************/
  /* SHT25 Air T&H Sensor                 */
  /****************************************/
  float tempAir;
  float RHAir;


  // union to convert float to byte string
  union u_tag {
    uint8_t b[4];
    float fval;
  } u;


  /****************************************/
  /* System Declarations                  */
  /****************************************/
  byte counter = 0;
  volatile byte state = 0;
  volatile unsigned int timerCount = 0; // used to determine 2.5sec timer count

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
    requestSensorData();
    delay(5000);
  }
