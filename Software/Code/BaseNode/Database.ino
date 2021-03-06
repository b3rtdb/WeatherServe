/********************************************
 * Send data every minute to an             *
 * Influx DB with Grafana                   *
 ********************************************/
void postToInflux() {
  postdb(localdb);
  //postdb(remotedb);
}

void postdb(String dblocation) {
  calcEThr();
  Process p;
  String cmd;
  cmd = dblocation;
  cmd += "'tempAir value=" + String(tempAir) + "\n";
  cmd += "RHAir value=" + String(RHAir) + "\n";
  cmd += "apparentT value=" + String(apparentT) + "\n";
  cmd += "dewPoint value=" + String(dewPoint) + "\n";
  cmd += "windSpeed value=" + String(windSpeed) + "\n";
  cmd += "windDir value=" + String(windDir) + "\n";
  cmd += "avgWindDir value=" + String(avgWindDir) + "\n";
  cmd += "pressure value=" + String(pressure) + "\n";
  cmd += "PM25 value=" + String(avgPM25) + "\n";
  cmd += "PM10 value=" + String(avgPM10) + "\n";
  cmd += "currentRain value=" + String(currentRain) + "\n";
  cmd += "rainIntensity value=" + String(rainIntensity) + "\n";
  cmd += "solarRad value=" + String(solarRad) + "\n";
  cmd += "uvIndex value=" + String(uvIndex) + "\n";
  cmd += "ClearSkyRad value=" + String(Csr) + "\n";
  cmd += "windGustRec value=" + String(windGustRec) + "\n";
  cmd += "sunMoment value=" + String(sunMoment) + "\n";
  cmd += "EThour value=" + String(EThour) + "\n";
  cmd += "EThr value=" + String(EThr) + "\n";
  cmd += "avgNO2 value=" + String(avgNO2) + "\n";
  cmd += "avgO3 value=" + String(avgO3) + "'";
  p.runShellCommand(cmd);
  p.close();
}
