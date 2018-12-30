/********************************************
 * Send data every minute to an             *
 * Influx DB with Grafana                   *
 ********************************************/
void postToInflux() {
  local();
  remote();
}

void local() {
  Process p;
  String cmd;
  cmd = "curl -i -XPOST 'http://10.69.20.25:8086/write?db=WeatherServe&precision=s' --data-binary ";
  cmd += "'tempAir value=" + String(tempAir) + "\n";
  cmd += "RHAir value=" + String(RHAir) + "\n";
  cmd += "apparentT value=" + String(apparentT) + "\n";
  cmd += "dewPoint value=" + String(dewPoint) + "\n";
  cmd += "sunHoursDec value=" + String(sunHoursDec) + "\n";
  cmd += "windSpeed value=" + String(windSpeed) + "\n";
  cmd += "windDir value=" + String(windDir) + "\n";
  cmd += "avgWindDir value=" + String(avgWindDir) + "\n";
  cmd += "pressure value=" + String(pressure) + "\n";
  cmd += "avgPm25_24h value=" + String(avgPm25_24h) + "\n";
  cmd += "avgPm10_24h value=" + String(avgPm10_24h) + "\n";
  cmd += "PM25 value=" + String(PM25) + "\n";
  cmd += "PM10 value=" + String(PM10) + "\n";
  cmd += "rainIntensity value=" + String(rainIntensity) + "\n";
  cmd += "totalRain24h value=" + String(totalRain24h) + "\n";
  cmd += "solarRad value=" + String(solarRad) + "\n";
  cmd += "uvIndex value=" + String(uvIndex) + "\n";
  cmd += "ClearSkyRad value=" + String(Csr) + "\n";
  cmd += "windGustRec value=" + String(windGustRec) + "\n";
  cmd += "ETday value=" + String(ETday) + "'";
  p.runShellCommand(cmd);
  p.close();
}

void remote() {
  Process p;
  String cmd;
  cmd = "curl -i -XPOST 'https://corlysis.com:8086/write?db=WeatherServe' -u token:d639197c7aa51d7bd803d62f7b15e46a --data-binary ";
  cmd += "'tempAir value=" + String(tempAir) + "\n";
  cmd += "RHAir value=" + String(RHAir) + "\n";
  cmd += "apparentT value=" + String(apparentT) + "\n";
  cmd += "dewPoint value=" + String(dewPoint) + "\n";
  cmd += "sunHoursDec value=" + String(sunHoursDec) + "\n";
  cmd += "windSpeed value=" + String(windSpeed) + "\n";
  cmd += "windDir value=" + String(windDir) + "\n";
  cmd += "avgWindDir value=" + String(avgWindDir) + "\n";
  cmd += "pressure value=" + String(pressure) + "\n";
  cmd += "avgPm25_24h value=" + String(avgPm25_24h) + "\n";
  cmd += "avgPm10_24h value=" + String(avgPm10_24h) + "\n";
  cmd += "PM25 value=" + String(PM25) + "\n";
  cmd += "PM10 value=" + String(PM10) + "\n";
  cmd += "rainIntensity value=" + String(rainIntensity) + "\n";
  cmd += "totalRain24h value=" + String(totalRain24h) + "\n";
  cmd += "solarRad value=" + String(solarRad) + "\n";
  cmd += "uvIndex value=" + String(uvIndex) + "\n";
  cmd += "ClearSkyRad value=" + String(Csr) + "\n";
  cmd += "windGustRec value=" + String(windGustRec) + "\n";
  cmd += "ETday value=" + String(ETday) + "'";
  p.runShellCommand(cmd);
  p.close();
}
