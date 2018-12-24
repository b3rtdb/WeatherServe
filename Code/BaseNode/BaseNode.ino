/********************************************
 *                                          *
 * LIBRARIES                                *
 *                                          *
 ********************************************/
#include <XBee.h>
#include <Process.h>
#include <DueTimer.h>

  /****************************************/
  /* Xbee Declarations                    */
  /****************************************/ 
  XBee xbee = XBee();
  ZBRxResponse rx = ZBRxResponse();
  uint8_t payload[76];                                                    /* array of length 74, 0-73, to broadcast all values */
  XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x40A9C935);               /* SH + SL Address of receiving XBee = CLEO  // BROADCAST ADDRESS (0x00000000, 0x0000FFFF) */
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

  int arrayOffsetRX, arrayOffsetTX = 0;
  byte errorWSN1, errorWSN2 = 0B00000000;
    /* errorWSN1: 1 = SHT Error, 2 = Fan current Error, 3 = Fan voltage Error */
    /* errorWSN2: 1 = LPS25HB Error, 2 = PMSensor Error, 4 = AS3935 Tuning NOK, 8 = AS3935 Noise level too high, 16 = AS3935 Disturber detected */

  
  // union to convert float to byte string
  union u_tag {
    uint8_t b[4];
    float fval;
  } u;


  /****************************************/
  /* System declarations                  */
  /****************************************/
  volatile unsigned int timerCount = 0; // used to determine 60sec timer count
  volatile bool refreshData, checkXbeeRx, checkWsn = false; 
  unsigned long currentMillis, lastupdateWSN1, lastupdateWSN2 = 0;           /* timer value when last measure update was done of WSNx */
  byte onlineFlagWSN1, onlineFlagWSN2 = 0;                                   /* 0 = undefined, 1 = offline, 2 = online */
  int year, month, day, hour, minute, seconds, DST = 0;
  Process date;

