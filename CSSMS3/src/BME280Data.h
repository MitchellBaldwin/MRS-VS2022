/*	BME280Data.h
*	BME280DataClass - Base class to hold measurements from a BME280 sensor module
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.0:	Initial commit
*	v 0.1:	Refactor to encapsulate BME280 sensor module
*
*/

#ifndef _BME280DATA_h
#define _BME280DATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <BME280.h>
#include <BME280I2C.h>
#include <BME280I2C_BRZO.h>
#include <BME280Spi.h>
#include <BME280SpiSw.h>
#include <EnvironmentCalculations.h>

class BME280DataClass
{
protected:
	char buf[32];
	BME280I2C* bme280;
	float Tchip = NAN;
	float Pbaro = NAN;
	float RH = NAN;

public:
	BME280DataClass();	// Default constructor
	bool Init();
	void ReadENVData();
	void GetTchipString(String* tempString);
	void GetPbaroString(String* pbaroString);
	void GetRHString(String* rhString);

};

//extern BME280DataClass BME280Data;

#endif

