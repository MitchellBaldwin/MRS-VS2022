/*	I2CBusClass - Base class for interfacing with the I2C bus
*
*
*
*
*	Mitchell Baldwin copyright 2022
*
*	v 0.00:	Initial command set
*	v
*
*/


#ifndef _I2CBUS_h
#define _I2CBUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr auto MaxI2CDeviceCount = 8;

class I2CBusClass
{
	protected:
		bool maxDeviceCountExceeded = false;

	public:
		uint8_t I2CDeviceCount = 0;
		uint8_t ActiveI2CDeviceAddresses[MaxI2CDeviceCount];

		void Init();
		bool ScanAddress(uint8_t address);
		void Scan();
		bool MaxDeviceCountExceeded();
};

extern I2CBusClass I2CBus;

#endif

