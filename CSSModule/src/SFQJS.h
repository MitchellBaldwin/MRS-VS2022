/*	SFQJS.h
*	SFQJSClass - Base class to hold measurements from a Sparkfun QWIIC Joystick module (SFE P/N COM-15168)
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

#ifndef _SFQJS_h
#define _SFQJS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SparkFun_Qwiic_Joystick_Arduino_Library.h>

class SFQJSClass
{
protected:
	uint8_t JSI2CAddress = QWIIC_JOYSTICK_ADDR;
	JOYSTICK JS;

public:
	int16_t X;
	int16_t Y;
	bool B;

	SFQJSClass();		// Default constructor
	bool Init();
	bool Init(uint8_t i2cAddress);
	void Update();

};

//extern SFQJSClass SFQJS;

#endif

