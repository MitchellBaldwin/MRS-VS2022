/*	MCCStatus.h
*	MCCStatusClass - Base class for module level status and parameters
*
*	Mitchell Baldwin copyright 2024
*
*/

#include "MCCStatus.h"
#include "RC2x15AMC.h"
#include "DEBUG Macros.h"

void MCCStatusClass::Init()
{
}

void MCCStatusClass::Update()
{
	CSSMESPNOWLinkStatus = (CSSMPacketReceivedCount != SaveCSSMPacketReceivedCount);

	//TODO: Re-establich / re-synch UART communication with the motor controller; simply ending and re-beginning
	//the UART does not see to work.  
	//if (!RC2x15AMCStatus)
	//{
	//	RC2x15AMCStatus = RC2x15AMC.ResetUARTLink();
	//	if (!RC2x15AMCStatus)
	//	{
	//		_PL("Motor controller not responding")
	//	}
	//}
}

void MCCStatusClass::AddDebugTextLine(String newLine)
{
	if (curDebugTextLine >= MAX_TEXT_LINES)
	{
		for (curDebugTextLine = 0; curDebugTextLine < MAX_TEXT_LINES - 1; ++curDebugTextLine)
		{
			debugTextLines[curDebugTextLine] = debugTextLines[curDebugTextLine + 1];
		}
	}
	debugTextLines[curDebugTextLine++] = newLine;
}

void MCCStatusClass::ClearDebugText()
{
	for (int i = 0; i < MAX_TEXT_LINES; ++i)
	{
		debugTextLines[i] = "";
	}
	curDebugTextLine = 0;
}

MCCStatusClass MCCStatus;

