void calcSun() {
  calcET();
  calcUV();
  calcSunHours();
  calcMaxSolar();
  }

  /****************************************/
  /* Calculation of Max Solar Irradiance  */
  /****************************************/
void calcMaxSolar() {
  if(solarRad > maxSolarRad) {
      maxSolarRad = solarRad;
    }
}

/********************************************
 * Calculate the hours of sun today         *
 * Defined as ratio Rs/Rso > 75%            *
 ********************************************/
void calcSunHours() {
  double Rs = solarRad;
  double tmp1 = 0;
  Csr = calcClearSkyRad();
  if(Csr != 0) {
    rdif = (Rs/Csr)*100;

    if(rdif > 75) sunHoursMinCounter++;
    tmp1 = sunHoursMinCounter;
    sunHoursDec = tmp1/60.0;
    if(sunHoursMinCounter < 60) {
      hSunHours = 0;
      mSunHours = sunHoursMinCounter;
    }
    else {
      hSunHours = sunHoursMinCounter/60;
      mSunHours = sunHoursMinCounter - (hSunHours*60);
    }
  }
  else rdif = 0;
}

double calcClearSkyRad() {
  double t = hour;
  t *= 60;
  t += minute;
  t -= sunOffset;
  t /= 60;
  int J = calculateDayOfYear();
  double phi_rad = PI/180.0 * latitude;
  double d = (23.45*PI/180.0) * sin((2*PI*J/365.0)-1.39); // rad
  double w = calc_omega(t, J);

  double es = calc_es(tempAir);
  double ea = calc_ea(es, RHAir);
  double P = calc_atmos_pres(altitude_sealevel, tempAir);
  double wAir = calc_w(ea, P);
  
  double Kb = calc_Kb(d, w, wAir, phi_rad, P);
  double Kd = 0.35 - (0.36*Kb);

  double E0 = 1 + (0.033 * cos(2*PI*J/365.0)); // rad
  double Ra = Gsc*E0*((sin(phi_rad)*sin(d)) + (cos(phi_rad) * cos(d) * cos(w)));
  double Rso = (Kb + Kd) * Ra;
  Rso /= 0.0036;
  return Rso;
}

/********************************************
 * Calculate the Evapotranspiration         *
 * Following the Penman-Monteith algorithm  *
 ********************************************/
void fillArrayMeanRadTRHW() {
    radArray[radTRHWSArrayCounter] = solarRad;      // W/m2
    TArray[radTRHWSArrayCounter] = tempAir;         // °C
    RHArray[radTRHWSArrayCounter] = RHAir;          // %
    wsArray[radTRHWSArrayCounter] = windSpeed/3.6;  // m/s
    radTRHWSArrayCounter++;
}

void calcMeanRadTRHW() {
  for (byte i = 0; i<60; i++) {
      Rmean += *(radArray + i);
    }

    for (byte i = 0; i<60; i++) {
      Tmean += *(TArray + i);
    }

    for (byte i = 0; i<60; i++) {
      RHmean += *(RHArray + i);
    }

    for (byte i = 0; i<60; i++) {
      WSmean += *(wsArray + i);
    }

    Rmean /= 60;  // 1h
    Tmean /= 60;  // 1h
    RHmean /= 60;  // 1h
    WSmean /= 60;  // 1h
}

