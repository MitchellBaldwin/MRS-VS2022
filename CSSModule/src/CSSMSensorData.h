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
#include "BME280Data.h"
#include <ESP32Encoder.h>

//#include "SFQJS.h"
//#include "SparkFun_Qwiic_Joystick_Arduino_Library.h"

class CSSMSensorData
{
protected:
	byte KBSensePin = 34;
	byte ESP32VINSensePin = 36;

	MeasurementClass KPVoltage;		// Raw analog voltage from keypad ladder button array
	MeasurementClass ESP32VIN;		// Bufferesd analog voltage measurement

	byte HDGEncoderDTPin = 26;
	byte HDGEncoderCLKPin = 25;

	ESP32Encoder HDGEncoder;

public:
	BME280DataClass ENVData;		// Environment measurements

	int HDGEncoderSetting = 0;
	int CRSEncoderSetting = 0;

	CSSMSensorData();
	bool Init(byte kbSensePin, byte esp32VINSensePin);
	void Update();
	
	uint16_t GetKBRaw();
	float GetKBReal();
	String GetKPString();
	String GetKPString(String format);

	float GetESP32VINReal();
	String GetESP32VINString();
	String GetESP32VINString(String format);
	
	void ReadENVData();
};

extern CSSMSensorData SensorData;

#endif

