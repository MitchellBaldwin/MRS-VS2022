/*	STControl.h
*	STControlClass - Class to maintain status information on the MRS Sensors module
*
*
*
*	Mitchell Baldwin copyright 2026
*
*	v 0.00:	Initial data structure
*	v
*
*/
#ifndef _STCONTROL_h
#define _STCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
//#include <AVRStepperPins.h>
//#include <common.h>
#include <FastAccelStepper.h>
//#include <PoorManFloat.h>
//#include <RampCalculator.h>
//#include <RampConstAcceleration.h>
//#include <RampGenerator.h>
//#include <StepperISR.h>
//#include <test_probe.h>
constexpr int DefaultTurretStepPin = GPIO_NUM_7;	// TMC2209 Step pin
constexpr int DefaultTurretDirPin = GPIO_NUM_8;		// TMC2209 Direction pin
constexpr int DefaultTurretEnablePin = GPIO_NUM_9;	// TMC2209 Enable pin (Low active); optionally can be connected to GND for always enabled

class STControlClass
{
protected:
	char buf[128];

	FastAccelStepperEngine fasEngine = FastAccelStepperEngine();
	FastAccelStepper* STMotor = NULL;
	int StepsPerRev = 1600;					// MS1 & MS2 set for 1/8 microstepping => 1600 steps/rev
	uint32_t STMotorSpeed = 400;			// Speed in steps/s
	uint32_t STMotorAccel = 50;				// Acceleration in steps/s^2
	bool STScanning = false;				// True when sensor turret is performing a scan between STScanLeftLimit and STScanRightLimit
	uint32_t STScanLeftLimit = 0;			// Sensor turret scan left limit in steps; 0 corresponds to 0° position
	uint32_t STScanRightLimit = 1600;		// Sensor turret scan right limit in steps; 1600 corresponds to 360° position

public:
	bool Init();
	void Update();
	bool TestSTMotor();
	bool TestSTMotor(int32_t targetPosition, bool blocking);
	bool MoveSTInSteps(int32_t targetPosition, bool blocking = false);
	bool MoveST(int32_t angleInDegrees, bool blocking = false);
	bool MoveSTRelative(int32_t angleInDegrees, bool blocking = false);
	void StopST(bool hardStop = false);
	void SetSTSpeed(uint32_t speedInStepsPerSec);
	void SetSTAccel(uint32_t accelInStepsPerSec2);
	void SetSTSpeedAndAccel(uint32_t speedInStepsPerSec, uint32_t accelInStepsPerSec2);
	void SetSTScanRange(int32_t minAngleInDegrees, int32_t maxAngleInDegrees);
	void StartSTScan();
	void StopSTScan();
	bool IsSTScanning() { return STScanning; }
	bool MoveToHomePosition(bool blocking = false) { return MoveST(0, blocking); }

};

extern STControlClass STControl;

#endif

