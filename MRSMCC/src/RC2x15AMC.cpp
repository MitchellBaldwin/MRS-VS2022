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

bool RC2x15AMCClass::Init()
{
	char buf[64];
	bool success = false;

	RC2x15AUART = new HardwareSerial(1);			// Establish a new hardware UART port using the MCC MCU UART1 device
	RC2x15A = new RoboClaw(RC2x15AUART, 10000);

	RC2x15AUART->begin(115200, SERIAL_8N1, 18, 17);
	if (!RC2x15AUART)
	{
		MCCStatus.RC2x15AUARTStatus = false;
		_PL("Failed to initialize RC2x15ASerial")
	}
	else
	{
		MCCStatus.RC2x15AUARTStatus = true;

		// Test code:
		//RC2x15AUART.println("Hello from the RC2x15AUART port...");
	}

	// Test UART link to motor controller:
	success = RC2x15A->ReadVersion(PSAddress, buf);
	_PL(buf)

	// Test code:
	//uint32_t maxI = 0;
	//success = RC2x15A->ReadM1MaxCurrent(RC2x15AAddress, maxI);
	//sprintf(buf, "%d %d", success, maxI);
	//_PL(buf)
	//success = RC2x15A->SpeedM1(RC2x15AAddress, 1000);
	//delay(1000);
	//success = RC2x15A->SpeedM1(RC2x15AAddress, 0);
	// End of test code block

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
		// Include a call to Drive in the rotation to ensure, in particular, that stop commands
		//get through:
		Drive(MCCStatus.cssmDrivePacket.Throttle, MCCStatus.cssmDrivePacket.OmegaXY);
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
	//TODO: Re-establich / re-synch UART communication with the motor controller; simply ending and re-beginning
	//the UART does not see to work.  

	char buf[32];
	bool success = false;
	
	//RC2x15AUART->flush();
	RC2x15AUART->end();
	delay(100);
	RC2x15AUART->begin(115200, SERIAL_8N1, 18, 17);
	if (!RC2x15AUART)
	{
		MCCStatus.RC2x15AUARTStatus = false;
		_PL("Failed to initialize RC2x15ASerial")
	}
	else
	{
		MCCStatus.RC2x15AUARTStatus = true;
		// Test UART link to motor controller:
		success = RC2x15A->ReadVersion(PSAddress, buf);
	}
	return success;
}

void RC2x15AMCClass::Update()
{
	// Determine whether drive commands have changed since the last cycle and, if so, update and send
	//the new speed and turn rate settings to the motor controller.  Otherwise, request the next 
	//motor controller parameter update.

	bool success = false;
	float newSpeed = MCCStatus.cssmDrivePacket.Throttle;
	float newTurnRate = MCCStatus.cssmDrivePacket.OmegaXY;
	if (abs(newSpeed - LastSpeedSetting) >= GAMMA || abs(newTurnRate - LastTurnRateSetting) >= GAMMA)
	{
		if (abs(newSpeed) < GAMMA && abs(newTurnRate) < GAMMA)
		{
			//TODO: Determine how to not only stop the motors but cut breaking current, too.
			// Configure pin S3 as "E-Stop" - connecting to GND stops motors and cuts current to nil.
			// Setting speeds to 0 stops the motors but there is a significant residual current draw
			//clicking the "STOP ALL" button in the Basic Micro Motion Studio application cuts
			//current draw to nil
			// SOLUTION: Set duty cycle for both motors to 0:
			success = RC2x15A->DutyM1M2(PSAddress, 0, 0);
			//RC2x15A->SpeedM1M2(PSAddress, 0, 0);
		}
		else
		{
			success = Drive(newSpeed, newTurnRate);
		}
	}
	else
	{
		success = ReadStatus();
	}
	LastSpeedSetting = newSpeed;
	LastTurnRateSetting = newTurnRate;

	MCCStatus.RC2x15AMCStatus = success;
}

/// <summary>
/// Most basic differential drive implementation, 
/// Converts commanded speed and trun rate values into motor speed settings for the left and right drive motors
/// </summary>
/// <param name="speed">
/// Commanded speed as a percent of full speed (-100% to +100%)
/// </param>
/// <param name="turnRate">
/// Commanded turn rate as a percent of maximum possible turn rate (-100% (full right turn) to +100% (full left turn))
/// </param>
/// <returns>
/// Returns success reported by serial communication with the RoboClaw 2x15A motor controller
/// </returns>
bool RC2x15AMCClass::Drive(float speed, float turnRate)
{
	bool success = false;

	//TODO: Convert these to global constants:
	uint32_t lMotorFullSpeedQPPS = 7000;
	uint32_t rMotorFullSpeedQPPS = 7000;

	// Convert commanded speed and turn rate into motor speeds (qpps - quadrature pulses per second)
	int32_t lMotorSpeed = 0 - speed / 100.0f * lMotorFullSpeedQPPS;
	int32_t rMotorSpeed = 0 - speed / 100.0f * rMotorFullSpeedQPPS;
	
	int32_t turnDifferentialQPPS = 0;
	if (abs(speed) > GAMMA)
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

	success = RC2x15A->SpeedM1M2(PSAddress, lMotorSpeed, rMotorSpeed);

	MCCStatus.mcStatus.M1SpeedSetting = lMotorSpeed;
	MCCStatus.mcStatus.M2SpeedSetting = rMotorSpeed;

	return success;
}

RC2x15AMCClass RC2x15AMC;
