/*	CSSMS3EnvSensors.cpp
*	CSSMS3EnvSensors - Base class for managing measurements and sensor data from the
*	MRS Control Stick Steering Module S3
*
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#include "BME280Data.h"

#ifndef _CSSMS3EnvSensors_h
#define _CSSMS3EnvSensors_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMS3EnvSensors
{
protected:

public:
	BME280DataClass BME280Data;
	bool Init();
	void Update();

};

extern CSSMS3EnvSensors EnvSensors;

#endif

