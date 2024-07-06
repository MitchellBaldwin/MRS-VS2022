/*	SoftOSB.h
*	SoftOSBClass - Base class managing the display of text and graphics associated with an option select button (OSB)
*	on a multi-function color display (MFCD)
*
*	Mitchell Baldwin copyright 2024
*
*/

#include "SoftOSB.h"

SoftOSBClass::SoftOSBClass()
{
}

SoftOSBClass::SoftOSBClass(String text, OSBPositions position)
{
	Text = text;
	Position = position;
}

//SoftOSBClass(String text, OSBPositions position, OSBOnPressHandler handler)
//{
//	Text = text;
//	Position = position;
//	onPressHandler = handler;
//}

bool SoftOSBClass::Init()
{


}

void SoftOSBClass::SetOnPressHandler(OSBOnPressHandler handler)
{
	onPressHandler = handler;
}

void SoftOSBClass::InvokeOnPressHandler()
{
	if (onPressHandler != nullptr)
	{
		onPressHandler();
	}
}

