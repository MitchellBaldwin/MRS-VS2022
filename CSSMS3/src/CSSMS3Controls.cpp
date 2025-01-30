/*	CSSMS3Controls.cpp
*	CSSMS3Controls - Base class for managing data from sensors on the MRS Control Stick Steering Module
*
*/

#include "CSSMS3Controls.h"
#include "CSSMS3Status.h"

uint32_t CSSMS3Controls::ReadCalibratedADC1(int rawADC1)
{
	return esp_adc_cal_raw_to_voltage(rawADC1, &ADC1Chars);
}

/// <summary>
/// Get the latest smoothed (averaged) measurement from the left throttle
/// </summary>
/// <returns>Averaged left throttle measurement</returns>
float CSSMS3Controls::GetLThrottleActual()
{
	return LThrottleSetting.GetAverageRealValue();
}

/// <summary>
/// Get left throttle setting in the range [-100%:+100%] with center dead zone
/// </summary>
/// <returns>Throttle setting adjusted for dead zone</returns>
float CSSMS3Controls::GetLThrottle()
{
	float actual = LThrottleSetting.GetAverageRealValue();

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

/// <summary>
/// Get the latest smoothed (averaged) measurement from the right throttle
/// </summary>
/// <returns>Averaged left throttle measurement</returns>
float CSSMS3Controls::GetRThrottleActual()
{
	return RThrottleSetting.GetAverageRealValue();
}

/// <summary>
/// Get right throttle setting in the range [-100%:+100%] with center dead zone
/// </summary>
/// <returns>Throttle setting adjusted for dead zone</returns>
float CSSMS3Controls::GetRThrottle()
{
	float actual = RThrottleSetting.GetAverageRealValue();

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

bool CSSMS3Controls::Init()
{
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &ADC1Chars);

	pinMode(KPSensePin, INPUT);
	// Assumes use of a unity gain OpAmp buffer (3.3 V max);
	// 3.3 V / 4096 * 1000000 = 806 uV/count:
	KPVoltage.Init(0, 33, 40960, "V", 16);

	pinMode(LThrottlePin, INPUT);
	// Assumes use of a potentiometer voltage divider working from 0.0 - 3.3 V (0 - 4095)
	// Return throttle setting in the range -100.0% to +100.0%
	// +/- 100.0% / 2048 = 0.04883 %/count
	LThrottleSetting.Init(2048, 1000, 20480, "%");

	pinMode(RThrottlePin, INPUT);
	// Assumes use of a potentiometer voltage divider working from 0.0 - 3.3 V (0 - 4095)
	// Return throttle setting in the range -100.0% to +100.0%
	// +/- 100.0% / 2048 = 0.04883 %/count
	RThrottleSetting.Init(2048, 1000, 20480, "%");

	return true;

}

void CSSMS3Controls::Update()
{
	// Test code:
	CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::HDG;

	
	uint16_t newReading = analogRead(LThrottlePin);
	LThrottleSetting.AddReading(newReading);
	CSSMS3Status.cssmDrivePacket.LThrottle = GetLThrottle();

	//Test code:
	//CSSMS3Status.cssmDrivePacket.Speed = CSSMS3Status.cssmDrivePacket.LThrottle;

	newReading = analogRead(RThrottlePin);
	RThrottleSetting.AddReading(newReading);
	CSSMS3Status.cssmDrivePacket.RThrottle = GetRThrottle();

}

CSSMS3Controls cssmS3Controls;

