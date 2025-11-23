/*	MCCSensors.h
*	MCCSensors - Manage MRC Master Communication & Coltrol module sensors
*
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _MCCSensors_h
#define _MCCSensors_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Measurement.h"
#include <Zanshin_BME680.h>
constexpr byte defaultBME680Address = 0x77;

//#include <Adafruit_INA219.h>
#include "INA219.h"
constexpr byte defaultINA219Address = 0x41;			// I2C address of INA219 sensor on WaveShare UPS 3S module

#include "esp_adc_cal.h"
constexpr uint32_t defaultVRef = 1100;

// Circuit internal to Lilygo T-Display S3 AMOLED: Battery positive terminal -> 100k resistor -> VMCU pin -> 100k resistor -> Battery negative terminal (GND)
constexpr byte defaultVMCUPin = GPIO_NUM_4;			// ADC1 channel 3; analog voltage measured at the MCU battery JST connector

constexpr byte TS3MCSupplySensePin = GPIO_NUM_21;	// Digital input; low when battery power is connected through TS3

class MCCSensors
{
protected:
	char buf[64];

	BME680_Class* BME680;
	float BME680Altitude(const int32_t press, const float seaLevel = 1013.25);

	MeasurementClass VMCU;							// Analog voltage measured at the MCU battery JST connector

	esp_adc_cal_characteristics_t ADC1Chars;
	uint32_t VRef = defaultVRef;
	bool SetupADC();
	uint32_t ReadCalibratedADC1(int rawADC1);		// Returns calibrated ADC1 measurement in mV

	//Adafruit_INA219* WSUPS3SINA219 = new Adafruit_INA219(defaultINA219Address);
	INA219* WSUPS3SINA219 = new INA219(defaultINA219Address);

public:
	
	bool Init();
	void Update();

	uint16_t GetMCURawADC();
	float GetMCUVoltageReal();
	String GetMCUVoltageString();
	String GetMCUVoltageString(String format);

};

extern MCCSensors mccSensors;

#endif

