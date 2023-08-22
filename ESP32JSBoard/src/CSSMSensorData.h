/*	CSSMSensorData.h
*	CSSMSensorData - Base class for managing data from sensors on the MRS Control Stick Steering Module
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _CSSMSensorData_h
#define _CSSMSensorData_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Measurement.h"
#include <BME280I2C.h>

class CSSMSensorData
{
protected:
	byte KBSensePin;
	byte ESP32VINSensePin;
	BME280I2C* bme280;

public:
	MeasurementClass KPVoltage;
	MeasurementClass ESP32VIN;

	CSSMSensorData();
	bool Init(byte kbSensePin, byte esp32VINSensePin);
	void Update();
	uint16_t GetKBRaw();
};

extern CSSMSensorData SensorData;

#endif

