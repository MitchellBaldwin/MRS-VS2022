/*	STControl.h
*	STControlClass - Class to maintain status information on the MRS Sensors module
*
*	Mitchell Baldwin copyright 2026
*
*/

#include "STControl.h"
#include "DEBUG Macros.h"
#include "MRSSENStatus.h"

bool STControlClass::Init()
{
	fasEngine.init();
	STMotor = fasEngine.stepperConnectToPin(DefaultTurretStepPin);
	if (STMotor == NULL)
	{
		_PL("Error initializing sensor turret motor...");
	}
	else
	{
		STMotor->setDirectionPin(DefaultTurretDirPin, true, 0);		// true: High counts up (clockwise)
		STMotor->setEnablePin(DefaultTurretEnablePin, true);		// Low active enables TMC2209
		STMotor->setAutoEnable(true);
		_PL("Sensor turret motor initialized successfully");

		TestSTMotor();
	}
	mrsSENStatus.SensorTurretMotorStatus = (STMotor != NULL);

	return mrsSENStatus.SensorTurretMotorStatus;
}

void STControlClass::Update()
{
	mrsSENStatus.mrsSensorPacket.TurretPosition = (int)(STMotor->getCurrentPosition() * 360.0f / StepsPerRev);
	if (STScanning)
	{
		if (!STMotor->isRunning())
		{
			if (STMotor->getCurrentPosition() >= 0)
			{
				STMotor->moveTo(STScanLeftLimit, false);
			}
			else if (STMotor->getCurrentPosition() <0)
			{
				STMotor->moveTo(STScanRightLimit, false);
			}
		}
	}
}

/// <summary>
/// Tests the SensorTurretMotor by configuring it and performing one clockwise and one counter-clockwise revolution while logging progress. 
/// The moves are performed synchronously (blocking).
/// </summary>
/// <returns>true if the motor was initialized and the test moves completed; false if SensorTurretMotor was not initialized.</returns>
bool STControlClass::TestSTMotor()
{
	bool success = false;
	if (STMotor != NULL)
	{
		STMotor->setSpeedInHz(STMotorSpeed);
		STMotor->setAcceleration(STMotorAccel);
		_PL("Rotating sensor turret motor 1 revolution clockwise");
		STMotor->moveTo(1600, true);	// MS1 & MS2 set for 1/8 microstepping => 1600 steps/rev
		_PL("Rotating sensor turret motor 1 revolution counter-clockwise");
		STMotor->moveTo(0, true);
		_PL("Sensor turret motor test complete");
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

/// <summary>
/// Checks that the sensor turret motor is initialized, configures its speed and acceleration, logs the action, 
/// and commands the motor to move to a given position.
/// </summary>
/// <param name="targetPosition">
/// Target position (in motor/encoder units) to move the sensor turret to.
/// </param>
/// <param name="blocking">
/// If true, request a blocking move (wait until the move completes); if false, return immediately after issuing the move command.
/// </param>
/// <returns>
/// True if the SensorTurretMotor was initialized and the move command was issued; 
/// false if the SensorTurretMotor is not initialized.
/// </returns>
bool STControlClass::TestSTMotor(int32_t targetPosition, bool blocking)
{
	bool success = false;
	if (STMotor != NULL)
	{
		STMotor->setSpeedInHz(STMotorSpeed);
		STMotor->setAcceleration(STMotorAccel);
		sprintf(buf, "Testing sensor turret motor move to position %d", targetPosition);
		_PL(buf)
			STMotor->moveTo(targetPosition, blocking);
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

/// <summary>
/// Moves the sensor turret to the specified target position if the turret motor is initialized.
/// </summary>
/// <param name="targetPosition">
/// The desired position for the sensor turret (int32_t).
/// </param>
/// <returns>
/// true if the move command was dispatched (SensorTurretMotor was initialized); 
/// false if the motor was not initialized and no command was issued.
/// </returns>
bool STControlClass::MoveSTInSteps(int32_t targetPosition, bool blocking)
{
	bool success = false;
	if (STMotor != NULL)
	{
		STMotor->moveTo(targetPosition, blocking);
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

bool STControlClass::MoveST(int32_t angleInDegrees, bool blocking)
{
	bool success = false;

	// Calculate position in steps based on target position in degrees and steps per revolution:
	int targetPositionInSteps = (int)(angleInDegrees * StepsPerRev / 360.0f);
	if (STMotor != NULL)
	{
		STMotor->moveTo(targetPositionInSteps, blocking);
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

bool STControlClass::MoveSTRelative(int32_t angleInDegrees, bool blocking)
{
	bool success = false;

	// Calculate position in steps based on target position in degrees and steps per revolution:
	int targetAngleInSteps = (int)(angleInDegrees * StepsPerRev / 360.0f) + STMotor->getPositionAfterCommandsCompleted();
	if (STMotor != NULL)
	{
		STMotor->moveTo(targetAngleInSteps, blocking);
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

void STControlClass::StopST(bool hardStop)
{
	if (STMotor != NULL)
	{
		if (hardStop)
		{
			STMotor->forceStop();
		}
		else
		{
			STMotor->stopMove();
		}
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
}

void STControlClass::SetSTSpeed(uint32_t speedInStepsPerSec)
{
	if (STMotor != NULL)
	{
		STMotor->setSpeedInHz(speedInStepsPerSec);
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
}

void STControlClass::SetSTAccel(uint32_t accelInStepsPerSec2)
{
	if (STMotor != NULL)
	{
		STMotor->setAcceleration(accelInStepsPerSec2);
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
}

void STControlClass::SetSTSpeedAndAccel(uint32_t speedInStepsPerSec, uint32_t accelInStepsPerSec2)
{
	if (STMotor != NULL)
	{
		STMotor->setSpeedInHz(speedInStepsPerSec);
		STMotor->setAcceleration(accelInStepsPerSec2);
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
}

void STControlClass::SetSTScanRange(int32_t minAngleInDegrees, int32_t maxAngleInDegrees)
{
	// Calculate position in steps based on target position in degrees and steps per revolution:
	STScanLeftLimit = (int)(minAngleInDegrees * StepsPerRev / 360.0f);
	STScanRightLimit = (int)(maxAngleInDegrees * StepsPerRev / 360.0f);
	sprintf(buf, "Sensor turret scan range set to: %d° to %d° (%d steps to %d steps)", minAngleInDegrees, maxAngleInDegrees, STScanLeftLimit, STScanRightLimit);
	_PL(buf);
}

void STControlClass::StartSTScan()
{
	if (STMotor != NULL)
	{
		STScanning = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
}

void STControlClass::StopSTScan()
{
	if (STMotor != NULL)
	{
		STScanning = false;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
}


STControlClass STControl;

