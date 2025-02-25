/* RC2x15AMC.h
* RC2x15AMC class - Container class for working with the RoboClaw 2x 15A Motor Controller
*
* Mitchell Baldwin copyright 2024
* 
*/

#include "DEBUG Macros.h"
#include "RC2x15AMC.h"
#include "MCCStatus.h"
#include <math.h>

bool RC2x15AMCClass::TestInProgress()
{
	return calibratingDrive;
}

bool RC2x15AMCClass::CalibrateDriveSystem(uint64_t testPeriod)
{
	if (!calibratingDrive)	// Start of a new test sequence?
	{
		calibratingDrive = true;
		CalibrateDriveStartTime = millis();
		RC2x15A->ResetEncoders(RC2x15AAddress);

		return DriveThrottleTurnRate(25.0f, 0.0f);
	}
	else
	{
		uint64_t elapsedTime = millis() - CalibrateDriveStartTime;
		if (elapsedTime >= testPeriod)	// End of test?
		{
			char buf[64]{};
			uint32_t data3, data4;
			bool success = RC2x15A->ReadEncoders(PSAddress, data3, data4);
			if (success)
			{
				MCCStatus.mcStatus.M1Encoder = data3;
				MCCStatus.mcStatus.M2Encoder = data4;
			}
			MCCStatus.mcStatus.ENCPOSValid = success;
			sprintf(buf, "L %d qp  R %d qp  in %d ms", data3, data4, elapsedTime);
			MCCStatus.AddDebugTextLine(buf);
			_PP("Drive system calibration results: ")
			_PL(buf)
			calibratingDrive = false;

			return RC2x15A->DutyM1M2(PSAddress, 0, 0);
		}
		else
		{
			return true;
		}
	}
}

bool RC2x15AMCClass::Init()
{
	char buf[64];
	bool success = false;

	RC2x15AUART = new HardwareSerial(1);			// Establish a new hardware UART port using the MCC MCU UART1 device
	RC2x15A = new RoboClaw(RC2x15AUART, 10000);
	
	// The following takes the place of a call to RC2x15A->begin(), which assumes use of the primary Serial port:
	RC2x15AUART->begin(115200, SERIAL_8N1, 18, 17);
	if (!RC2x15AUART)
	{
		MCCStatus.RC2x15AUARTStatus = false;
		_PL("Failed to initialize RC2x15ASerial")
	}
	else
	{
		MCCStatus.RC2x15AUARTStatus = true;
		_PL("RC2x15AUART initialized")

		// Test UART link to motor controller:
		success = RC2x15A->ReadVersion(PSAddress, buf);
		_PL(buf)

		// Test code:
		//RC2x15AUART.println("Hello from the RC2x15AUART port...");
		//RC2x15A->
	}

	KLTrack = defaultKLTrack;
	KRTrack = defaultKRTrack;
	TrackSpan = defaulTrackSpan;

	return success;
}

/// <summary>
/// Cycle through motor controller parameter list, reading one parameter category each cycle
/// 
/// </summary>
/// <returns>
/// Returns true if parameter read(s) succeed, false if not
/// </returns>
bool RC2x15AMCClass::ReadStatus()
{
	uint16_t data1, data2;
	uint32_t data3, data4;
	int16_t data5, data6;
	bool success = false;
	uint8_t status[32];

	// Cycle to the next motor controller measurement in the list:
	if (CurrentMCParam >= MCParamTypes::NoType)
	{
		CurrentMCParam = MCParamTypes::VBAT;
	}
	else
	{
		int param = CurrentMCParam;
		param++;
		CurrentMCParam = (MCParamTypes)param;
	}

	switch (CurrentMCParam)
	{
	case MCParamTypes::VBAT:
		data1 = RC2x15A->ReadMainBatteryVoltage(PSAddress, &success);
		if (success)
		{
			MCCStatus.mcStatus.SupBatV = data1 / 10.0;
		}
		MCCStatus.mcStatus.VBATValid = success;
		break;
	case MCParamTypes::T1:
		success = RC2x15A->ReadTemp(PSAddress, data1);
		if (success)
		{
			MCCStatus.mcStatus.Temp1 = data1 / 10.0;
		}
		MCCStatus.mcStatus.T1Valid = success;
		break;
	case MCParamTypes::T2:
		success = RC2x15A->ReadTemp2(PSAddress, data1);
		if (success)
		{
			MCCStatus.mcStatus.Temp2 = data1 / 10.0;
		}
		MCCStatus.mcStatus.T2Valid = success;
		break;
	case MCParamTypes::IMOT:
		success = RC2x15A->ReadCurrents(PSAddress, data5, data6);
		if (success)
		{
			MCCStatus.mcStatus.M1Current = (float)data5 / 100.0f;
			MCCStatus.mcStatus.M2Current = (float)data6 / 100.0f;
		}
		MCCStatus.mcStatus.IMOTValid = success;
		break;
	case MCParamTypes::ENCPOS:
		success = RC2x15A->ReadEncoders(PSAddress, data3, data4);
		if (success)
		{
			MCCStatus.mcStatus.M1Encoder = data3;
			MCCStatus.mcStatus.M2Encoder = data4;
		}
		MCCStatus.mcStatus.ENCPOSValid = success;
		break;
	case MCParamTypes::SPEEDS:
		data3 = RC2x15A->ReadSpeedM1(PSAddress, status, &success);
		data4 = RC2x15A->ReadSpeedM2(PSAddress, status, &success);
		if (success)
		{
			MCCStatus.mcStatus.M1Speed = data3;
			MCCStatus.mcStatus.M2Speed = data4;
		}
		MCCStatus.mcStatus.SPEEDSValid = success;
		break;
	case MCParamTypes::NoType:
		// Include a call to DriveThrottleTurnRate in the rotation to ensure, in particular, that stop commands
		//get through:
		//success = DriveThrottleTurnRate(MCCStatus.cssmDrivePacket.Speed, MCCStatus.cssmDrivePacket.OmegaXY);
		success = true;
		break;
	default:
		success = true;
		break;
	}

	//return true;
	return success;
}

