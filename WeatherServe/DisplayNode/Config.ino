void configNode() {
  //Serial.begin(115200);
  Serial1.begin(9600);    // xBee
  xbee.begin(Serial1);
  vl6180.begin();         // ToF sensor
  delay(50);
  CleO.begin();
  delay(500);

  /* Set screen coordinate in landscape mode */
  CleO.DisplayRotate(2, 0);
  int16_t h = CleO.LoadImageFile("Pictures/bgWeather.png", 0);
  CleO.SetBackgroundImage(h, TILED);
  refreshDisplay();
}

