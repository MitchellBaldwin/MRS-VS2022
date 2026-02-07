/*	MRSNavSensors.cpp
*	MRSNavSensors - Base class for managing setup, taking sensor measurements, and building and communicating
*	packets containing sensor data with the MRS MCC
*
*/

#include "MRSNavSensors.h"
#include "DEBUG Macros.h"
#include "MRSSENStatus.h"

bool MRSNavSensors::Init()
{
	char buf[32];

	// Initialize the odometry sensors:
	OTOS = new QwiicOTOS();
	mrsSENStatus.OTOSStatus = OTOS->begin();
	if (!mrsSENStatus.OTOSStatus)
	{
		_PL("OTOS initialization FAILED")
	}
	else
	{
		sfe_otos_version_t hwVersion;
		sfe_otos_version_t fwVersion;
		OTOS->getVersionInfo(hwVersion, fwVersion);
		snprintf(buf, 31, "OTOS HWv%X.%X FWv%X.%X", hwVersion.major, hwVersion.minor, fwVersion.major, fwVersion.minor);
		mrsSENStatus.OTOSVersion = buf;
		_PP(buf)
		_PP(" ")
		_PL("Calibrating OTOS IMU...")
		OTOS->calibrateImu();
		OTOS->setLinearUnit(kSfeOtosLinearUnitMeters);
		OTOS->setAngularUnit(kSfeOtosAngularUnitDegrees);
		OTOS->resetTracking();
	}

	// Initialize the RTC:
	RTC = new PCF8563();
	RTC->init();
	RTC->stopClock();
	RTC->setHour(0);
	RTC->setMinut(0);
	RTC->setSecond(0);
	RTC->setYear(25);
	RTC->setMonth(4);
	RTC->setDay(25);
	RTC->startClock();
	mrsSENStatus.RTCStatus = RTC->checkClockIntegrity();
	if (!mrsSENStatus.RTCStatus)
	{
		_PL("RTC initialization FAILED")
	}
	else
	{
		_PL(GetDateTimeString())
	}

	return mrsSENStatus.OTOSStatus && mrsSENStatus.RTCStatus;
}

String MRSNavSensors::GetDateTimeString()
{
	char buf[32];

	// Get the current date and time from the RTC:
	mrsSENStatus.RTCtime = RTC->getTime();
	snprintf(buf, 31, "RTC %02d:%02d:%02d %02d-%02d-%02d", mrsSENStatus.RTCtime.hour, mrsSENStatus.RTCtime.minute,
		mrsSENStatus.RTCtime.second, mrsSENStatus.RTCtime.year, mrsSENStatus.RTCtime.month, mrsSENStatus.RTCtime.day);
	
	return String(buf);
}

void MRSNavSensors::Update()
{
	// Read the OTOS IMU data:
	sfe_otos_pose2d_t pose;
	OTOS->getPosition(pose);
	mrsSENStatus.mrsSensorPacket.ODOSPosX = pose.x;
	mrsSENStatus.mrsSensorPacket.ODOSPosY = pose.y;
	mrsSENStatus.mrsSensorPacket.ODOSHdg = pose.h;

	// Update the RTC time:
	mrsSENStatus.RTCtime = RTC->getTime();

}


MRSNavSensors mrsNavSensors;