bool RC2x15AMCClass::ResetUARTLink()
{
	//DONE: Re-establich / re-synch UART communication with the motor controller; simply ending and re-beginning
	//the UART does not see to work.  
	// Clearing the incomming buffer by reading all available data and then attempting to read the RC2x15A version info causes a guru meditation error (MCU reset),
	//but folowing the MCU reset everything seems to work OK...

	char buf[64];
	bool success = false;
	
	// Clear any incomming data:
	while (RC2x15AUART->available())
	{
		RC2x15AUART->read();
	}

	//_PL("RC2x15AUART re-initializing")

	// Test UART link to motor controller:
	RC2x15AUART->flush(true);
	success = RC2x15A->ReadVersion(PSAddress, buf);
	//_PL(buf)

	//RC2x15AUART->end();
	//delay(100);
	//RC2x15AUART->begin(115200, SERIAL_8N1, 18, 17);
	//if (!RC2x15AUART)
	//{
	//	MCCStatus.RC2x15AUARTStatus = false;
	//	_PL("Failed to initialize RC2x15ASerial")
	//}
	//else
	//{
	//	MCCStatus.RC2x15AUARTStatus = true;
	//	// Test UART link to motor controller:
	//	success = RC2x15A->ReadVersion(PSAddress, buf);
	//}

	return success;
}

/// <summary>
///	Determine whether drive commands have changed since the last cycle and, if so, update and send
///	the new speed and turn rate settings to the motor controller.  Otherwise, request the next 
///	motor controller parameter update.
/// /// </summary>
/// <returns>
/// true if any tested field of the drive command packet changed since the last cycle; otherwise false
/// </returns>
bool RC2x15AMCClass::DriveSettingsChanged()
{
	return (MCCStatus.cssmDrivePacket.DriveMode != MCCStatus.lastCSSMDrivePacket.DriveMode
			|| abs(MCCStatus.cssmDrivePacket.SpeedSetting - MCCStatus.lastCSSMDrivePacket.SpeedSetting) >= GAMMA 
			|| abs(MCCStatus.cssmDrivePacket.OmegaXYSetting - MCCStatus.lastCSSMDrivePacket.OmegaXYSetting) >= GAMMA
			|| abs(MCCStatus.cssmDrivePacket.SpeedSettingPct - MCCStatus.lastCSSMDrivePacket.SpeedSettingPct) >= GAMMA 
			|| abs(MCCStatus.cssmDrivePacket.OmegaXYSettingPct - MCCStatus.lastCSSMDrivePacket.OmegaXYSettingPct) >= GAMMA
			|| abs(MCCStatus.cssmDrivePacket.LThrottle - MCCStatus.lastCSSMDrivePacket.LThrottle) >= GAMMA
			|| abs(MCCStatus.cssmDrivePacket.RThrottle - MCCStatus.lastCSSMDrivePacket.RThrottle) >= GAMMA);
}

