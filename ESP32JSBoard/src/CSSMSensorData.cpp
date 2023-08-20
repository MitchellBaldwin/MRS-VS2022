/*	CSSMSensorData.cpp
*	CSSMSensorData - Base class for managing data from sensors on the MRS Joystick board
*
*/

#include "CSSMSensorData.h"

void CSSMSensorData::Init(byte kbSensePin)
{
	KBSensePin = kbSensePin;
	pinMode(KBSensePin, INPUT);
}

void CSSMSensorData::Update()
{
	KBRaw = analogRead(KBSensePin);
}

uint16_t CSSMSensorData::GetKBRaw()
{
	return KBRaw;
}


CSSMSensorData SensorData;

