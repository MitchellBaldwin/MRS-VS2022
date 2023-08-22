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

class MeasurementClass
{
protected:
	int Offset;
	int GainNumerator;
	int GainDenominator;

public:
	int RawValue;
	String Units;

	MeasurementClass();		// Default constructor
	MeasurementClass(int offset, int megaGain);
	MeasurementClass(int offset, int megaGain, String units);
	MeasurementClass(int offset, int gainNumerator, int gainDenominator);
	MeasurementClass(int offset, int gainNumerator, int gainDenominator, String units);

	void Init(int offset, int megaGain);
	void Init(int offset, int megaGain, String units);
	void Init(int offset, int gainNumerator, int gainDenominator);
	void Init(int offset, int gainNumerator, int gainDenominator, String units);

	float GetRealValue();
};

#endif

