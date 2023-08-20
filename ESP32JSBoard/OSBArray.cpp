/*	OSBArray.cpp
*	OSBArray - Base class for Option Select Buttons on the MRS Control Stick Steering Module
*
*/

#include "OSBArray.h"

OSBArrayClass::OSBArrayClass(byte sensePin)
{
	Init(sensePin);
}

void OSBArrayClass::Init(byte sensePin)
{
	OSBArraySensePin = sensePin;
	OSBArray = new ezAnalogKeypad(OSBArraySensePin);

	// Initialize OSB array:
	OSBArray->setNoPressValue(0);
	OSBArray->registerKey(OSB1, 1165);
	OSBArray->registerKey(OSB2, 1770);
	OSBArray->registerKey(OSB3, 2400);
	OSBArray->registerKey(OSB4, 3160);

}

byte OSBArrayClass::GetOSBPress()
{
	return OSBArray->getKey();
}


//OSBArrayClass OSBArray;

