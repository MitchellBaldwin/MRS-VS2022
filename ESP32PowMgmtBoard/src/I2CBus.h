/*	I2CBusClass - Base class for interfacing with the I2C bus
*
*
*
*
*	Mitchell Baldwin copyright 2022-2024
*
*	v 1.00:	Initial feature set
*	v 0.10: Modified for use with TTGO T Display module (26 Jun 2024)
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

// Using the TTGO T-Display pin 22 is used for the built-in LED, so we must re-map SCL to another pin:
// Default SCL definition is static const uint8_t SCL = 22
// GPIO26 is the default UART2TX pin, though, so consider using GPIO27 instead

constexpr int DefaultSDA = SDA;
constexpr int DefaultSCL = SCL;

class I2CBusClass
{
	protected:
		bool maxDeviceCountExceeded = false;

	public:
		uint8_t I2CDeviceCount = 0;
		uint8_t ActiveI2CDeviceAddresses[MaxI2CDeviceCount];

		bool Init();
		bool Init(int sdaPin, int sclPin);
		bool ScanAddress(uint8_t address);
		void Scan();
		bool MaxDeviceCountExceeded();

		String GetActiveI2CAddressesString();
		String Get1st6ActiveI2CAddressesString();
		String Get1st4ActiveI2CAddressesString();
};

extern I2CBusClass I2CBus;

#endif

