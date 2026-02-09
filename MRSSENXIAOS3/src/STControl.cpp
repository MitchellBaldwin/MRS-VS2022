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

		//TestSTMotor();
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

/// <summary>
/// Moves the sensor turret to a specified angle by converting the angle to motor steps and commanding the motor.
/// </summary>
/// <param name="angleInDegrees">
/// Target angle in degrees. Converted to motor steps using StepsPerRev and used as the absolute target position for the motor.
/// </param>
/// <param name="blocking">
/// If true, request the motor move to block until completion; if false, return immediately after issuing the move command.
/// </param>
/// <returns>
/// true if the motor object was initialized and the move command was issued; false if the motor was not initialized (an error is logged in that case).
/// </returns>
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

/// <summary>
/// Moves the sensor turret by a relative angle (in degrees).
/// </summary>
/// <param name="angleInDegrees">
/// Relative rotation to apply, in degrees. May be negative. Converted to motor steps using StepsPerRev and added to the motor's current target position.
/// </param>
/// <param name="blocking">
/// If true, block until the motor finishes the movement; if false, return immediately after issuing the move command.
/// </param>
/// <returns>
/// true if the move command was issued (STMotor was initialized); false if the motor was not initialized and no command was sent.
/// </returns>
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

/// <summary>
/// Stops the sensor turret motor. If hardStop is true, it forces an immediate stop; otherwise it requests a normal/controlled stop. 
/// If the motor is not initialized, an error is logged.
/// </summary>
/// <param name="hardStop">
/// If true, perform an immediate/forced stop; if false, perform a normal/controlled stop.
/// </param>
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

/// <summary>
/// Sets the sensor turret motor speed in steps per second. If the motor (STMotor) is not initialized, logs an error and does nothing.
/// </summary>
/// <param name="speedInStepsPerSec">
/// Desired motor speed in steps per second. If STMotor is initialized, this value is forwarded to STMotor->setSpeedInHz.
/// </param>
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

/// <summary>
/// Sets the acceleration for the Sensor Turret stepper motor.
/// </summary>
/// <param name="accelInStepsPerSec2">
/// Acceleration value in steps per second squared to apply to the motor.
/// </param>
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

/// <summary>
/// Sets the ST motor's speed and acceleration if the motor is initialized; logs an error if the motor is not initialized.
/// </summary>
/// <param name="speedInStepsPerSec">
/// Desired speed to set on STMotor, in steps per second.
/// </param>
/// <param name="accelInStepsPerSec2">
/// Desired acceleration to set on STMotor, in steps per second squared.
/// </param>
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

/// <summary>
/// Sets the sensor turret's scan range in degrees and updates the corresponding step limits.
/// </summary>
/// <param name="minAngleInDegrees">Minimum scan angle in degrees (int32_t). Converted to a step position and stored in STScanLeftLimit.</param>
/// <param name="maxAngleInDegrees">Maximum scan angle in degrees (int32_t). Converted to a step position and stored in STScanRightLimit.</param>
void STControlClass::SetSTScanRange(int32_t minAngleInDegrees, int32_t maxAngleInDegrees)
{
	// Calculate position in steps based on target position in degrees and steps per revolution:
	STScanLeftLimit = (int)(minAngleInDegrees * StepsPerRev / 360.0f);
	STScanRightLimit = (int)(maxAngleInDegrees * StepsPerRev / 360.0f);
	sprintf(buf, "Sensor turret scan range set to: %d° to %d° (%d steps to %d steps)", minAngleInDegrees, maxAngleInDegrees, STScanLeftLimit, STScanRightLimit);
	_PL(buf);
}

/// <summary>
/// Member function that starts the sensor-turret scan: if STMotor is initialized it sets STScanning to true; otherwise it logs an error.
/// </summary>
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

/// <summary>
/// Stops the sensor-turret scanning operation if the motor is initialized; logs an error if the motor is not initialized.
/// </summary>
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

