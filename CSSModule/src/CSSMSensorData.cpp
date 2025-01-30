/*	CSSMSensorData.cpp
*	CSSMSensorData - Base class for managing data from sensors on the MRS Joystick board
*
*/

#include "CSSMSensorData.h"
#include "CSSMStatus.h"
#include <DEBUG Macros.h>

#include <math.h>

uint32_t CSSMSensorData::ReadCalibratedADC1(int rawADC1)
{
	return  esp_adc_cal_raw_to_voltage(rawADC1, &ADC1Chars);
}

CSSMSensorData::CSSMSensorData()
{
	CSSMStatus.BME280Status = false;
}

bool CSSMSensorData::Init()
{
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &ADC1Chars);
	
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
	// With 5.127 V indicated pn precision DMM, this scale factor results in an indicated
	//measurement of 4.45 V (13.2% error)
	//
	// Update: Using ESP32 efuse calibration data:
	// With 5.00 V supplied, system displays 6.48 V with neutral gain
	//ESP32VIN.Init(0, 10000, "V");
	// so adjust megain to 10000 * 5.0 / 6.48 = 7716
	ESP32VIN.Init(0, 7716, "V");

	CSSMStatus.BME280Status = ENVData.Init();

	ESP32Encoder::useInternalWeakPullResistors = puType::UP;
	HDGEncoder.attachHalfQuad(HDGEncoderDTPin, HDGEncoderCLKPin);
	HDGEncoder.setCount(0);
	CRSEncoder.attachHalfQuad(CRSEncoderDTPin, CRSEncoderCLKPin);
	CRSEncoder.setCount(0);

	LeftToggleSwitch = new ezButton(LeftToggleSwitchPin);
	CenterToggleSwitch = new ezButton(CenterToggleSwitchPin);
	RightToggleSwitch = new ezButton(RightToggleSwitchPin);
	RightRockerSwitch = new ezButton(RightRockerSwitchPin);

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

bool CSSMSensorData::Init(byte kbSensePin, byte throttleSensePin, byte esp32VINSensePin, byte leftToggleSwitchPin, byte centerToggleSwitchPin, byte rightToggleSwitchPin, byte rightRockerSwitchPin)
{
	LeftToggleSwitchPin = leftToggleSwitchPin;
	CenterToggleSwitchPin = centerToggleSwitchPin;
	RightToggleSwitchPin = rightToggleSwitchPin;
	RightRockerSwitchPin = rightRockerSwitchPin;

	return Init(kbSensePin, throttleSensePin, esp32VINSensePin);
}

void CSSMSensorData::Update()
{
	int newReading = analogRead(KBSensePin);
	KPVoltage.AddReading(newReading);

	newReading = analogRead(ThrottleSensePin);
	ThrottleSetting.AddReading(newReading);
	CSSMStatus.cssmDrivePacket.Speed = GetThrottle();
	
	newReading = analogRead(ESP32VINSensePin);
	newReading = ReadCalibratedADC1(newReading);
	ESP32VIN.AddReading(newReading);
	
	if (CSSMStatus.cssmDrivePacket.DriveMode != CSSMStatus.LastDriveMode)
	{
		// Drive mode changed since last update so configure HDG encoder appropriately:
		if (CSSMStatus.cssmDrivePacket.DriveMode == CSSMDrivePacket::DriveModes::DRV)
		{
			// Set function of HDG encoder to control turn rate:
			HDGEncoder.setCount(0);
		}
		else
		{
			// Return function of HDG encoder to normal:
			HDGEncoder.setCount(CSSMStatus.cssmDrivePacket.HeadingSetting * 2);
			// Ensure commanded turn rate is set back to 0.0 in case we are exiting direct drive mode:
			CSSMStatus.cssmDrivePacket.OmegaXY = 0.0;
		}
		CSSMStatus.LastDriveMode = CSSMStatus.cssmDrivePacket.DriveMode;
	}
	
	if (CSSMStatus.cssmDrivePacket.DriveMode == CSSMDrivePacket::DriveModes::DRV)
	{
		float turnRate = (float)HDGEncoder.getCount() * 5.0f;
		if (turnRate > 100.0)
		{
			turnRate = 100.0f;
		}
		else if (turnRate < -100.0)
		{
			turnRate = -100.0f;
		}
		CSSMStatus.cssmDrivePacket.OmegaXY = turnRate;
	}
	else
	{
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
		CSSMStatus.cssmDrivePacket.HeadingSetting = HDGEncoderSetting;
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
	CSSMStatus.cssmDrivePacket.CourseSetting = CRSEncoderSetting;
	
	LeftToggleSwitch->loop();
	int newState = LeftToggleSwitch->getState();
	if (LeftToggleSwitchState != newState)
	{
		LeftToggleSwitchState = newState;
		// Execute callback function when state has changed:
		if (LeftToggleSwitchHandler != nullptr)
		{
			LeftToggleSwitchHandler(0x00);
		}
		else
		{
			_PL(LeftToggleSwitchState);
		}
	}
	CenterToggleSwitch->loop();
	newState = CenterToggleSwitch->getState();
	if (CenterToggleSwitchState != newState)
	{
		CenterToggleSwitchState = newState;
		_PL(CenterToggleSwitchState);
	}
	RightToggleSwitch->loop();
	newState = RightToggleSwitch->getState();
	if (RightToggleSwitchState != newState)
	{
		RightToggleSwitchState = newState;
		_PL(RightToggleSwitchState);
	}
	RightRockerSwitch->loop();
	newState = RightRockerSwitch->getState();
	newState = newState?0:1;	// Need to reverse sense of state for rocker switch
	if (RightRockerSwitchState != newState)
	{
		RightRockerSwitchState = newState;
		_PL(RightRockerSwitchState);
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
/// Get throttle setting in the range [-100%:+100%] with center dead zone
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
	
	//CSSMStatus.cssmDrivePacket.Throttle = actual;

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

int CSSMSensorData::GetRightRockerSwitchStateRaw()
{
	return digitalRead(RightRockerSwitchPin);
}


CSSMSensorData SensorData;

