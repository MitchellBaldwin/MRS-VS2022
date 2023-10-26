/*	SFQJS.h
*	SFQJSClass - Base class to hold measurements from a Sparkfun QWIIC Joystick module (SFE P/N COM-15168)
*
*/

#include "SFQJS.h"

SFQJSClass::SFQJSClass()
{
	JS = new JOYSTICK();
}

bool SFQJSClass::Init()
{
	bool success = JS->begin(Wire, JSI2CAddress);
	if (success)
	{

	}
	else
	{

	}
	return success;
}

bool SFQJSClass::Init(uint8_t i2cAddress)
{
	bool success = JS->begin(Wire, i2cAddress);
	if (success)
	{
		JSI2CAddress = i2cAddress;
	}
	else
	{

	}
	return success;
}


SFQJSClass SFQJS;

