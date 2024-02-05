/*	Measurement.h
*	MeasurementClass - Base class for measurements made as single integer values, such as from an ADC
* 
*	Default zero offset is 0, default gain is 1.0
*	Default gain denominator is 1000000; use appropriate constructor to specify Mega-gain
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _Measurement_h
#define _Measurement_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <movingAvg.h>

constexpr int defaultAverageInterval = 8;

class MeasurementClass
{
protected:
	int RawValue;
	int Offset;
	int GainNumerator;
	int GainDenominator;
	String Units;
	int AverageInterval = defaultAverageInterval;
	movingAvg* AverageValue;
	char buf[32];

public:

	MeasurementClass();		// Default constructor
	MeasurementClass(int offset, int megaGain, int averageInterval = 8);
	MeasurementClass(int offset, int megaGain, String units, int averageInterval = 8);
	MeasurementClass(int offset, int gainNumerator, int gainDenominator, int averageInterval = 8);
	MeasurementClass(int offset, int gainNumerator, int gainDenominator, String units, int averageInterval = 8);

	void Init(int offset, int megaGain, int averageInterval = 8);
	void Init(int offset, int megaGain, String units, int averageInterval = 8);
	void Init(int offset, int gainNumerator, int gainDenominator, int averageInterval = 8);
	void Init(int offset, int gainNumerator, int gainDenominator, String units, int averageInterval = 8);

	void AddReading(int rawValue);
	int GetRawValue();
	int GetAverageRawValue();

	float GetRealValue();
	float GetAverageRealValue();

	String GetRealString();
	String GetRealString(String format);

};

#endif

