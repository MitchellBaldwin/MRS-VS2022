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
#include <PCF8563.h>

class MRSSENStatus
{
protected:


public:
	// MRS Sensors module firmware version:
	uint8_t MajorVersion = 1;
	uint8_t MinorVersion = 0;

	bool LocDispStatus = false;
	bool OTOSStatus = false;
	String OTOSVersion;
	bool FwdVL53L1XStatus = false;
	String FwdVL53L1XSWVersion;
	bool RTCStatus = false;
	Time RTCtime;
	
	MRSSensorPacket mrsSensorPacket;

	bool Init();
	String GetDateTimeString();

};

extern MRSSENStatus mrsSENStatus;

#endif

