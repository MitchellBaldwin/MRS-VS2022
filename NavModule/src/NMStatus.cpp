/*	NMStatus.h
*	NMStatusClass - Base class for module level status and parameters
*
*/
#include "NMStatus.h"

void NMStatusClass::Init()
{
}

void NMStatusClass::AddDebugTextLine(String newLine)
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

void NMStatusClass::ClearDebugText()
{
	for (int i = 0; i < MAX_TEXT_LINES; ++i)
	{
		debugTextLines[i] = "";
	}
}

NMStatusClass NMStatus;

