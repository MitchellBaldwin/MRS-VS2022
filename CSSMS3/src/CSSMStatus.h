/*	CSSMStatus.h
*	CSSMStatusClass - Base class for module level status and parameters
*
*
*
*
*	Mitchell Baldwin copyright 2023-2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _CSSMSTATUS_h
#define _CSSMSTATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#include "C:\Repos\MRS-VS2022\MRSCommon\src\CSSMDrivePacket.h"
#include "..\..\MRSCommon\src\CSSMDrivePacket.h"

class CSSMStatusClass
{
protected:


public:
	 
	// Control Stick Steering Module firmware version:
	uint8_t MajorVersion = 1;
	uint8_t MinorVersion = 0;

	CSSMDrivePacket::DriveModes LastDriveMode = CSSMDrivePacket::DriveModes::DRV;	// Detect when Drive Mode has changed since last update
	CSSMDrivePacket::DriveModes SavedDriveMode = CSSMDrivePacket::DriveModes::DRV;	// Remember Drive Mode when temporarily overridden

	CSSMDrivePacket cssmDrivePacket;
	
	enum ComModes
	{
		IDCPktSerial,	// COBS encoded packet exchange with MRS RC MCC through UART1 (Default mode)
		WiFiTCP,		// For the CSSM, direct WiFi connection to MRS MCC, bypassing MRS RC MCC 
		ESPNOW,			// For the CSSM, direct ESP-NOW connection with MRS MCC, bypassing MRS RC MCC

		NoComs
	};
	ComModes ComMode = ComModes::NoComs;
	 
	bool UART0Status = false;
	bool UART1Status = false;
	bool UART2Status = false;

	bool ESPNOWStatus = false;
	int ESPNOWPacketSentCount = 0;
	uint16_t SendRetries = 0;

	bool WiFiStatus = false;
	 
	bool LocalDisplayStatus = false;
	bool DebugDisplayStatus = false;
	 
	bool BME280Status = false;
	
	bool IMUStatus = false;

	void Init();
};

extern CSSMStatusClass CSSMStatus;

#endif

