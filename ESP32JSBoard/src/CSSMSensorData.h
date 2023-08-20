/*	CSSMSensorData.h
*	CSSMSensorData - Base class for managing data from sensors on the MRS Control Stick Steering Module
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _CSSMSensorData_h
#define _CSSMSensorData_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMSensorData
{
protected:
	byte KBSensePin;
	uint16_t KBRaw;

public:
	void Init(byte kbSensePin);
	void Update();
	uint16_t GetKBRaw();
};

extern CSSMSensorData SensorData;

#endif

