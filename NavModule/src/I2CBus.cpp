/*	I2CBusClass - Base class for interfacing with the I2C bus
*
*
*
*
*	Mitchell Baldwin copyright 2022-2024
*
*/

#include "I2CBus.h"
#include <Wire.h>

//#include "DEBUG Macros.h"

// Using the TTGO T Display pin 22 is used for the built-in LED,
//so we must re-map SCL to another pin:

const uint8_t sda = 21;
const uint8_t scl = 27;

void I2CBusClass::Init()
{
	Wire.begin(sda, scl);
}

bool I2CBusClass::ScanAddress(uint8_t address)
{
	////Debug code:
	//char buf[32];
	//snprintf(buf, 31, "Scanning I2C %02X", address);
	//_PL(buf)

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

String I2CBusClass::GetActiveI2CAddressesString()
{
	char buf[32];
	snprintf(buf, 31, "I2C %02X %02X %02X %02X %02X %02X %02X %02X",
		I2CBus.ActiveI2CDeviceAddresses[0],
		I2CBus.ActiveI2CDeviceAddresses[1],
		I2CBus.ActiveI2CDeviceAddresses[2],
		I2CBus.ActiveI2CDeviceAddresses[3],
		I2CBus.ActiveI2CDeviceAddresses[4],
		I2CBus.ActiveI2CDeviceAddresses[5],
		I2CBus.ActiveI2CDeviceAddresses[6],
		I2CBus.ActiveI2CDeviceAddresses[7]);
	
	return String(buf);
}

String I2CBusClass::Get1st6ActiveI2CAddressesString()
{
	char buf[32];
	snprintf(buf, 31, "I2C %02X %02X %02X %02X %02X %02X",
		I2CBus.ActiveI2CDeviceAddresses[0],
		I2CBus.ActiveI2CDeviceAddresses[1],
		I2CBus.ActiveI2CDeviceAddresses[2],
		I2CBus.ActiveI2CDeviceAddresses[3],
		I2CBus.ActiveI2CDeviceAddresses[4],
		I2CBus.ActiveI2CDeviceAddresses[5]);

	return String(buf);
}

String I2CBusClass::Get1st4ActiveI2CAddressesString()
{
	char buf[32];
	snprintf(buf, 31, "I2C %02X %02X %02X %02X",
		I2CBus.ActiveI2CDeviceAddresses[0],
		I2CBus.ActiveI2CDeviceAddresses[1],
		I2CBus.ActiveI2CDeviceAddresses[2],
		I2CBus.ActiveI2CDeviceAddresses[3]);

	return String(buf);
}

I2CBusClass I2CBus;

