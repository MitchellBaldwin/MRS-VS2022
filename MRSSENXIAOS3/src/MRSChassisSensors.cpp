/*	MRSChassisSensors.h
*	MRSChassisSensorsClass - Class to maintain status information on the MRS Sensors module
*
*	Mitchell Baldwin copyright 2026
*
*/

#include "MRSChassisSensors.h"
#include "DEBUG Macros.h"
#include "MRSSENStatus.h"

bool MRSChassisSensorsClass::Init()
{
	char buf[64];

	WSUPS3SINA219->begin();
	WSUPS3SINA219->setCalibration_32V_2A();	// Configure for 32V, 2A operation
	if (WSUPS3SINA219->getBusVoltage_V() > 0.0)
	{
		mrsSENStatus.INA219Status = true;
		_PL("Right WS UPS 3S INA219 initialized")
	}
	else
	{
		mrsSENStatus.INA219Status = false;
		_PL("Right WS UPS 3S INA219 initialization FAILED")
	}

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

			FwdVL53L1X->startRanging();

	}


	return mrsSENStatus.INA219Status && mrsSENStatus.FwdVL53L1XStatus;
}

bool MRSChassisSensorsClass::Update()
{
	mrsSENStatus.mrsSensorPacket.RINA219VBus = WSUPS3SINA219->getBusVoltage_V();
	mrsSENStatus.mrsSensorPacket.RINA219VShunt = WSUPS3SINA219->getShuntVoltage_mV();
	mrsSENStatus.mrsSensorPacket.RINA219Current = WSUPS3SINA219->getCurrent_mA();
	mrsSENStatus.mrsSensorPacket.RINA219Power = WSUPS3SINA219->getPower_mW();

	// Read forward VL53L1X distance:
	if (FwdVL53L1X->checkForDataReady())
	{
		mrsSENStatus.mrsSensorPacket.FWDVL53L1XRange = FwdVL53L1X->getDistance();
		FwdVL53L1X->clearInterrupt();
	}


	return true;
}

MRSChassisSensorsClass MRSChassisSensors;

