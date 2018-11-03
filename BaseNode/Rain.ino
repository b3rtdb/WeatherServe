/********************************************
 * Calculate the rainfall as intensity      *
 * every 10 min for mm/hour and             *
 * every 24 hours for a sum of mm/day       *
 ********************************************/
void calcRain() {
  float rainIntensityTicks = 0;
  rainArray[rainArrayCounter] = rainSensorCount;      // Rain gauge ticks are kept in rainArray
  rainArrayCounter++;
           
  totalRain24hTicks += rainSensorCount;               // The Rain gauge ticks are added for the total mm today and are reset with the global reset @ midnight
  totalRain24h = totalRain24hTicks*0.2;               // Recalculate the rain gauge ticks to actual mm

  for (byte i = 0; i<15; i++) {
    rainIntensityTicks += *(rainArray + i);           // The rainIntensity is always the sum of all the values in the rainArray (15 minutes), this sum is done by a for loop (0->14)
  }
  rainIntensity = rainIntensityTicks*4*0.2;           // Recalculate the rain gauge ticks per 15 minutes to actual mm/hour (15min * 4 = 60min * 0,2mm/tick)

  if (rainArrayCounter == 15) {  // Every 15min the counter is reset. The values stay in the array and are overwritten --> running sum
    rainArrayCounter = 0;
  }
}
