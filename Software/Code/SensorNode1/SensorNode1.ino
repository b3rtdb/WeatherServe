/*
 * SHT25: I²C (0x40), D21 (SCL), D20 (SDA) [3V3 supply via converter]
 * WINDSPEED: D3 [5V supply] (interrupt 5)
 * WINDDIRECTION: A2 [5V supply]
 * SOLAR RADIATION: A4 [3V supply]
 * UV SENSOR: A6 [3V supply]
 * INA219: I²C (0x41), D21 (SCL), D20 (SDA) [5V supply]
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

  /****************************************/
  /* Windspeed Sensor                     */
  /****************************************/
  const byte windSpeedInterruptPin = 3;
  volatile unsigned long rotations = 0; // cup rotation counter used in interrupt routine 
  float windSpeed, windGust = 0.0;

  /****************************************/
  /* WindDirection Sensor                 */
  /****************************************/
  const int windDirectionPin = A2;
  float windDir = 0.0;
  byte windDirArrayCounter = 0;

  /****************************************/
  /* Solar Radiation Sensor  0-3V         */
  /****************************************/
  const int solarRadiationPin = A4;
  float solarRad = 0.0;
  const float refTemp = 25.0;

  /****************************************/
  /* UV Radiation Sensor  0-3V            */
  /****************************************/
  const int uvRadiationPin = A6;
  float uvRad = 0.0;

  /****************************************/
  /* Read the FAN Power Sensor            */
  /****************************************/
  Adafruit_INA219 ina219(0x41); // I2C address changed by jumper, normally has same address as SHT25
  int loadCurrent = 0;          // when fan is normally running, should be around 80mA
  const int currentLow = 40;    // below 40mA, fan is not running
  const int currentHigh = 120;  // above 120mA, fan is blocked
  float loadVoltage = 0.0;
  const float voltageLow = 6.0; // below 6V or above 7V, something is wrong with the regulator (should be 6.5V)
  const float voltageHigh = 7.0;
  
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
  /* Hardware Declarations                */
  /****************************************/
  const byte fanPin = 5;             // D5
  const byte errorPin = 52;          // D52

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
