/*	BME280Data.cpp
*	BME280DataClass - Base class to hold measurements from a BME280 sensor module
*
*/

#include "BME280Data.h"
#include <BME280.h>
#include <DEBUG Macros.h>

#ifdef _DEBUG_
#include "CSSMS3Status.h"
#endif

BME280DataClass::BME280DataClass()
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
}

bool BME280DataClass::Init()
{
	bool success = bme280->begin();

#ifdef _DEBUG_
	if (!success)
	{
		_PP(F("\BME280 initialization failed; "));
		snprintf(buf, 31, "Firmware version: %02d.%02d", CSSMS3Status.MajorVersion, CSSMS3Status.MinorVersion);
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
			success = false;
		}
	}
#endif

	if (success)
	{
		ReadENVData();
	}

	return success;
}

void BME280DataClass::ReadENVData()
{
	BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
	BME280::PresUnit presUnit(BME280::PresUnit_hPa);

	bme280->read(Pbaro, Tchip, RH, tempUnit, presUnit);

}

void BME280DataClass::GetTchipString(String* tempString)
{
	BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
	snprintf(buf, 22, "%7.2f%c%s", Tchip, 0xF7, tempUnit == BME280::TempUnit_Celsius ? "C" : "F");
	*tempString = buf;
	return;
}

void BME280DataClass::GetPbaroString(String* pbaroString)
{
	BME280::PresUnit presUnit(BME280::PresUnit_hPa);
	snprintf(buf, 22, "%7.2f %s", Pbaro, String(presUnit == BME280::PresUnit_hPa ? "hPa" : "Pa"));
	*pbaroString = buf;
	return;
}

void BME280DataClass::GetRHString(String* rhString)
{
	snprintf(buf, 22, "%7.2f%%RH", RH);
	*rhString = buf;
	return;
}

//BME280DataClass BME280Data;

