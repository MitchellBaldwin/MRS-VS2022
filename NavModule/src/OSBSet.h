/*	OSBSet.h
*	OSBSetClass - Class that manages a set of physical buttons arrayed as Option Select Buttons (OSBs) relative to a 
*	display, the logical relationship each has with text and data presented on the display in proximity to each button,
*	and the function invoked when a button is pressed.
*
*	Designed for use with the MFCDPageClass, which creates an instance of this class for each physical set of OSBs
*	associated with the display hosting the pages, and which updates the text, data and functions associated with each
*	physical button in the way needed to render each buttons' appearence and functionality as needed for each page.  
*
*	Maintains text, color, state and other information associated with a single OSB and its function in the context of
*	its parent MFCDPageClass instance, supplying this information as needed by the parent page to render text and
*	graphics associated with the OSB
*
*	Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/
#ifndef _OSBSet_h
#define _OSBSet_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr byte DefaultOSBArraySensePin = 0x24;		// GPIO36 (ADC1 CH0); Default analog input pin for OSB resistor ladder voltage measurement

#include <ezAnalogKeypad.h>
#include "SoftOSB.h"

class OSBSet
{
public:
	enum OSBIDs
	{
		NoOsb = 0,
		OSB1 = 1,
		OSB2,
		OSB3,
		OSB4,
		OSB5,
		OSB6,
		OSB7,
		OSB8
	};
	static const byte MaxOSBCount = OSBIDs::OSB8 + 1;
	byte OSBCount = 0;

	SoftOSBClass* OSBInfo[MaxOSBCount];

protected:
	ezAnalogKeypad* OSBArray;
	byte OSBArraySensePin = DefaultOSBArraySensePin;
	bool NewOSBPress = false;
	OSBIDs LastOSBPressed = OSBIDs::NoOsb;


public:
	int OSBADCReading = 0;
	
	void Init(uint8_t numLevels, const uint16_t levels[]);
	void Init(byte sensePin, uint8_t numLevels, const uint16_t levels[]);
	void InitOSB(OSBIDs osbID, String text);
	void InitOSB(OSBIDs osbID, String text, NMCommands::Commands command);

	void Update();

	OSBIDs GetOSBPress();
	NMCommands::Commands GetOSBPressCommand();
	NMCommands::Commands GetOSBPressCommand(OSBIDs osb);
	
	OSBIDs NewOSBPressed();

};

#endif

