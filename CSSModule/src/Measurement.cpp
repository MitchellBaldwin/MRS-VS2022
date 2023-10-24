/*	Measurement.h
*	MeasurementClass - Base class for measurements made as single integer values, such as from an ADC
*
*/

#include "Measurement.h"

MeasurementClass::MeasurementClass()
{
	Offset = 0;
	GainNumerator = 1000000;
	GainDenominator = 1000000;
}

MeasurementClass::MeasurementClass(int offset, int megaGain)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
}

MeasurementClass::MeasurementClass(int offset, int megaGain, String units)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
	Units = units;
}

MeasurementClass::MeasurementClass(int offset, int gainNumerator, int gainDenominator)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
}

MeasurementClass::MeasurementClass(int offset, int gainNumerator, int gainDenominator, String units)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
	Units = units;
}

void MeasurementClass::Init(int offset, int megaGain)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
}

void MeasurementClass::Init(int offset, int megaGain, String units)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
	Units = units;
}

void MeasurementClass::Init(int offset, int gainNumerator, int gainDenominator)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
}

void MeasurementClass::Init(int offset, int gainNumerator, int gainDenominator, String units)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
	Units = units;
}

float MeasurementClass::GetRealValue()
{
	return (RawValue - Offset) * (float)GainNumerator / (float)GainDenominator;
}
