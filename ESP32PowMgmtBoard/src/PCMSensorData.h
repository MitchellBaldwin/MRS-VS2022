/*	PCMSensorData.h
*	PCMSensorData - Base class for managing data from sensors on the MRS Power Control Module
*
*
*
*
*	Mitchell Baldwin copyright 2023-2024
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _PCMSensorData_h
#define _PCMSensorData_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr byte VBAT_PIN = 0x20;					// GPIO32 (ADC4) Conditioned DIYM cell voltage sense pin
constexpr byte VBBAT_PIN = 0x22;				// GPIO34 (ADC?) Internal TTGO T-Display supply voltage sense pin
constexpr byte VEXT_PIN = 0x24;					// GPIO36 (ADC0) Conditioned external supply plug voltage sense pin

class PCMSensorData
{
protected:
	int Vref = 1100;							// Default reference voltage for supply voltage measurement
	
	byte VBatSensePin = VBAT_PIN;
	byte VBBatSensePin = VBBAT_PIN;
	byte VExtSensePin = VEXT_PIN;

	bool SetupADC();

public:
	float VBat = 0.0f;
	float VBBat = 0.0f;
	float VExt = 0.0f;

	PCMSensorData();
	bool Init();
	void Update();

};

extern PCMSensorData SensorData;

#endif

