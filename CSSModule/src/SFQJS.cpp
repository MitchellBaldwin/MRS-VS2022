/*	SFQJS.h
*	SFQJSClass - Base class to hold measurements from a Sparkfun QWIIC Joystick module (SFE P/N COM-15168)
*
*/

#include "SFQJS.h"

SFQJSClass::SFQJSClass()
{
	JS = new JOYSTICK();
}

void SFQJSClass::Init()
{


}


SFQJSClass SFQJS;

