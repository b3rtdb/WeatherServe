/********************************************
 *                                          *
 * LIBRARIES                                *
 *                                          *
 ********************************************/
#include <CleO.h>
#include <XBee.h>
#include <Adafruit_VL6180X.h>

  Adafruit_VL6180X vl6180 = Adafruit_VL6180X();

  /****************************************/
  /* Zigbee declarations                  */
  /****************************************/ 
  XBee xbee = XBee();
  XBeeResponse response = XBeeResponse();
  ZBRxResponse rx = ZBRxResponse();
  
  // union to convert float to byte string
  union u_tag {
    uint8_t b[4];
    float fval;
  } u;

  /****************************************/
  /* Variable declarations                */
  /****************************************/ 
  byte nodeIdent, arrayOffset, AQI10, AQI25, AQIuv, RIR, zambrettiNumber, moonPhaseNumber, moonPhase, hRising, hSetting, mRising, mSetting, trend, errorWSN1, errorWSN2 = 0;
  float floatStore, tempAir, RHAir, RHGnd, windSpeed, windDir, pressure, tempFeelslike, solarRad, minTempAir, maxTempAir, maxWindSpeed, avgWindDir, avgPm10_24h, avgPm25_24h, totalRain24h, rainIntensity, uvIndex, ETday, windRunKm, sunHoursDec= 0.0;
  double moonAge = 0;
  int intStore = 0;

  /****************************************/
  /* Display declarations                 */
  /****************************************/
  const int r0 = 55;
  const static uint32_t colors[10] = {0x0000FF, 0x007EFD, 0x009800, 0x00FF00, 0xCCFF33, 0xF8E748, 0xFF8000, 0xFF0000, 0xC00000, 0x5A0000};  // Colors AQI
  const static uint32_t colors2[12] = {0x1687F8, 0xFC3325, 0xC3C3C3, 0x00FF00, 0xB97A57, 0xFF8000, 0x65B5FE, 0xEBF501, 0xFFFFFF, 0xA349A4, 0xFFEEA3, 0xFFCB27}; // Colors other values
  const static uint32_t colors3[5] = {0x48A517, 0xFFFE36, 0xE9A82A, 0xDB3D1F, 0xD58AEC};  // colors UV
  byte colorIndex25, colorIndex10;
  const byte mes1Cx = 67;
  const byte mes2Cx = 111;
  const byte mes3Cx = 155;
  const byte mes1Cy = 238;
  const int mes2Cy = 263;
  const int mes3Cy = 288;
  const int windCx = 295;
  const byte windCy = 177;
  const byte lengthOfFloat = 4;

  /****************************************/
  /* System declarations                  */
  /****************************************/
  const unsigned long REFRESH_RATE = 30000UL;    // in milliseconds, 30sec, time between Display refreshes
  unsigned long lastUpdateRefresh = 0;           // timer value when last display update was done
  const unsigned long BACKLIGHT_TIME = 45000UL;  // time backlight is on when waved (45sec)
  unsigned long backLightOnTimer = 0;            // timer value for the backlight when waved
  const unsigned long ONLINE_RATE = 300000UL;    // in milliseconds, 5min, Coordinator sends data every 1min, so after 5 minutes without data it's offline
  unsigned long lastUpdateDue = 0;               // timer value when last update was done of Due
  unsigned long currentMillis = 0;
  byte onlineFlagWSN1,onlineFlagWSN2,onlineFlagDue= 0;               // 0 = undefined, 1 = offline, 2 = online
  boolean waved = false;
  #define torad(d) ((d) * (PI / 180.0))       /* Deg->Rad   */
  #define synmonth    29.53058868    /* Synodic month (new Moon to new Moon) */
  #define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle    */
  

/**********************************************************************
 *
 * Setup routine
 *
 *********************************************************************/
void setup() {
  configNode();
}

/**********************************************************************
 *
 * Main loop
 *
 *********************************************************************/
void loop() {
  getXbeeData();
  checkWaved();

  if(waved) {
    switchOnBacklight();
  }

  /****************************************/
  /* check some timers                    */
  /****************************************/
  currentMillis = millis();
  if ((unsigned long)(currentMillis - backLightOnTimer) >= BACKLIGHT_TIME ) {
    switchOffBacklight();
  }

  if ((unsigned long)(currentMillis - lastUpdateDue) >= ONLINE_RATE ) {
    onlineFlagDue = 1;
  }

  if ((unsigned long)(currentMillis - lastUpdateRefresh) >= REFRESH_RATE ) {
    refreshDisplay();
  }
}
