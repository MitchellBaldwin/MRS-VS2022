/*	OSBArray.h
*	OSBArrayClass - Base class for Option Select Buttons on the MRS Control Stick Steering Module
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.0:	Initial commit
*	v 0.1:	
*
*/

#ifndef _OSBARRAY_h
#define _OSBARRAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ezAnalogKeypad.h>

class OSBArrayClass
{
protected:
	ezAnalogKeypad* OSBArray;
	byte OSBArraySensePin;

public:
	enum OSBs
	{
		OSB1 = 1,
		OSB2,
		OSB3,
		OSB4,
		OSB5,
		OSB6,
		OSB7,
		OSB8
	};
	
	OSBArrayClass(byte sensePin);
	~OSBArrayClass();
	void Init(uint8_t numLevels, const uint16_t levels[]);
	byte GetOSBPress();
};

#endif

