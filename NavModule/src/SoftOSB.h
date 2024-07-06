/*	SoftOSB.h
*	SoftOSBClass - Base class managing the display of text and graphics associated with an option select button (OSB)
*	on a multi-function color display (MFCD)
*
*	Designed for use with the MFCDPageClass, which builds and maintains a list of references to instances of this class
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
#ifndef _SoftOSB_h
#define _SoftOSB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SoftOSBClass
{
public:
	enum States
	{
		Plain		= 0b00000000,
		Boxed		= 0b00000001,
		Inverted	= 0b00000010,
		Starred		= 0b00000100,

	};
	States State = States::Plain;

	enum OSBPositions
	{
		LOSB1,
		LOSB2,
		LOSB3,
		LOSB4,
		ROSB1,
		ROSB2,
		ROSB3,
		ROSB4,
	};
	OSBPositions Position;

	typedef void (*OSBOnPressHandler)(void);	// Function pointer for OnPress event handler

protected:
	String Text;		// Primary label text identifying this OSB

	OSBOnPressHandler onPressHandler = nullptr;

public:
	SoftOSBClass();
	SoftOSBClass(String text, OSBPositions position);
	//SoftOSBClass(String text, OSBPositions position, OSBOnPressHandler handler);

	bool Init();

	void SetOnPressHandler(OSBOnPressHandler handler);
	void InvokeOnPressHandler();
};

#endif

