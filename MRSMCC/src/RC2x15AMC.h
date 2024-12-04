/* RC2x15AMC.h
* RC2x15AMC class - Container class for working with the RoboClaw 2x 15A Motor Controller
*
*
*
*
* Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _RC2X15AMC_h
#define _RC2X15AMC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <HardwareSerial.h>
#include <RoboClaw.h>

constexpr uint8_t RC2x15AAddress = 0x80;

class RC2x15AMCClass
{
protected:
	HardwareSerial* RC2x15ASerial;	// Packet serial link to RoboClaw 2x15A Motor Controller
	RoboClaw* RC2x15A;
	uint8_t PSAddress = RC2x15AAddress;

public:
	void Init();
	bool ReadStatus();
	void Drive(float throttle, float turnRate);

};

extern RC2x15AMCClass RC2x15AMC;

#endif

