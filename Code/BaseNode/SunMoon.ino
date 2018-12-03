/********************************************
 * Calculate Moonphase, Sunrise, Sunset     *
 ********************************************/
void calcSunMoon() {
  double JD=calcJD(year,month,day);

  mSetting = (int)calcSunsetUTC(JD, latitude, longitude);
  mSetting += sunOffset;
  mSetting += (DST*60);
  mDaylength = mSetting;
  hSetting = mSetting/60;
  mSetting -= (hSetting*60);
  
  mRising = (int)calcSunriseUTC(JD, latitude, longitude);
  mRising += sunOffset;
  mRising += (DST*60);
  mDaylength -= mRising;
  hRising = mRising/60;
  mRising -= (hRising*60);

  hDaylength = mDaylength/60;
  mDaylength -= (hDaylength*60);

  calcMoonPhase(calcJulianDay());
}

/********************************************
 * Calculate Moonphase and moonage          *
 ********************************************/
void calcMoonPhase(double pdate) {

  double  Day, N, M, Ec, Lambdasun, ml, MM, MN, Ev, Ae, A3, MmP,
    mEc, A4, lP, V, lPP, NP, y, x, BetaM;

  Day = pdate - epoch;      /* Date within epoch */
  N = fixangle((360 / 365.2422) * Day); /* Mean anomaly of the Sun */
  M = fixangle(N + elonge - elongp);  /* Convert from perigee
            co-ordinates to epoch 1980.0 */
  Ec = kepler(M, eccent);     /* Solve equation of Kepler */
  Ec = sqrt((1 + eccent) / (1 - eccent)) * tan(Ec / 2);
  Ec = 2 * todeg(atan(Ec));   /* True anomaly */
  
  Lambdasun = fixangle(Ec + elongp);  /* Sun's geocentric ecliptic longitude */

        /* Moon's mean longitude */
  ml = fixangle(13.1763966 * Day + mmlong);

        /* Moon's mean anomaly */
  MM = fixangle(ml - 0.1114041 * Day - mmlongp);

        /* Moon's ascending node mean longitude */
  MN = fixangle(mlnode - 0.0529539 * Day);

  /* Evection */
  Ev = 1.2739 * sin(torad(2 * (ml - Lambdasun) - MM));

  /* Annual equation */
  Ae = 0.1858 * sin(torad(M));

  /* Correction term */
  A3 = 0.37 * sin(torad(M));

  /* Corrected anomaly */
  MmP = MM + Ev - Ae - A3;

  /* Correction for the equation of the centre */
  mEc = 6.2886 * sin(torad(MmP));

  /* Another correction term */
  A4 = 0.214 * sin(torad(2 * MmP));

  /* Corrected longitude */
  lP = ml + Ev + mEc - Ae + A4;

  /* Variation */
  V = 0.6583 * sin(torad(2 * (lP - Lambdasun)));

  /* True longitude */
  lPP = lP + V;

  /* Corrected longitude of the node */
  NP = MN - 0.16 * sin(torad(M));

  /* Y inclination coordinate */
  y = sin(torad(lPP - NP)) * cos(torad(minc));

  /* X inclination coordinate */
  x = cos(torad(lPP - NP));

  /* Ecliptic latitude */
  BetaM = todeg(asin(sin(torad(lPP - NP)) * sin(torad(minc))));

  /* Age of the Moon in degrees */
  moonAge = lPP - Lambdasun;

  /* Phase of the Moon */
  moonPhase = (1 - cos(torad(moonAge))) / 2;
  moonAge = synmonth * (fixangle(moonAge) / 360.0);

  if(moonAge < 1.84566)  moonPhaseNumber = 1; //New Moon
  else if(moonAge < 5.53699)  moonPhaseNumber = 2; //Waxing Crescent
  else if(moonAge < 9.22831)  moonPhaseNumber = 3; //First Quarter
  else if(moonAge < 12.91963) moonPhaseNumber = 4; //Waxing Gibbous
  else if(moonAge < 16.61096) moonPhaseNumber = 5; //Full Moon
  else if(moonAge < 20.30228) moonPhaseNumber = 6; //Waning Gibbous
  else if(moonAge < 23.99361) moonPhaseNumber = 7; //Last Quarter
  else if(moonAge < 27.68493) moonPhaseNumber = 8; //Waning Crescent
  else moonPhaseNumber = 1; //New Moon
}

