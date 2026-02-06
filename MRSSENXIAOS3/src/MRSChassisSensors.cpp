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
	WSUPS3SINA219->begin();
	WSUPS3SINA219->setCalibration_32V_2A();	// Configure for 32V, 2A operation
	if (WSUPS3SINA219->getBusVoltage_V() > 0.0)
	{
		mrsSENStatus.INA219Status = true;
		_PL("WS UPS 3S INA219 initialized")
	}
	else
	{
		mrsSENStatus.INA219Status = false;
		_PL("WS UPS 3S INA219 initialization FAILED")
	}


	return true;
}

bool MRSChassisSensorsClass::Update()
{
	mrsSENStatus.mrsSensorPacket.INA219VBus = WSUPS3SINA219->getBusVoltage_V();
	mrsSENStatus.mrsSensorPacket.INA219VShunt = WSUPS3SINA219->getShuntVoltage_mV();
	mrsSENStatus.mrsSensorPacket.INA219Current = WSUPS3SINA219->getCurrent_mA();
	mrsSENStatus.mrsSensorPacket.INA219Power = WSUPS3SINA219->getPower_mW();
	return true;
}

MRSChassisSensorsClass MRSChassisSensors;

