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

constexpr uint8_t defaultRC2x15AAddress = 0x80;
constexpr float GAMMA = 0.10f;
constexpr uint32_t defaultLMotorFullSpeedQPPS = 7500;			// Motor 2
constexpr uint32_t defaultRMotorFullSpeedQPPS = 7500;			// Motor 1

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

	float M1kp = 0.0f;
	float M1ki = 0.0f;
	float M1kd = 0.0f;
	uint32_t M1qpps = defaultLMotorFullSpeedQPPS;
	float M2kp = 0.0f;
	float M2ki = 0.0f;
	float M2kd = 0.0f;
	uint32_t M2qpps = defaultRMotorFullSpeedQPPS;
	
	bool TestInProgress();

protected:
	HardwareSerial* RC2x15AUART;					// Packet serial link to RoboClaw 2x15A Motor Controller
	RoboClaw* RC2x15A;
	uint8_t PSAddress = defaultRC2x15AAddress;				// RoboClaw MC address for Packet Serial communications

	bool calibratingDrive = false;					// Development testing, such as calibrating odometry parameters
	uint64_t CalibrateDriveStartTime = 0;

	bool CalibrateDriveSystem(uint64_t testPeriod);

	float KLTrack = defaultKLTrack;
	float KRTrack = defaultKRTrack;
	float TrackSpan = defaulTrackSpan;
	uint64_t OdometerStartTime = 0;					// ms; used to calculate odometer time
	uint64_t LastOdometryUpdateTime = 0;			// ms; used to integrate dynamic measurements to obtain position and pose estimates
	uint64_t Trip1StartTime = 0;					// ms;
	float Trip1StartDistance = 0.0f;				// m
	uint64_t Trip2StartTime = 0;					// ms;
	float Trip2StartDistance = 0.0f;				// m

public:
	bool Init();
	bool ReadStatus();
	bool ResetUARTLink();
	RoboClaw* GetRC2x15A();
	uint8_t GetPSAddress();

	static void ResetOdometer();
	static void ResetTrip1();
	static void ResetTrip2();

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

