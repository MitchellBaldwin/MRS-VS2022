/*	MCCSensors.h
*	MCCSensors - Manage MRC Master Communication & Coltrol module sensors
*
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/
#include <Zanshin_BME680.h>

#ifndef _MCCSensors_h
#define _MCCSensors_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr byte defaultBME680Address = 0x77;

class MCCSensors
{
protected:
	BME680_Class* BME680;
	float BME680Altitude(const int32_t press, const float seaLevel = 1013.25);

public:
	
	bool Init();
	void Update();

};

extern MCCSensors mccSensors;

#endif

