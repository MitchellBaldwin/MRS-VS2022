/*	MRSNavSensors.h
*	MRSNavSensors - Base class for managing setup, taking sensor measurements, and building and communicating
*	packets containing sensor data with the MRS MCC
* 
*	Sensor types managed by this class include:
*	Proximity & distance sensors
*	Odometry sensors
*	IMU
*	GPS
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MRSNavSensors_h
#define _MRSNavSensors_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SparkFun_Qwiic_OTOS_Arduino_Library.h>
#include <SparkFun_VL53L1X.h>
#include <PCF8563.h>

class MRSNavSensors
{
protected:
	QwiicOTOS* OTOS;
	SFEVL53L1X* FwdVL53L1X;
	PCF8563* RTC;

public:
	bool Init();
	String GetDateTimeString();

	void Update();

};

extern MRSNavSensors mrsNavSensors;

#endif