double kepler(double m, double ecc) {
  double e, delta;

#define EPSILON 1E-6

  e = m = torad(m);
  do {
    delta = e - ecc * sin(e) - m;
    e -= delta / (1 - ecc * cos(e));
  } while (abs (delta) > EPSILON);
  return e;
}

/********************************************
 * Calculate sunrise & sunset               *
 ********************************************/
//http://souptonuts.sourceforge.net/code/sunrise.c.html
double calcSunEqOfCenter(double t);


double  degToRad(double angleDeg) {
  return (M_PI * angleDeg / 180.0);
}

double radToDeg(double angleRad) {
  return (180.0 * angleRad / M_PI);
}


double calcMeanObliquityOfEcliptic(double t) {
  double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
  double e0 = 23.0 + (26.0 + (seconds/60.0))/60.0;
  return e0;              // in degrees
}


double calcGeomMeanLongSun(double t) {
  double L = 280.46646 + t * (36000.76983 + 0.0003032 * t);
  while( (int) L >  360 ) {
      L -= 360.0;
    }
  while(  L <  0) {
      L += 360.0;
    }
  return L;              // in degrees
}


double calcObliquityCorrection(double t) {
  double e0 = calcMeanObliquityOfEcliptic(t);
  double omega = 125.04 - 1934.136 * t;
  double e = e0 + 0.00256 * cos(degToRad(omega));
  return e;               // in degrees
}

double calcEccentricityEarthOrbit(double t) {
  double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
  return e;               // unitless
}

double calcGeomMeanAnomalySun(double t) {
  double M = 357.52911 + t * (35999.05029 - 0.0001537 * t);
  return M;               // in degrees
}

double calcEquationOfTime(double t) {
  double epsilon = calcObliquityCorrection(t);               
  double  l0 = calcGeomMeanLongSun(t);
  double e = calcEccentricityEarthOrbit(t);
  double m = calcGeomMeanAnomalySun(t);
  double y = tan(degToRad(epsilon)/2.0);
  y *= y;
  double sin2l0 = sin(2.0 * degToRad(l0));
  double sinm   = sin(degToRad(m));
  double cos2l0 = cos(2.0 * degToRad(l0));
  double sin4l0 = sin(4.0 * degToRad(l0));
  double sin2m  = sin(2.0 * degToRad(m));
  double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0 - 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;
  return radToDeg(Etime)*4.0; // in minutes of time
}

double calcTimeJulianCent(double jd) {
  double T = ( jd - 2451545.0)/36525.0;
  return T;
}

double calcSunTrueLong(double t) {
  double l0 = calcGeomMeanLongSun(t);
  double c = calcSunEqOfCenter(t);
  double O = l0 + c;
  return O;               // in degrees
}



double calcSunApparentLong(double t) {
  double o = calcSunTrueLong(t);
  double  omega = 125.04 - 1934.136 * t;
  double  lambda = o - 0.00569 - 0.00478 * sin(degToRad(omega));
  return lambda;          // in degrees
}

double calcSunDeclination(double t) {
  double e = calcObliquityCorrection(t);
  double lambda = calcSunApparentLong(t);
  double sint = sin(degToRad(e)) * sin(degToRad(lambda));
  double theta = radToDeg(asin(sint));
  return theta;           // in degrees
}


double calcHourAngleSunrise(double lat, double solarDec) {
  double latRad = degToRad(lat);
  double sdRad  = degToRad(solarDec);
  double HA = (acos(cos(degToRad(90.833))/(cos(latRad)*cos(sdRad))-tan(latRad) * tan(sdRad)));
  return HA;              // in radians
}

