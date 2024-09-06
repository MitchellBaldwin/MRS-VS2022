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

constexpr byte V5_PIN = GPIO_NUM_1;					// GPIO01 (ADC1_CH0) Conditioned 5 VDC regulator output (from VEXT)
constexpr byte VEXT_PIN = GPIO_NUM_2;				// GPIO02 (ADC1_CH1) Conditioned external supply plug voltage sense pin
constexpr byte VBAT_PIN = GPIO_NUM_3;				// GPIO03 (ADC1_CH2) Conditioned DIYM cell voltage sense pin
constexpr byte VMCU_PIN = GPIO_NUM_4;				// GPIO04 (ADC1_CH3?) Internal T-Display supply voltage sense pin
constexpr byte VBBAT_PIN = GPIO_NUM_10;				// GPIO10 (ADC1_CH9) Conditioned 1S LiPo backup battery voltage sense pin

class PCMSensorData
{
protected:
	int Vref = 1100;							// Default reference voltage for supply voltage measurement
	
	byte V5SensePin = V5_PIN;
	byte VBatSensePin = VBAT_PIN;
	byte VBBatSensePin = VBBAT_PIN;
	byte VMCUSensePin = VMCU_PIN;
	byte VExtSensePin = VEXT_PIN;

	bool SetupADC();

public:
	float V5 = 0.0f;
	float VBat = 0.0f;
	float VBBat = 0.0f;
	float VMCU = 0.0f;
	float VExt = 0.0f;

	PCMSensorData();
	bool Init();
	void Update();

};

extern PCMSensorData SensorData;

#endif

