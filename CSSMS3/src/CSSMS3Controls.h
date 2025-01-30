/*	CSSMS3Controls.h
*	CSSMS3Controls - Base class for managing control inputs and sensor data from the 
*	MRS Control Stick Steering Module
*
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _CSSMS3Controls_h
#define _CSSMS3Controls_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Measurement.h"
#include "BME280Data.h"
#include <ezButton.h>
#include "esp_adc_cal.h"

constexpr byte defaultKPSensePin = GPIO_NUM_1;
constexpr byte defaultLThrottlePin = GPIO_NUM_2;
constexpr byte defaultRThrottlePin = GPIO_NUM_3;

class CSSMS3Controls
{
protected:
	byte KPSensePin = defaultKPSensePin;
	byte LThrottlePin = defaultLThrottlePin;
	byte RThrottlePin = defaultRThrottlePin;

	MeasurementClass KPVoltage;			// Analog voltage from keypad ladder button array
	MeasurementClass LThrottleSetting;	// -100.0 tp 100.0 % left slide throttle setting
	MeasurementClass RThrottleSetting;	// -100.0 tp 100.0 % right slide throttle setting

	float ThrottleDeadZone = 5.0f;				// +/- % dead zone applied around zero (for both throttles)

	esp_adc_cal_characteristics_t ADC1Chars;
	uint32_t ReadCalibratedADC1(int rawADC1);	// Returns calibrated ADC1 measurement in mV

	float GetLThrottleActual();			// Get unmasked left throttle setting
	float GetLThrottle();				// Get left throttle setting adjusted for dead zone(s)
	float GetRThrottleActual();			// Get unmasked right throttle setting
	float GetRThrottle();				// Get right throttle setting adjusted for dead zone(s)


public:
	bool Init();
	void Update();

};

extern CSSMS3Controls cssmS3Controls;

#endif