double calcHourAngleSunset(double lat, double solarDec) {
  double latRad = degToRad(lat);
  double sdRad  = degToRad(solarDec);
  double HA = (acos(cos(degToRad(90.833))/(cos(latRad)*cos(sdRad))-tan(latRad) * tan(sdRad)));
  return -HA;              // in radians
}

double calcJD(int year,int month,int day) {
  if (month <= 2) {
    year -= 1;
    month += 12;
  }
  int A = floor(year/100);
  int B = 2 - A + floor(A/4);
  double JD = floor(365.25*(year + 4716)) + floor(30.6001*(month+1)) + day + B - 1524.5;
  return JD;
}

double calcJDFromJulianCent(double t) {
  double JD = t * 36525.0 + 2451545.0;
  return JD;
}

double calcJulianDay() {

  long YY, MM, K1, K2, K3, JD;
  double corrFactorJD;

  if(hour < 12) {
    corrFactorJD = ((double)(12.0-hour)/24.0)*-1;
  }
  if(hour >= 12) {
    corrFactorJD = ((double)(hour-12.0)/24.0);
  }
                                      
  // calculate the Julian date at 12h UT
  YY = year - floor((12 - month) / 10);
  MM = month + 9;
  if(MM >= 12)
    MM = MM - 12;
  
  K1 = floor(365.25 * (YY + 4712));
  K2 = floor(30.6 * MM + 0.5);
  K3 = floor(floor((YY / 100) + 49) * 0.75) - 38;

  JD = K1 + K2 + day + 59;  // for dates in Julian calendar
  if(JD > 2299160)
    JD = JD - K3;          // for Gregorian calendar
    return JD + corrFactorJD;
}


double calcSunEqOfCenter(double t) {
    double m = calcGeomMeanAnomalySun(t);
    double mrad = degToRad(m);
    double sinm = sin(mrad);
    double sin2m = sin(mrad+mrad);
    double sin3m = sin(mrad+mrad+mrad);
    double C = sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;
    return C;   // in degrees
}


double calcSunriseUTC(double JD, double latitude, double longitude) {
  double  t = calcTimeJulianCent(JD);
  double  eqTime = calcEquationOfTime(t);
  double  solarDec = calcSunDeclination(t);
  double  hourAngle = calcHourAngleSunrise(latitude, solarDec);
  double  delta = longitude - radToDeg(hourAngle);
  double  timeDiff = 4 * delta; // in minutes of time 
  double  timeUTC = 720 + timeDiff - eqTime;  // in minutes 
  double  newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC/1440.0); 

  eqTime = calcEquationOfTime(newt);
  solarDec = calcSunDeclination(newt);
    
  hourAngle = calcHourAngleSunrise(latitude, solarDec);
  delta = longitude - radToDeg(hourAngle);
  timeDiff = 4 * delta;
  timeUTC = 720 + timeDiff - eqTime; // in minutes
  return timeUTC;
}

double calcSunsetUTC(double JD, double latitude, double longitude) {
  double  t = calcTimeJulianCent(JD);
  double  eqTime = calcEquationOfTime(t);
  double  solarDec = calcSunDeclination(t);
  double  hourAngle = calcHourAngleSunset(latitude, solarDec);
  double  delta = longitude - radToDeg(hourAngle);
  double  timeDiff = 4 * delta; // in minutes of time 
  double  timeUTC = 720 + timeDiff - eqTime;  // in minutes 
  double  newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC/1440.0); 

  eqTime = calcEquationOfTime(newt);
  solarDec = calcSunDeclination(newt);
  
  hourAngle = calcHourAngleSunset(latitude, solarDec);
  delta = longitude - radToDeg(hourAngle);
  timeDiff = 4 * delta;
  timeUTC = 720 + timeDiff - eqTime; // in minutes
  return timeUTC;
}
