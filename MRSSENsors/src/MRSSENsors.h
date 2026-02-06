/*	MRSSENsors.h
*	MRSSENsorsClass - Base class for controlling and accessing MRS Sensors module sensors
*
*	Mitchell Baldwin copyright 2026
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MRSSENsors_h
#define _MRSSENsors_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr uint8_t defaultMRSSENAddress = 0x08;			// I2C address of MRS Sensors module on MCC I2C bus

class MRSSENsorsClass
{
protected:
	char buf[64];
	uint8_t _i2caddress = defaultMRSSENAddress;

public:
	MRSSENsorsClass();
	bool Init();
	bool Init(uint8_t i2cAddress);
};

#endif