void RC2x15AMCClass::Update()
{
	bool success = false;

	if (calibratingDrive)
	{
		success = CalibrateDriveSystem(defaultTestDrivePeriod);
	}

	if (DriveSettingsChanged())
	{
		// Determine the Drive method to use based on the current DriveMode:
		switch (MCCStatus.cssmDrivePacket.DriveMode)
		{
		case CSSMDrivePacket::DriveModes::DRV:
			// SpeedSetting in ±mm/s and OmegaXYSetting in ±mrad/s:
			success = Drive(MCCStatus.cssmDrivePacket.SpeedSetting, MCCStatus.cssmDrivePacket.OmegaXYSetting);
			break;
		case CSSMDrivePacket::DriveModes::HDG:
			success = CalibrateDriveSystem(defaultTestDrivePeriod);
			break;
		case CSSMDrivePacket::DriveModes::WPT:

			break;
		case CSSMDrivePacket::DriveModes::SEQ:

			break;
		case CSSMDrivePacket::DriveModes::DRVLR:
			success = DriveLRThrottle(MCCStatus.cssmDrivePacket.LThrottle, MCCStatus.cssmDrivePacket.RThrottle);
			break;
		case CSSMDrivePacket::DriveModes::DRVTw:
			//// SpeedSetting in ±mm/s and OmegaXYSetting in ±mrad/s:
			//success = Drive(MCCStatus.cssmDrivePacket.SpeedSetting, MCCStatus.cssmDrivePacket.OmegaXYSetting);
			//DONE: Add fields to CSSMDrivePacket to maintain speed and turn rate settings in ±% of full
			// SpeedSetting in ±% and OmegaXYSetting in ±%:
			success = DriveThrottleTurnRate(MCCStatus.cssmDrivePacket.SpeedSettingPct, MCCStatus.cssmDrivePacket.OmegaXYSettingPct);
			break;
		default:
			success = ReadStatus();
		}
	}
	else
	{
		success = ReadStatus();
	}
	
	//TODO: Check that M1 is the Left motor and M2 is the right motor
	
	// Calculate ground dynamics based on motor odometry:
	MCCStatus.mcStatus.GroundSpeed = ((float)(MCCStatus.mcStatus.M1Speed) / KLTrack + (float)(MCCStatus.mcStatus.M2Speed) / KRTrack) / 2.0f;	// ±mm/s
	MCCStatus.mcStatus.TurnRate = ((float)(MCCStatus.mcStatus.M2Speed) / KRTrack - (float)(MCCStatus.mcStatus.M1Speed) / KLTrack) / TrackSpan;	// ±rad/s

	// Estimate heading by integrating turn rate:
	uint64_t timeNow = millis();
	float hdg = MCCStatus.mcStatus.Heading;
	hdg += MCCStatus.mcStatus.TurnRate * (float)(timeNow - MCCStatus.mcStatus.LastOdometryUpdateTime) * 180.0f / (1000.0 * PI);
	if (hdg > 360.0f)
	{
		hdg -= 360.0f;
	}
	else if (hdg < 0.0f)
	{
		hdg += 360.0f;
	}
	MCCStatus.mcStatus.Heading = hdg;

	MCCStatus.mcStatus.LastOdometryUpdateTime = timeNow;

	// Save drive settings to test for changes next cycle:
	MCCStatus.lastCSSMDrivePacket = MCCStatus.cssmDrivePacket;	// Using default C++ copy mechanism

	if (!success)	// Failure communicating with motor controller?
	{
		// If so try reconnecting the serial link:
		success = ResetUARTLink();
	}

	MCCStatus.RC2x15AUARTStatus = success;
}

/// <summary>
/// Standard drive implementation given commanded ground speed and turn rate
/// Converts commanded speed and trun rate values into motor speed settings for the left and right drive motors
/// Implements reverse kinematic equations to turn speed and turn rate settings from navigation systems into the
/// motor controller commands needed to move the MRS to the navigation target
/// </summary>
/// <param name="speed">
/// Commanded speed of MRS drive center point in ±mm/s
/// </param>
/// <param name="turnRate">
/// Commanded turn rate about the instantaneous center of curvature (ICC) in ±rad/s
/// </param>
/// <returns>
/// Returns success reported by serial communication with the RoboClaw 2x15A motor controller
///	</returns>
bool RC2x15AMCClass::Drive(float vf, float wxy)
{
	bool success = false;

	// Left and right motor speed settings in qp/s:
	float wLSet = vf * KLTrack;
	float wRSet = vf * KRTrack;

	wLSet -= wxy * TrackSpan * KLTrack / 2.0f;
	wRSet += wxy * TrackSpan * KRTrack / 2.0f;

	int32_t lMotorSpeed = wLSet;
	int32_t rMotorSpeed = wRSet;

	if (abs(lMotorSpeed) < 1 && abs(rMotorSpeed) < 1)
	{
		success = RC2x15A->DutyM1M2(PSAddress, 0, 0);
	}
	else
	{
		success = RC2x15A->SpeedM1M2(PSAddress, lMotorSpeed, rMotorSpeed);
	}

	MCCStatus.mcStatus.M1SpeedSetting = lMotorSpeed;
	MCCStatus.mcStatus.M2SpeedSetting = rMotorSpeed;

	return success;
}

