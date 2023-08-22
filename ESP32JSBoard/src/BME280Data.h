/*	BME280Data.h
*	BME280DataClass - Base class to hold measurements from a BME280 sensor module
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _BME280DATA_h
#define _BME280DATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class BME280DataClass
{
protected:


public:
	float Tchip = NAN;
	float Pbaro = NAN;
	float RH = NAN;

	void Init();
	String GetTchipString();
};

extern BME280DataClass BME280Data;

#endif

