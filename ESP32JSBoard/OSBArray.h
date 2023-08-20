/*	OSBArray.h
*	OSBArray - Base class for Option Select Buttons on the MRS Control Stick Steering Module
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
		OSB4
	};
	
	OSBArrayClass(byte sensePin);
	void Init(byte sensePin);
	byte GetOSBPress();
};

//extern OSBArrayClass OSBArray;

#endif

