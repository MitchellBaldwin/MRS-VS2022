/*	OSBArray.cpp
*	OSBArrayClass - Base class for Option Select Buttons on the MRS Control Stick Steering Module
*
*/

#include "OSBArray.h"

OSBArrayClass::OSBArrayClass()
{
}

OSBArrayClass::OSBArrayClass(byte sensePin)
{
	OSBArraySensePin = sensePin;
}

OSBArrayClass::~OSBArrayClass()
{
	if (OSBArray != nullptr)
	{
		delete OSBArray;
	}
}

void OSBArrayClass::Init(uint8_t numLevels, const uint16_t levels[])
{
	OSBArray = new ezAnalogKeypad(OSBArraySensePin);

	// Initialize OSB array:
	OSBArray->setNoPressValue(levels[0]);
	for (byte osbID = 1; osbID < numLevels; ++osbID)
	{
		OSBArray->registerKey((OSBs)osbID, levels[osbID]);
	}
}

void OSBArrayClass::Init(uint8_t sensePin, uint8_t numLevels, const uint16_t levels[])
{
	OSBArraySensePin = sensePin;
	OSBArray = new ezAnalogKeypad(OSBArraySensePin);

	// Initialize OSB array:
	OSBArray->setNoPressValue(levels[0]);
	for (byte osbID = 1; osbID < numLevels; ++osbID)
	{
		OSBArray->registerKey((OSBs)osbID, levels[osbID]);
	}
}

byte OSBArrayClass::GetOSBPress()
{
	return OSBArray->getKey();
}
