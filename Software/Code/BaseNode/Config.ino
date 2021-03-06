void configDue() {
  Bridge.begin();
  
  Serial1.begin(9600);          // xBee
  xbee.begin(Serial1);

  getTimeDate();
  calcSunMoon();

  Timer3.attachInterrupt(refreshIRQ);
  Timer3.start(500000); // µs (0,5s)
}

/****************************************
 * Interrupt routine for Refresh        *
 * sensor data TIMER                    *
 ****************************************/
void refreshIRQ() {
  timerCount++;                     // Timer interrupt every 0,5s
  if((timerCount%2) == 0)  {        // Check Xbee data every 1s
    checkXbeeRx = true;
  }

  if((timerCount%10) == 0)  {        // Check WSNx online modus every 5s
    checkWsn = true;
  }
  
  if(timerCount == 120)  {          // Timer interrupt every 60 seconds  120 x 0,5s = 60s
    refreshData = true;
    timerCount = 0;
  }
}


/********************************************
 * Clear Statistics at midnight             *
 ********************************************/
void clearStatisticsMidnight() {
  minTempAir = tempAir;
  maxTempAir = tempAir;
  windGust = windGustRec;
  maxSolarRad = solarRad;
  maxUV = uvIndex;
  sunHoursTotal = 0;
  ETday = 0;
  windRun = 0;
  radTRHWSArrayCounter = 0;
  totalRain24hTicks = 0;
  rainArrayCounter = 0;
  windDirArrayCounter = 0;
  windSpeedArrayCounter = 0;
  uvArrayCounter = 0;
  pressure3hCounter = 0;
  pressureArrayCounter = 0;
  pmArrayCounter = 0;
}

/********************************************
 * Get Time & Date from YUN                 *
 ********************************************/
void getTimeDate() {
  int prevDay, summerWinter = 0;
  String command;
  prevDay = day;

  command = "+%Y";
  year = runTimeCommand(command);
  command = "+%m";
  month = runTimeCommand(command);
  command = "+%d";
  day = runTimeCommand(command);
  command = "+%H";
  hour = runTimeCommand(command);
  command = "+%M";
  minute = runTimeCommand(command);
  command = "+%z";
  summerWinter = runTimeCommand(command);
  if(summerWinter == 200){  // the return value is 0100 for winter (UTC+1) and 0200 for summer (UTC+2)
    DST = 1;
  }
  else DST = 0;
  
  if(prevDay != day){
      clearStatisticsMidnight();
      calcSunMoon();
    }
}

int runTimeCommand(String command) {
  int value = 0;
  String receivedString;
  if (!date.running()) {
    date.begin("date");
    date.addParameter(command);
    date.run();
    while (date.available()>0) {
      receivedString = date.readString();
      value = receivedString.toInt();
    }
  }
  return value;
}