/****************************************/
/*                                      */
/* Variable declarations                */
/*                                      */
/****************************************/

  /****************************************/
  /* Received Variables                   */
  /****************************************/
  byte lightningDetected,lightningDistance,rainSensorCount = 0;
  float tempAir,RHAir,windSpeed,windGustRec,uvIndex = 0.0;
  unsigned long pressure = 0;
  int windDir,PM01,PM25,PM10,solarRad = 0;

  /****************************************/
  /* Calculated Variables                 */
  /****************************************/
  float minTempAir,maxTempAir,windGust,avgWindSpeed10m,avgPm10_24h,avgPm25_24h,totalRain24h,rainIntensity, apparentT, dewPoint, maxUV, avgUV10m, sunHoursDec, windRunKm = 0.0;
  byte totalRain24hTicks,rainArrayCounter,windDirArrayCounter,windSpeedArrayCounter,uvArrayCounter,windSpeedCounter,pressureArrayCounter,pmArrayCounter,pmCounter,radTRHWSArrayCounter,trend,moonPhaseNumber, zambrettiNumber, pressure3hCounter = 0;
  signed long pdif = 0;
  int mRising, mSetting, hRising, hSetting, hDaylength, mDaylength, sunHoursMinCounter, maxSolarRad, windRun, avgWindDir = 0;
  double moonPhase, moonAge, Rmean, Tmean, RHmean, WSmean, ETday, EThour, rdif, Csr = 0;
  
  byte rainArray[15];
  int pm25Array[144], pm10Array[144];
  unsigned long pressureArray[8];
  float windSpeedArray[10], uvArray[10], radArray[60], TArray[60], RHArray[60], wsArray[60];
  int windDirArray[60];

  /****************************************/
  /* Constants for Calculations           */
  /****************************************/
  #define abs(x) ((x) < 0 ? (-(x)) : (x))                   /* Absolute val */
  #define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle    */
  #define torad(d) ((d) * (PI / 180.0))                     /* Deg->Rad   */
  #define todeg(d) ((d) * (180.0 / PI))                     /* Rad->Deg   */
  const float epoch = 2444238.5;                            /* 1980 January 0.0 */
  const float elonge = 278.833540;                          /* Ecliptic longitude of the Sun at epoch 1980.0 */
  const float elongp = 282.596403;                          /* Ecliptic longitude of the Sun at perigee */
  const float eccent = 0.016718;                            /* Eccentricity of Earth's orbit */
  const float mmlong = 64.975464;                           /* Moon's mean lonigitude at the epoch */
  const float mmlongp = 349.383063;                         /* Mean longitude of the perigee at the epoch */
  const float mlnode = 151.950429;                          /* Mean longitude of the node at the epoch */
  const float minc = 5.145396;                              /* Inclination of the Moon's orbit */
  const float synmonth = 29.53058868;                       /* Synodic month (new Moon to new Moon) */
  const byte LutRising[14] = {1,2,2,3,6,7,9,10,12,13,13,17,20,25};
  const byte LutFalling[10] = {2,4,8,15,18,21,22,24,24,26};
  const byte LutSteady[17] = {1,2,2,2,5,11,14,14,16,16,19,23,23,24,24,24,26};
  const int zambrettiPressCorrArray[16] = {520,420,320,105,-110,-315,-520,-835,-1150,-940,-730,-525,-320,-115,90,305}; /* wind correction for pressure in Pascal */
  const double boltzmann = 2.042E-10;                       /* constant of boltzmann, MJ/m2/h/K4 */
 
  /****************************************/
  /* Variables declared as constants      */
  /****************************************/
  const byte sunOffset = 24;                    /* 24min? */
  const unsigned long baroTop = 104080;
  const unsigned long baroBottom = 98230;
  const int windmeter_altitude = 4;             /* 4 meter altitude measurement  */
  const int altitude_sealevel = 7;
  const float Gsc = 4.92;                       /* Solar constant MJ/m2/h */
  const float Cn = 37.5;                        /* numerator constant for reference crop */
  const signed int pressCorrection = -645;      /* pressure correction in Pascal */
  const int windDirOffset = 0;                  /* Wind Direction correction in Degrees (WSN1 station rotation to true North) */
  const unsigned long onlineRate = 120000UL;    /* in milliseconds, 2min, every Sensornode sends data every minute, so if it does not respond after 2 minutes, it's offline */
  const double Lstm = -15;                      /* Local Standard Time Meridian, degr west */
  const double latitude = 51.0406778;           /* coordinates of Schorsemolenstraat 75, Sint-Katelijne-Waver */
  const double longitude = 4.5093139;

/**********************************************************************
 *
 * Setup routine
 *
 *********************************************************************/
void setup() {
  configDue();
}

/**********************************************************************
 *
 * Main loop
 *
 *********************************************************************/
void loop() {

  /****************************************/
  /* Check Xbee Rx every 1s               */
  /****************************************/
  if(checkXbeeRx) {
    checkXbeeRx = false;
    getXbeeData();
  }

  /****************************************/
  /* Check Online Status WSN1, WSN2       */
  /****************************************/
  if(checkWsn) {
    checkWsn = false;
    currentMillis = millis();
    if ((unsigned long)(currentMillis - lastupdateWSN1) >= onlineRate ) {
      onlineFlagWSN1 = 1;
    }
   
    if ((unsigned long)(currentMillis - lastupdateWSN2) >= onlineRate ) {
      onlineFlagWSN2 = 1;
    }
  }


  /****************************************/
  /* Refresh values every minute (Timer3) */
  /****************************************/
  if (refreshData) {
    refreshData = false;
    getTimeDate();
    calcTempHum();
    calcWind();
    calcRain();
    calcAirQuality();
    calcPrediction();
    calcSun();
    calcSunMoon();
    xbeeTx();
    postToInflux();
  }
}
