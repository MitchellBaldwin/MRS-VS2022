/*	SFQJS.h
*	SFQJSClass - Base class to hold measurements from a Sparkfun QWIIC Joystick module (SFE P/N COM-15168)
*
*/

#include "SFQJS.h"
#include "DEBUG Macros.h"

SFQJSClass::SFQJSClass()
{
	//JS = new JOYSTICK();
	X = Y = 0;
	B = false;
}

bool SFQJSClass::Init()
{
	bool success = JS.begin(Wire, JSI2CAddress);
	if (success)
	{
		_PL("Success!")
	}
	else
	{
		_PL("NoGo")
	}
	return success;
}

bool SFQJSClass::Init(uint8_t i2cAddress)
{
	//bool success = JS.begin();
	bool success = JS.begin(Wire, i2cAddress);
	if (success)
	{
		JSI2CAddress = i2cAddress;
		_PPH(i2cAddress)
		_PL("Success!")
	}
	else
	{
		_PPH(i2cAddress)
		_PL("NoGo")
	}
	return success;
}

void SFQJSClass::Update()
{
	X = JS.getHorizontal() - 512;
	Y = JS.getVertical() - 512;
	B = JS.checkButton();
}


//SFQJSClass SFQJS;

