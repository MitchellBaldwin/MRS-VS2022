/*	MCCSensors.h
*	MCCSensors - Manage MRC Master Communication & Coltrol module sensors
*
*/

#include "MCCSensors.h"
#include "DEBUG Macros.h"
#include "MCCStatus.h"

float MCCSensors::BME680Altitude(const int32_t press, const float seaLevel)
{
	float alt = 44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));	// Convert into meters
	return alt;
}

bool MCCSensors::Init()
{
	// Initialize BME680 sensor:
	BME680 = new BME680_Class();
	bool success = BME680->begin(Wire.getClock(), defaultBME680Address);
	if (!success)
	{
		_PL("Failed to initialize BME680 sensor")

	}
	else
	{
		BME680->setOversampling(TemperatureSensor, Oversample16);	// Use enumerated type values
		BME680->setOversampling(HumiditySensor, Oversample16);		// Use enumerated type values
		BME680->setOversampling(PressureSensor, Oversample16);		// Use enumerated type values
		BME680->setIIRFilter(IIR4);									// Use enumerated type values
		BME680->setGas(320, 150);									// 320?c for 150 milliseconds
	}

	// Initialize VL53L1X distance sensor:

	// Initialoze Odometry sensor:


	return success;
}

void MCCSensors::Update()
{
	char buf[32];
	static int32_t temp, rh, pbaro, gas;
	if (!BME680->measuring())
	{
		// If not measuring then assume the last measurement period has ended:
		MCCStatus.mrsSensorPacket.BME280Temp = (float)temp / 100.0f;	// Convert from centidegrees
		MCCStatus.mrsSensorPacket.BME280RH = (float)rh / 1000.0f;		// Convert from milli-%
		MCCStatus.mrsSensorPacket.BME280Pbaro = (float)pbaro / 100.0f;	// Convert from Pa to hPa
		MCCStatus.mrsSensorPacket.BME280Gas = (float)gas / 100.0f;		// Convert from milliohms (?)
		MCCStatus.mrsSensorPacket.BME280Alt = BME680Altitude(pbaro);	// m
	}
	else
	{
		// Start a new measurement:
		BME680->getSensorData(temp, rh, pbaro, gas, false);				// Setting waitSwitch = false to read asynchronously
	}

}


MCCSensors mccSensors;

