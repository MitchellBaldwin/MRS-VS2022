/*	CSSMSensorData.cpp
*	CSSMSensorData - Base class for managing data from sensors on the MRS Joystick board
*
*/

#include "CSSMSensorData.h"
#include "CSSMStatus.h"
#include "DEBUG Macros.h"

CSSMSensorData::CSSMSensorData()
{
	/* Recommended Modes -
	   Based on Bosch BME280I2C environmental sensor data sheet.

	Weather Monitoring :
	   forced mode, 1 sample/minute
	   pressure ×1, temperature ×1, humidity ×1, filter off
	   Current Consumption =  0.16 ?A
	   RMS Noise = 3.3 Pa/30 cm, 0.07 %RH
	   Data Output Rate 1/60 Hz

	Humidity Sensing :
	   forced mode, 1 sample/second
	   pressure ×0, temperature ×1, humidity ×1, filter off
	   Current Consumption = 2.9 ?A
	   RMS Noise = 0.07 %RH
	   Data Output Rate =  1 Hz

	Indoor Navigation :
	   normal mode, standby time = 0.5ms
	   pressure ×16, temperature ×2, humidity ×1, filter = x16
	   Current Consumption = 633 ?A
	   RMS Noise = 0.2 Pa/1.7 cm
	   Data Output Rate = 25Hz
	   Filter Bandwidth = 0.53 Hz
	   Response Time (75%) = 0.9 s

	Gaming :
	   normal mode, standby time = 0.5ms
	   pressure ×4, temperature ×1, humidity ×0, filter = x16
	   Current Consumption = 581 ?A
	   RMS Noise = 0.3 Pa/2.5 cm
	   Data Output Rate = 83 Hz
	   Filter Bandwidth = 1.75 Hz
	   Response Time (75%) = 0.3 s

	*/

	// Setting up BME280 sensor for "Indoor Navigation":
	BME280I2C::Settings settings(
		BME280::OSR_X1,
		BME280::OSR_X1,
		BME280::OSR_X4,
		BME280::Mode_Forced,
		BME280::StandbyTime_1000ms,
		BME280::Filter_16,
		BME280::SpiEnable_False,
		BME280I2C::I2CAddr_0x76		// I2C address. I2C specific.
	);

	bme280 = new BME280I2C(settings);
	CSSMStatus.BME280Status = false;
}

bool CSSMSensorData::Init(byte kbSensePin, byte esp32VINSensePin)
{
	KBSensePin = kbSensePin;
	pinMode(KBSensePin, INPUT);
	// Assumes use of a unity gain OpAmp buffer (3.3 V max);
	// 3.3 V / 4096 * 1000000 = 806 V/count:
	//KPVoltage.Init(0, 806, "V");
	// Can achieve the same result as follows:
	KPVoltage.Init(0, 33, 40960, "V");
	
	ESP32VINSensePin = esp32VINSensePin;
	pinMode(ESP32VINSensePin, INPUT);
	// Assumes use of 68k/10k voltage divider into an OpAmp buffer to scale to 3.3 V max;
	// accomodates a max VIN of 25.74 V (25.74 V / 4096 * 1000000 = 6284 V/count):
	//ESP32VIN.Init(0, 6284, "V");
	// Test data demonstrates that measurement error varies from 1.18 (18%) at 5.0 V to
	// about 1.04 (4%) at 12.0 V; since supply voltage (VIN) in use is expected to be in a
	// range of about 8.0V (LiPo supply) to 12.0 V (external power supply), we adjust the 
	// MegaGain by a factor of 1.05: 6284 * 1.05 = 6598:
	ESP32VIN.Init(0, 6598, "V");

	CSSMStatus.BME280Status = bme280->begin();

#ifdef _DEBUG_
	if (!CSSMStatus.BME280Status)
	{
		char buf[32];
		_PP(F("\BME280 initialization failed; "));
		snprintf(buf, 31, "Firmware version: %02d.%02d", CSSMStatus.MajorVersion, CSSMStatus.MinorVersion);
		_PL(buf);
	}
	else
	{
		switch (bme280->chipModel())
		{
		case BME280::ChipModel_BME280:
			_PL(F("BME280 sensor fully initialized"));
			break;
		case BME280::ChipModel_BMP280:
			_PL(F("BMP280 sensor initialized; no RH measurement available"));
			break;
		default:
			_PL(F("BMx280 chip not recognized"));
			CSSMStatus.BME280Status = false;
		}
	}
#endif
	
	if (CSSMStatus.BME280Status)
	{
		ReadENVData();
	}

	return CSSMStatus.BME280Status;
}

void CSSMSensorData::Update()
{
	KPVoltage.RawValue = analogRead(KBSensePin);
	ESP32VIN.RawValue = analogRead(ESP32VINSensePin);
}

uint16_t CSSMSensorData::GetKBRaw()
{
	return KPVoltage.RawValue;
}

void CSSMSensorData::ReadENVData()
{
	BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
	BME280::PresUnit presUnit(BME280::PresUnit_hPa);

	bme280->read(BME280Data.Pbaro, BME280Data.Tchip, BME280Data.RH, tempUnit, presUnit);

}


CSSMSensorData SensorData;

