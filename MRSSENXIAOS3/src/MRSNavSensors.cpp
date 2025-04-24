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

	OTOS = new QwiicOTOS();
	mrsSENStatus.OTOSStatus = OTOS->begin();
	sfe_otos_version_t hwVersion;
	sfe_otos_version_t fwVersion;
	OTOS->getVersionInfo(hwVersion, fwVersion);
	snprintf(buf, 31, "OTOS HWv%X.%X FWv%X.%X", hwVersion.major, hwVersion.minor, fwVersion.major, fwVersion.minor);
	mrsSENStatus.OTOSVersion = buf;
	_PL(buf)

	// Initialize proximity & distance sensors:
	FwdVL53L1X = new SFEVL53L1X();
	mrsSENStatus.FwdVL53L1XStatus = !FwdVL53L1X->begin();
	if (!mrsSENStatus.FwdVL53L1XStatus)
	{
		_PL("Forward VL53L1X initialization FAILED")
	}
	else
	{
		VL53L1X_Version_t ver = FwdVL53L1X->getSoftwareVersion();
		snprintf(buf, 31, "FwdVL53L1X v%d.%d.%d.%d", ver.major, ver.minor, ver.revision, ver.build);
		mrsSENStatus.FwdVL53L1XSWVersion = buf;
		_PL(buf)
	}

	return true;
}


MRSNavSensors mrsNavSensors;

