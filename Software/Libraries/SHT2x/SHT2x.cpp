/*
  SHT2x - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor
	SHT2x-Breakout Module - http://www.misenso.com/products/001
	
  Created by Christopher Ladden at Modern Device on December 2009.
  Modified by Paul Badger March 2010
  
  Modified by www.misenso.com on October 2011:
	- code optimisation
	- compatibility with Arduino 1.0

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <inttypes.h>
#include <Wire.h>
#include "Arduino.h"
#include "SHT2x.h"



/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float SHT2xClass::GetHumidity(void)
{
  float value = readSensor(eRHumidityHoldCmd);
  if (value == 0) {
      return 0;                       // Some unrealistic value
  }
  return -6.0 + 125.0 / 65536.0 * value;
}


/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float SHT2xClass::GetTemperature(void)
{
    float value = readSensor(eTempHoldCmd);
    if (value == 0) {
        return -273;                    // Roughly Zero Kelvin indicates an error
    }
    return -46.85 + 175.72 / 65536.0 * value;
}


/**********************************************************
 * resetSHT
 *  Resets the temperature sensor.
 *
 * no return
 **********************************************************/
void SHT2xClass::resetSHT() {
	Wire.beginTransmission(eSHT2xAddress);
	Wire.write(eSoftReset);
	Wire.endTransmission();
	delay(15);	// wait for SHT to reset
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT2xClass::readSensor(uint8_t command)
{
    uint16_t result;

    Wire.beginTransmission(eSHT2xAddress);	//begin
    Wire.write(command);					//send the pointer location
    Wire.endTransmission();               	//end
    delay(100);
    

    Wire.requestFrom(eSHT2xAddress, 3);
    uint32_t timeout = millis() + 300;       // Don't hang here for more than 300ms
    while(Wire.available() < 3) {
      if ((millis() - timeout) > 0) {
        return 0;
      }
    }

    //Store the result
    result = ((Wire.read()) << 8);
    result += Wire.read();
	  result &= ~0x0003;   // clear two low bits (status bits)
    return result;

    //Clear the final byte from the buffer
    Wire.read();    
}

SHT2xClass SHT2x;