// Calculate the Reference Evapotranspiration for Grass
// Calculated every hour, and a dailty total is made
void calcET() {
  fillArrayMeanRadTRHW();
  if(radTRHWSArrayCounter == 60){
    radTRHWSArrayCounter = 0;
    calcMeanRadTRHW();

    // Change value of Cd in function of daytime/nighttime
    float Cd = 0;
    if(Rmean > 0) {
      Cd = 0.24;
    }
    else Cd = 0.96;

    // Solar Radiation hourly mean, STEP 2
    double Rs = Rmean * 0.0036;  // W/m2 to MJ/m2/h

    // Hourly Mean wind speed (m/s), corrected to 2m, STEP 3
    double u2 = calc_ws2m(windmeter_altitude);

    // Mean Saturation Vapor Pressure, derived from Tmean (kPa), STEP 10
    double es = calc_es(Tmean);

    // Actual Vapor Pressure, derived from RHmean (kPa), STEP 11
    double ea = calc_ea(es, RHmean);

    // Slope of Saturation Vapor Pressure, STEP 4
    double delta = calc_delta(es);

    // Atmospheric Pressure (kPa), STEP 5
    double P = calc_atmos_pres(altitude_sealevel, Tmean);

    // Psychrometric Constant, latent heat of vaporization, STEP 6
    double lambda = 2.501 - (0.002361 * Tmean);
    double psi = (0.00163 / lambda) * P;

    // Delta Term, STEP 7
    double DT = delta / (delta + (psi*(1 + (Cd*u2))));

    // Psi Term, STEP 8
    double PT = psi / (delta + (psi*(1 + (Cd*u2))));

    // Temperature Term, STEP 9
    double TT = (Cn/(Tmean + 273.16)) * u2;

    // Solar Declination, STEP 12
    int J = calculateDayOfYear();
    double d = (23.45*PI/180.0) * sin((2*PI*J/365.0)-1.39); // rad

    // Latitude in Radians, STEP 13
    double phi_rad = PI/180.0 * latitude;

    // Hour Angle, STEP 14
    double t = hour;
    t -= ((double)sunOffset/60.0);
    t -= 0.5;  // solar angle at midpoint of the hourly angle
    double w = calc_omega(t, J);
    double w1 = w - (PI/24);
    double w2 = w + (PI/24);

    // Extraterrestrial radiation, STEP 15
    double E0 = 1 + (0.033 * cos(2*PI*J/365.0)); // rad
    double Ra = (12/PI)*Gsc*E0*(((w2-w1)*sin(phi_rad)*sin(d)) + (cos(phi_rad) * cos(d) * (sin(w2) - sin(w1))));

    double wAir = calc_w(ea, P);
    double Kb = calc_Kb(d, w, wAir, phi_rad, P);
    double Kd = 0.35 - (0.36*Kb);
    
    // Clear Sky Radiation, STEP 16
    double Rso = (Kb + Kd) * Ra;

    // Net Solar or Net Shortwave Radiation, STEP 17
    double Rns = 0.77 * Rs;

    // Net Outgoing long wave solar Radiation, STEP 18
    double epsilon = (0.34 - (0.14 * sqrt(ea)));  // Net Emissivity of the Surface
    double relCloud = Rs/Rso;
    if(relCloud < 0.3) relCloud = 0.3;
    if(relCloud > 1.0) relCloud = 1.0;
    if(isnan(relCloud)) relCloud = 0.5; // happens during the night as Rso = nan
    double cloudiness = ((1.35 * relCloud) - 0.35); // Cloudiness Factor
    double Rnl = boltzmann * pow((Tmean + 273.16), 4) * epsilon * cloudiness;

    // Net Radiation, STEP 19
    double Rn = Rns - Rnl;
    double Rng = 0.408 * Rn;  // express net radiation in eq of evaporation

    // Final step, Radiation Term + Wind Term
    double ETRad = DT * Rng;
    double ETWind = PT * TT * (es - ea);
    EThour = ETWind + ETRad;
    if (EThour < 0.0) EThour = 0.0;
    ETday += EThour; 
  }
}


double calc_es(double temp) {
  double tmp1 = (17.27 * temp) / (temp + 237.3);
  double es = 0.6108 * exp(tmp1);
  return es;
}

double calc_ea(double es, double RH) {
  double ea = es * (RH/100.0);
  return ea;
}

double calc_delta(double es) {
  double tmp1 = 4098.0 * es;
  double delta = tmp1 / pow((Tmean + 237.3), 2);
  return delta;
}

double calc_ws2m(int z) {
  double tmp1 = (67.8 * z) - 5.42;
  double ws2m = WSmean * (4.87 / log(tmp1));
  return ws2m;
}

double calc_atmos_pres(int alt, double temp) {
  double tmp1 = ((273.16+temp) - (0.0065 * alt)) / (273.16+temp);
  double tmp2 = pow(tmp1, (9.807/(0.0065*287)));
  double atmos_pres = 101.3 * tmp2;
  return atmos_pres;
}

double calc_omega(double t, int J) {
  double b = (2*PI/365.0) * (J-81);
  double EoT = 0.1723*sin(2*b) - 0.1314*cos(b) - 0.026*sin(b); // Equation of Time, rad
  double Tcf = 4*(Lstm - longitude) + (EoT*180.0/PI); // Time Correction Factor, minutes
  double Lst = t + (Tcf / 60.0); // Local Solar Time
  double w = (PI/12.0)*(Lst - 12); // Hour Angle
  return w;
}

double calc_w(double ea, double P) {
  double w = (0.14*ea*P) + 2.1;
  return w;
}

double calc_Kb(double d, double w, double wAir, double phi_rad, double P) {
  double sinPhi = (sin(phi_rad)*sin(d)) + (cos(phi_rad) * cos(d) * cos(w));
  double Kb = 0.98 * exp((-0.00146*P/sinPhi)-(0.075*pow((wAir/sinPhi),0.4)));
  return Kb;
}

int calculateDayOfYear() {
  
  int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  
  // Check if it is a leap year, this is confusing business
  if (year%4  == 0) {
    if (year%100 != 0) {
      daysInMonth[1] = 29;
    }
    else {
      if (year%400 == 0) {
        daysInMonth[1] = 29;
      }
    }
   }
  
  int doy = 0;
  for (int i = 0; i < month - 1; i++) {
    doy += daysInMonth[i];
  }
  
  doy += day;
  return doy;
}

  /****************************************/
  /* Calculation of Max UV - 24h          */
  /* Calculation of 10m average UV        */
  /****************************************/
void calcUV() {
  calcUVMax();
  calcUVAvg();
}
void calcUVMax() {
  if(uvIndex > maxUV) {
      maxUV = uvIndex;
    }
}

void calcUVAvg() {
  uvArray[uvArrayCounter] = uvIndex;
  uvArrayCounter++;
  
  if(uvArrayCounter == 10) {
    uvArrayCounter = 0;
  }
  avgUV10m = 0;
  for (byte i = 0; i<10; i++) {
    avgUV10m += *(uvArray + i); // = uvArray[i]
  }
  avgUV10m /= 10;
}
