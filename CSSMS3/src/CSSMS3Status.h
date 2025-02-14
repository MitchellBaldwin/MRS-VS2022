/*	CSSMS3Status.h
*	CSSMS3StatusClass - Base class for module level status and parameters
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

constexpr uint8_t MAX_DEBUG_TEXT_LINES = 14;

//#include "C:\Repos\MRS-VS2022\MRSCommon\src\CSSMDrivePacket.h"
#include "..\..\MRSCommon\src\CSSMDrivePacket.h"

class CSSMS3StatusClass
{
protected:
	char buf[64];
	uint8_t curDebugTextLine = 0x00;


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

	uint32_t MRSMCCPacketReceivedCount = 0;
	uint32_t SaveMRSMCCPacketReceivedCount = 0;
	bool MRSMCCESPNOWLinkStatus = false;
	
	bool WiFiStatus = false;
	 
	bool LocalDisplayStatus = false;
	bool DebugDisplayStatus = false;
	 
	bool NavEncoderStatus = false;
	bool FuncEncoderStatus = false;

	bool BME280Status = false;
	
	bool IMUStatus = false;

	String debugTextLines[MAX_DEBUG_TEXT_LINES];

	bool TS2State = false;

	void Init();
	void Update();
	void AddDebugTextLine(String newLine);
	void ClearDebugText();

};

extern CSSMS3StatusClass CSSMS3Status;

#endif

