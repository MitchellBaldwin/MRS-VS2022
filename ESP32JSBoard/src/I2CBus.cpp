/*	I2CBusClass - Base class for interfacing with the I2C bus
*
*
*
*
*	Mitchell Baldwin copyright 2022
*
*/

#include "I2CBus.h"
#include <Wire.h>

void I2CBusClass::Init()
{
	Wire.begin();

}

bool I2CBusClass::ScanAddress(uint8_t address)
{
	Wire.beginTransmission(address);
	if (Wire.endTransmission() == 0x00)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void I2CBusClass::Scan()
{
	I2CDeviceCount = 0;
	for (uint8_t i = 0x00; i < MaxI2CDeviceCount; ++i)
	{
		ActiveI2CDeviceAddresses[i] = 0x00;
	}
	// Scan for devices across the range 0x01 - 0x7F (0x00 always returns true?)
	for (uint8_t i = 0x01; i < 0x80; ++i)
	{
		if (ScanAddress(i))
		{
			if (I2CDeviceCount < MaxI2CDeviceCount)
			{
				ActiveI2CDeviceAddresses[I2CDeviceCount] = i;
				I2CDeviceCount++;
			}
			else
			{
				maxDeviceCountExceeded = true;
			}
		}
	}
}

bool I2CBusClass::MaxDeviceCountExceeded()
{
	return maxDeviceCountExceeded;
}

I2CBusClass I2CBus;

