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
#include <ezButton.h>
#include <ESP32Encoder.h>
#include "esp_adc_cal.h"

//#include "SFQJS.h"
//#include "SparkFun_Qwiic_Joystick_Arduino_Library.h"

class CSSMSensorData
{
protected:
	byte LeftToggleSwitchPin = 0x1B;	// GPIO27
	byte CenterToggleSwitchPin = 0x20;	// GPIO32
	byte RightToggleSwitchPin = 0x21;	// GPIO33
	byte RightRockerSwitchPin = 0x04;	// GPIO04
	
	byte KBSensePin = 34;
	byte ThrottleSensePin = 35;
	byte ESP32VINSensePin = 36;

	float ThrottleDeadZone = 5.0f;		// +/- % dead zone applied around zero

	MeasurementClass KPVoltage;			// Analog voltage from keypad ladder button array
	MeasurementClass ThrottleSetting;	// -100.0 tp 100.0 % slide throttle setting
	MeasurementClass ESP32VIN;			// Analog CSSM supply voltage measurement

	byte HDGEncoderDTPin = 17;
	byte HDGEncoderCLKPin = 16;
	//byte HDGEncoderDTPin = 26;
	//byte HDGEncoderCLKPin = 25;

	byte CRSEncoderDTPin = 25;
	byte CRSEncoderCLKPin = 26;
	//byte CRSEncoderDTPin = 13;
	//byte CRSEncoderCLKPin = 14;

	ESP32Encoder HDGEncoder;
	ESP32Encoder CRSEncoder;

	ezButton* LeftToggleSwitch;
	ezButton* CenterToggleSwitch;
	ezButton* RightToggleSwitch;
	ezButton* RightRockerSwitch;

	esp_adc_cal_characteristics_t ADC1Chars;
	uint32_t ReadCalibratedADC1(int rawADC1);	// Returns calibrated ADC1 measurement in mV

public:
	BME280DataClass ENVData;					// Environment measurements

	int HDGEncoderSetting = 0;
	int CRSEncoderSetting = 0;

	int LeftToggleSwitchState = 0;
	int CenterToggleSwitchState = 0;
	int RightToggleSwitchState = 0;
	int RightRockerSwitchState = 0;

	CSSMSensorData();
	bool Init();
	bool Init(byte kbSensePin, byte throttleSensePin, byte esp32VINSensePin);
	bool Init(byte kbSensePin, byte throttleSensePin, byte esp32VINSensePin, byte leftToggleSwitchPin, byte centerToggleSwitchPin, byte rightToggleSwitchPin, byte rightRockerSwitchPin);
	void Update();
	
	uint16_t GetKBRaw();
	float GetKBReal();
	String GetKPString();
	String GetKPString(String format);

	float GetThrottleActual();			// Get unmasked throttle setting
	float GetThrottle();				// Get throttle setting adjusted for dead zone(s)
	
	float GetESP32VINReal();
	String GetESP32VINString();
	String GetESP32VINString(String format);
	
	void ReadENVData();

	int GetRightRockerSwitchStateRaw();

};

extern CSSMSensorData SensorData;

#endif

