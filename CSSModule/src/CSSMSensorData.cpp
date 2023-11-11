/*	CSSMSensorData.cpp
*	CSSMSensorData - Base class for managing data from sensors on the MRS Joystick board
*
*/

#include "CSSMSensorData.h"
#include "CSSMStatus.h"
#include "DEBUG Macros.h"
#include <math.h>

CSSMSensorData::CSSMSensorData()
{
	CSSMStatus.BME280Status = false;
}

bool CSSMSensorData::Init()
{
	pinMode(KBSensePin, INPUT);
	// Assumes use of a unity gain OpAmp buffer (3.3 V max);
	// 3.3 V / 4096 * 1000000 = 806 uV/count:
	//KPVoltage.Init(0, 806, "V");
	// Can achieve the same result as follows:
	KPVoltage.Init(0, 33, 40960, "V", 16);
	
	pinMode(ThrottleSensePin, INPUT);
	// Assumes use of a potentiometer voltage divider working from 0.0 - 3.3 V (0 - 4095)
	// Return throttle setting in the range -100.0% to +100.0%
	// +/- 100.0% / 2048 = 0.04883 %/count
	ThrottleSetting.Init(2048, 1000, 20480, "%");
	
	pinMode(ESP32VINSensePin, INPUT);
	// Assumes use of 68k/10k voltage divider into an OpAmp buffer to scale to 3.3 V max;
	// accomodates a max VIN of 25.74 V (25.74 V / 4096 * 1000000 = 6284 uV/count):
	//ESP32VIN.Init(0, 6284, "V");
	// Test data demonstrates that measurement error varies from 1.18 (18%) at 5.0 V to
	// about 1.04 (4%) at 12.0 V; since supply voltage (VIN) in use is expected to be in a
	// range of about 8.0V (LiPo supply) to 12.0 V (external power supply), we adjust the 
	// MegaGain by a factor of 1.05: 6284 * 1.05 = 6598:
	ESP32VIN.Init(0, 6598, "V");

	CSSMStatus.BME280Status = ENVData.Init();

	ESP32Encoder::useInternalWeakPullResistors = puType::UP;
	HDGEncoder.attachHalfQuad(HDGEncoderDTPin, HDGEncoderCLKPin);
	HDGEncoder.setCount(0);
	CRSEncoder.attachHalfQuad(CRSEncoderDTPin, CRSEncoderCLKPin);
	CRSEncoder.setCount(0);

	bool success = CSSMStatus.BME280Status && true;
	return success;
}

bool CSSMSensorData::Init(byte kbSensePin, byte throttleSensePin, byte esp32VINSensePin)
{
	KBSensePin = kbSensePin;
	ThrottleSensePin = throttleSensePin;
	ESP32VINSensePin = esp32VINSensePin;

	return Init();
}

void CSSMSensorData::Update()
{
	int newReading = analogRead(KBSensePin);
	KPVoltage.AddReading(newReading);

	newReading = analogRead(ThrottleSensePin);
	ThrottleSetting.AddReading(newReading);
	
	newReading = analogRead(ESP32VINSensePin);
	ESP32VIN.AddReading(newReading);
	
	HDGEncoderSetting = (int)HDGEncoder.getCount() / 2;
	if (HDGEncoderSetting > 359)
	{
		HDGEncoderSetting = 0;
		HDGEncoder.setCount(0);
	}
	else if (HDGEncoderSetting < 0)
	{
		HDGEncoderSetting = 359;
		HDGEncoder.setCount(HDGEncoderSetting * 2);
	}
	
	CRSEncoderSetting = (int)CRSEncoder.getCount() / 2;
	if (CRSEncoderSetting > 359)
	{
		CRSEncoderSetting = 0;
		CRSEncoder.setCount(0);
	}
	else if (CRSEncoderSetting < 0)
	{
		CRSEncoderSetting = 359;
		CRSEncoder.setCount(CRSEncoderSetting * 2);
	}
	
}

uint16_t CSSMSensorData::GetKBRaw()
{
	return KPVoltage.GetAverageRawValue();
}

float CSSMSensorData::GetKBReal()
{
	return KPVoltage.GetAverageRealValue();
}

String CSSMSensorData::GetKPString()
{
	return KPVoltage.GetRealString();
}

String CSSMSensorData::GetKPString(String format)
{
	return KPVoltage.GetRealString(format);
}

float CSSMSensorData::GetThrottleActual()
{
	return ThrottleSetting.GetAverageRealValue();
}

/// <summary>
/// Get throttle setting in the  range [-100%:+100%] with center dead zone
/// </summary>
/// <returns>Throttle setting adjusted for dead zone</returns>
float CSSMSensorData::GetThrottle()
{
	float actual = ThrottleSetting.GetAverageRealValue();

	if (abs(actual) < ThrottleDeadZone)
	{
		return 0.0f;
	}
	if (actual >= ThrottleDeadZone)
	{
		actual = (actual - ThrottleDeadZone) * 100.0f / (100.0f - ThrottleDeadZone);
	}
	else if (actual <= -ThrottleDeadZone)
	{
		actual = (actual + ThrottleDeadZone) * 100.0f / (100.0f - ThrottleDeadZone);
	}
	
	return actual;
}

float CSSMSensorData::GetESP32VINReal()
{
	return ESP32VIN.GetAverageRealValue();
}

String CSSMSensorData::GetESP32VINString()
{
	return ESP32VIN.GetRealString();
}

String CSSMSensorData::GetESP32VINString(String format)
{
	return ESP32VIN.GetRealString(format);
}

void CSSMSensorData::ReadENVData()
{
	ENVData.ReadENVData();
}


CSSMSensorData SensorData;

