/*	OSBSet.h
*	OSBSet - Class that manages a set of physical buttons arrayed as Option Select Buttons (OSBs) relative to a
*	display, the logical relationship each has with text and data presented on the display in proximity to each button,
*	and the function invoked when a button is pressed.
*
*
*	Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#include "OSBSet.h"

void OSBSet::Init(uint8_t numLevels, const uint16_t levels[])
{
	Init(DefaultOSBArraySensePin, numLevels, levels);
}

void OSBSet::Init(byte sensePin, uint8_t numLevels, const uint16_t levels[])
{
	OSBArraySensePin = sensePin;
	OSBArray = new ezAnalogKeypad(OSBArraySensePin);

	// Initialize OSB array:
	OSBArray->setNoPressValue(levels[0]);
	OSBInfo[0] = new SoftOSBClass("None");
	for (byte osbID = 1; osbID < numLevels; ++osbID)
	{
		OSBArray->registerKey(osbID, levels[osbID]);
		OSBInfo[osbID] = new SoftOSBClass("NOP");		// Represents an uninitialized OSB
		++OSBCount;
	}
}

void OSBSet::InitOSB(OSBIDs osbID, String text)
{
	OSBInfo[osbID]->LabelText = text;
}

void OSBSet::InitOSB(OSBIDs osbID, String text, NMCommands::Commands command)
{
	OSBInfo[osbID]->LabelText = text;
	OSBInfo[osbID]->Command = command;
}

void OSBSet::Update()
{
	// Check for OSB press:
	OSBADCReading = analogRead(OSBArraySensePin);
	OSBIDs osb = GetOSBPress();
	if (osb != OSBIDs::NoOsb)
	{
		NewOSBPress = true;
		LastOSBPressed = osb;
	}
}

OSBSet::OSBIDs OSBSet::GetOSBPress()
{
	return (OSBSet::OSBIDs)OSBArray->getKey();
}

NMCommands::Commands OSBSet::GetOSBPressCommand()
{
	return OSBInfo[OSBArray->getKey()]->Command;
}

NMCommands::Commands OSBSet::GetOSBPressCommand(OSBIDs osb)
{
	return OSBInfo[osb]->Command;
}

OSBSet::OSBIDs OSBSet::NewOSBPressed()
{
	if (NewOSBPress)
	{
		NewOSBPress = false;
		return LastOSBPressed;
	}
	else
	{
		return OSBIDs::NoOsb;
	}
}
