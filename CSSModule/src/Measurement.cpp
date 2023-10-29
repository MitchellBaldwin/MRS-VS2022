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

MeasurementClass::MeasurementClass(int offset, int megaGain, int averageInterval)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
	AverageInterval = averageInterval;
}

MeasurementClass::MeasurementClass(int offset, int megaGain, String units, int averageInterval)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
	Units = units;
	AverageInterval = averageInterval;
}

MeasurementClass::MeasurementClass(int offset, int gainNumerator, int gainDenominator, int averageInterval)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
	AverageInterval = averageInterval;
}

MeasurementClass::MeasurementClass(int offset, int gainNumerator, int gainDenominator, String units, int averageInterval)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
	Units = units;
	AverageInterval = averageInterval;
}

void MeasurementClass::Init(int offset, int megaGain, int averageInterval)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
	AverageInterval = averageInterval;
	AverageValue = new movingAvg(AverageInterval);
	AverageValue->begin();
}

void MeasurementClass::Init(int offset, int megaGain, String units, int averageInterval)
{
	Offset = offset;
	GainNumerator = megaGain;
	GainDenominator = 1000000;
	Units = units;
	AverageInterval = averageInterval;
	AverageValue = new movingAvg(AverageInterval);
	AverageValue->begin();
}

void MeasurementClass::Init(int offset, int gainNumerator, int gainDenominator, int averageInterval)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
	AverageInterval = averageInterval;
	AverageValue = new movingAvg(AverageInterval);
	AverageValue->begin();
}

void MeasurementClass::Init(int offset, int gainNumerator, int gainDenominator, String units, int averageInterval)
{
	Offset = offset;
	GainNumerator = gainNumerator;
	GainDenominator = gainDenominator;
	Units = units;
	AverageInterval = averageInterval;
	AverageValue = new movingAvg(AverageInterval);
	AverageValue->begin();
}

void MeasurementClass::AddReading(int rawValue)
{
	// The reading() method returns the updated average value, so we could store that in a field here for easire future access
	RawValue = rawValue;
	AverageValue->reading(rawValue);
}

int MeasurementClass::GetRawValue()
{
	return RawValue;
}

int MeasurementClass::GetAverageRawValue()
{
	if (AverageValue->getCount() > 0)
	{
		return AverageValue->getAvg();
	}
	else
	{
		return 0;
	}
}

float MeasurementClass::GetRealValue()
{
	return (RawValue - Offset) * (float)GainNumerator / (float)GainDenominator;
}

float MeasurementClass::GetAverageRealValue()
{
	if (AverageValue->getCount() > 0)
	{
		return (AverageValue->getAvg() - Offset) * (float)GainNumerator / (float)GainDenominator;
	}
	else
	{
		return 0.0f;
	}
}
