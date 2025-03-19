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
	sprintf(buf, "OTOS HWv%X.%X FWv%X.%X", hwVersion.major, hwVersion.minor, fwVersion.major, fwVersion.minor);
	_PL(buf)

	// Initialize proximity & distance sensors:
	FwdVL53L1X = new SFEVL53L1X();
	mrsSENStatus.FwdVL53L1XStatus = !FwdVL53L1X->begin();
	if (!mrsSENStatus.FwdVL53L1XStatus)
	{
		_PL("Forward VL53L1X initialization FAILED")
	}



	return true;
}


MRSNavSensors mrsNavSensors;

