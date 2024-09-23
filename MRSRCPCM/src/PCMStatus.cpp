/*	PCMStatus.h
*	PCMStatusClass - Base class for module level status and parameters
*
*/
#include "PCMStatus.h"

void PCMStatusClass::Init()
{
}

void PCMStatusClass::AddDebugTextLine(String newLine)
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

void PCMStatusClass::ClearDebugText()
{
	for (int i = 0; i < MAX_TEXT_LINES; ++i)
	{
		debugTextLines[i] = "";
	}
	curDebugTextLine = 0;
}

PCMStatusClass PCMStatus;

