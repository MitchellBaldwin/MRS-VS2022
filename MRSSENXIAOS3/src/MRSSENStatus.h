/*	MRSSENStatus.h
*	MRSSENStatus - Class to maintain status information on the MRS Sensors module
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MRSSENStatus_h
#define _MRSSENStatus_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "C:\Repos\MRS-VS2022\MRSCommon\src\MRSSensorPacket.h"

class MRSSENStatus
{
protected:


public:
	// MRS Sensors module firmware version:
	uint8_t MajorVersion = 1;
	uint8_t MinorVersion = 0;

	bool LocDispStatus = false;
	bool OTOSStatus = false;
	bool FwdVL53L1XStatus = false;
	
	MRSSensorPacket mrsSensorPacket;

	bool Init();
};

extern MRSSENStatus mrsSENStatus;

#endif

