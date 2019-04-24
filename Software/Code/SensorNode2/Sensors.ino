  /**************************************/
  /* Request data from all sensors      */
  /**************************************/
  void requestSensorData() {  
    pressure = pressureSensor.getPressure_hPa();
    Sps30Read();
    
    state = 2;   // goto state 2
    statsUpdated = 0;
  }

  /****************************************/
  /* Read routine for PMsensor            */  
  /****************************************/
  void Sps30Read() {
    MassPM1Value = sps30.GetMassPM1();
    MassPM2Value = sps30.GetMassPM2();
    MassPM4Value = sps30.GetMassPM4();
    MassPM10Value = sps30.GetMassPM10();
}
