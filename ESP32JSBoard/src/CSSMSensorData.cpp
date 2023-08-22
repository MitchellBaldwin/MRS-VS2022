/*	CSSMSensorData.cpp
*	CSSMSensorData - Base class for managing data from sensors on the MRS Joystick board
*
*/

#include "CSSMSensorData.h"
#include "CSSMStatus.h"

CSSMSensorData::CSSMSensorData()
{
	bme280 = new BME280I2C();
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
	ESP32VIN.Init(0, 6284, "V");

	CSSMStatus.BME280Status = bme280->begin();

#if defined(_DEBUG_)
	if (!CSSMStatus.BME280Status)
	{
		char buf[32];
		_PP("\BME280 initialization failed; ");
		snprintf(buf, 31, "Firmware version: %02d.%02d", MajorVersion, MinorVersion);
		_PL(buf);
	}
	else
	{
		switch (bme280->chipModel())
		{
		case BME280::ChipModel_BME280:
			_PL("Found BME280 sensor! Success.");
			break;
		case BME280::ChipModel_BMP280:
			_PL("Found BMP280 sensor! No Humidity available.");
			break;
		default:
			_PL("Found UNKNOWN sensor! Error!");
			CSSMStatus.BME280Status = false;
		}
	}
#endif
	
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


CSSMSensorData SensorData;