/// <summary>
/// Simple differential drive implementation
/// Converts commanded throttle and trun rate values, each as a percentage of their full speed values, 
/// into motor speed settings for the left and right drive motors
/// </summary>
/// <param name="throttle">
/// Commanded speed as a percent of full speed (-100% to +100%)
/// </param>
/// <param name="turnRate">
/// Commanded turn rate as a percent of maximum possible turn rate (-100% (full right turn) to +100% (full left turn))
/// </param>
/// <returns>
/// Returns success reported by serial communication with the RoboClaw 2x15A motor controller
/// </returns>
bool RC2x15AMCClass::DriveThrottleTurnRate(float throttle, float turnRate)
{
	bool success = false;

	// Convert commanded speed and turn rate into motor speeds (qpps - quadrature pulses per second)
	int32_t lMotorSpeed = throttle / 100.0f * lMotorFullSpeedQPPS;
	int32_t rMotorSpeed = throttle / 100.0f * rMotorFullSpeedQPPS;
	
	int32_t turnDifferentialQPPS = 0;
	if (abs(throttle) > GAMMA)
	{
		// Blend commanded turn rate with forward / backward commanded speed:
		int32_t avgMotorSpeedQPPS = (lMotorSpeed + rMotorSpeed) / 2;
		turnDifferentialQPPS = avgMotorSpeedQPPS * (turnRate / 100.0f);
	}
	else
	{
		// Forward / backward commanded speed is nil so use commanded turn rate to spin in place
		//at a rate proportional to the commanded turn rate:
		uint32_t avgMotorFullSpeedQPPS = (lMotorFullSpeedQPPS + rMotorFullSpeedQPPS) / 2;
		turnDifferentialQPPS = avgMotorFullSpeedQPPS * (turnRate / 100.0f);
	}
	
	// Add turn rate values to each motor speed setting:
	lMotorSpeed -= turnDifferentialQPPS;
	rMotorSpeed += turnDifferentialQPPS;

	if (abs(lMotorSpeed) < 1 && abs(rMotorSpeed) < 1)
	{
		success = RC2x15A->DutyM1M2(PSAddress, 0, 0);
	}
	else
	{
		success = RC2x15A->SpeedM1M2(PSAddress, lMotorSpeed, rMotorSpeed);
	}

	MCCStatus.mcStatus.M1SpeedSetting = lMotorSpeed;
	MCCStatus.mcStatus.M2SpeedSetting = rMotorSpeed;

	return success;
}

/// <summary>
/// Most basic differential drive implementation
/// Converts commanded left and right throttle settings, each as a percentage of their full speed values, 
/// into motor speed settings for the left and right drive motors
/// </summary>
/// <param name="lThrottle">
/// Left throttle setting as a percent of full throttle (-100% to +100%)
/// </param>
/// <param name="rThrottle">
/// Right throttle setting as a percent of full throttle (-100% to +100%)
/// </param>
/// <returns>
/// Returns success reported by serial communication with the RoboClaw 2x15A motor controller
/// </returns>
bool RC2x15AMCClass::DriveLRThrottle(float lThrottle, float rThrottle)
{
	bool success = false;

	// Convert provided throttle settings into motor speeds (qpps - quadrature pulses per second)
	int32_t lMotorSpeed = lThrottle / 100.0f * lMotorFullSpeedQPPS;
	int32_t rMotorSpeed = rThrottle / 100.0f * rMotorFullSpeedQPPS;

	if (abs(lMotorSpeed) < 1 && abs(rMotorSpeed) < 1)
	{
		success = RC2x15A->DutyM1M2(PSAddress, 0, 0);
	}
	else
	{
		success = RC2x15A->SpeedM1M2(PSAddress, rMotorSpeed, lMotorSpeed);
	}

	MCCStatus.mcStatus.M1SpeedSetting = rMotorSpeed;
	MCCStatus.mcStatus.M2SpeedSetting = lMotorSpeed;

	return success;
}

bool RC2x15AMCClass::DriveLRTrackSpeed(float leftTrackSpeed, float rightTrackSpeed)
{
	bool success = false;

	return success;
}

RC2x15AMCClass RC2x15AMC;
