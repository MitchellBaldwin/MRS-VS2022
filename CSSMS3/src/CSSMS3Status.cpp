/*	CSSMS3Status.cpp
*	CSSMS3StatusClass - Base class for module level status and parameters
*
*/

#include "CSSMS3Status.h"

void CSSMS3StatusClass::Init()
{
}

void CSSMS3StatusClass::Update()
{
	MRSMCCESPNOWLinkStatus = (MRSMCCPacketReceivedCount != SaveMRSMCCPacketReceivedCount);
}

void CSSMS3StatusClass::AddDebugTextLine(String newLine)
{
	if (curDebugTextLine >= MAX_DEBUG_TEXT_LINES)
	{
		for (curDebugTextLine = 0; curDebugTextLine < MAX_DEBUG_TEXT_LINES - 1; ++curDebugTextLine)
		{
			debugTextLines[curDebugTextLine] = debugTextLines[curDebugTextLine + 1];
		}
	}
	debugTextLines[curDebugTextLine++] = newLine;
}

void CSSMS3StatusClass::ClearDebugText()
{
	for (int i = 0; i < MAX_DEBUG_TEXT_LINES; ++i)
	{
		debugTextLines[i] = "";
	}
	curDebugTextLine = 0;
}

CSSMS3StatusClass CSSMS3Status;

