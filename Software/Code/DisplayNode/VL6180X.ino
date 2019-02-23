void checkWaved() {
  waved = false;
  uint8_t range = vl6180.readRange();
  
  if(range < 100){
    waved = true;
  }
}


void switchOnBacklight() {
  float lux = vl6180.readLux(VL6180X_ALS_GAIN_5);
  float displayBrightness = (lux/8);
  if (displayBrightness < 32) displayBrightness = 32;
  if (displayBrightness > 255) displayBrightness = 255;
  CleO.Display(displayBrightness);  // Backlight ON
  backLightOnTimer = millis();
}


void switchOffBacklight() {
  CleO.Display(0);    // backlight OFF
}
