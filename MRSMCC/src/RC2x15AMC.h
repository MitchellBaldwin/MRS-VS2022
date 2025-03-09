/* RC2x15AMC.h
* RC2x15AMC class - Container class for working with the RoboClaw 2x 15A Motor Controller
*
*
*
*
* Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _RC2X15AMC_h
#define _RC2X15AMC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <HardwareSerial.h>
#include <RoboClaw.h>

constexpr uint8_t RC2x15AAddress = 0x80;
constexpr float GAMMA = 0.10f;
constexpr uint32_t lMotorFullSpeedQPPS = 7500;			// Motor 2
constexpr uint32_t rMotorFullSpeedQPPS = 7500;			// Motor 1

constexpr float defaultKLTrack = 13.0103;				// qp/mm; encoder quadrature pulse count per mm of track movement
constexpr float defaultKRTrack = 13.0103;				// qp/mm
constexpr float defaulTrackSpan = 185.5;				// mm;  horizontal distance between left and right track center lines

constexpr uint64_t defaultTestDrivePeriod = 15000;		// ms

class RC2x15AMCClass
{
public:
	enum MCParamTypes
	{
		VBAT,
		T1,
		T2,
		IMOT,
		ENCPOS,
		SPEEDS,

		NoType
	};
	MCParamTypes CurrentMCParam = MCParamTypes::VBAT;

	bool TestInProgress();

protected:
	HardwareSerial* RC2x15AUART;					// Packet serial link to RoboClaw 2x15A Motor Controller
	RoboClaw* RC2x15A;
	uint8_t PSAddress = RC2x15AAddress;				// RoboClaw MC address for Packet Serial communications

	bool calibratingDrive = false;					// Development testing, such as calibrating odometry parameters
	uint64_t CalibrateDriveStartTime = 0;

	bool CalibrateDriveSystem(uint64_t testPeriod);

	float KLTrack = defaultKLTrack;
	float KRTrack = defaultKRTrack;
	float TrackSpan = defaulTrackSpan;
	uint64_t OdometerStartTime = 0;					// ms; used to calculate odometer time
	uint64_t LastOdometryUpdateTime = 0;			// ms; used to integrate dynamic measurements to obtain position and pose estimates

public:
	bool Init();
	bool ReadStatus();
	bool ResetUARTLink();

	void ResetOdometer();

	bool DriveSettingsChanged();
	void Update();
	bool Drive(float vf, float wxy);
	bool DriveThrottleTurnRate(float throttle, float turnRate);
	bool DriveLRThrottle(float lThrottle, float rThrottle);
	bool DriveLRTrackSpeed(float leftTrackSpeed, float rightTrackSpeed);
	bool Stop(bool breaking = true);

};

extern RC2x15AMCClass RC2x15AMC;

#endif

