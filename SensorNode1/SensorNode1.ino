/*
 * SHT25: I²C, D21 (SCL), D20 (SDA) [3V3 supply via converter]
 * WINDSPEED: D3 [5V supply] (interrupt 5)
 * WINDDIRECTION: A2 [5V supply]
 * SOLAR RADIATION: A4 [3V supply]
 * UV SENSOR: A6 [3V supply]
 * FAN CURRENT SENSOR: I²C, D21 (SCL), D20 (SDA) [5V supply]
 * RAIN SENSOR: D2 (interrupt 4)
 * FAN FARS: D5 ON/OFF control [6V5 supply]
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
  volatile unsigned long Rotations = 0; // cup rotation counter used in interrupt routine 
  volatile unsigned int TimerCount = 0; // used to determine 2.5sec timer count
  volatile bool windSampleRequired = false;
  float windSpeedMph, windSpeed, windGust = 0.0;
  const float speedConversion = 1.60934; // MPH -> KMH

  /****************************************/
  /* WindDirection Sensor                 */
  /****************************************/
  const int windDirectionPin = A2;
  float windDir = 0.0;

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
  Adafruit_INA219 ina219;
  int loadCurrent = 0; // when fan is normally running, should be 66mA
  int currentLow = 30; // below 30mA, fan is not running
  int currentHigh = 100; // above 100mA, fan is blocked
  float loadvoltage = 0.0;
  float voltageLow = 6.0; // below 6V or above 7V, something is wrong with the regulator (should be 6.5V)
  float voltageHigh = 7.0;
  
  /****************************************/
  /* Davis Rain Sensor                    */
  /****************************************/
  const byte rainInterruptPin = 2;
  volatile byte rainCount = 0;
  byte windDirArrayCounter = 0;
  
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
  int windDirArray[24];
  float avgWindSpeed;
  float avgWindDir;
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
  #define REQUEST_RATE 2500          // in milliseconds - Sample rate, 2500 default (2,5s)
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
    
    if (counter > 24) {  // transition from state 2 -> 3 (24x2,5 = 60sec)
      state = 3;
    }
    
    if ( ( millis()-lastupdate ) > REQUEST_RATE ) { // transition from state 2 -> 1
      state = 1;
    }

    if(windSampleRequired) { 
      windSpeedMph = Rotations * 0.9; // V (mph) = P(2.25/2.5) = P * 0.9
      noInterrupts(); //Rotations is set by interrupt, so interrupt needs to be disabled to read it
      Rotations = 0; // Reset count for next sample
      interrupts();
      windSampleRequired = false;
    }
  }
