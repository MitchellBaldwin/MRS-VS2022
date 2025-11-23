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

#include "C:\Repos\MRS-VS2022\MRSCommon\src\CSSMDrivePacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\CSSMCommandPacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\RC2x15AMCStatusPacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\MRSStatusPacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\MRSSensorPacket.h"

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
	RC2x15AMCStatusPacket mcStatus;
	MRSStatusPacket mrsStatusPacket;
	MRSSensorPacket mrsSensorPacket;

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

	bool ESPNOWStatus = false;					// Controls CSSM telemetry (downlink) broadcast over ESP-NOW; during start-up this flag
												//is used to indicate success initializing ESP-NOW
	int ESPNOWPacketSentCount = 0;				// Running count of command packets successfully sent to the MRS MCC
	uint16_t SendRetries = 0;					// Count of contiguous retries sending command packets to the MRS MCC

	//uint8_t MRSMCCMAC[] = { 0xF0, 0xF5, 0xBD, 0x42, 0xB7, 0x78 };		// MRS Dev1
	//uint8_t MRSMCCMAC[] = { 0x48, 0x27, 0xE2, 0xEA, 0xCA, 0x4C };		// Breadboard prototype
	uint8_t MRSMCCMAC[6] = { 0xF0, 0xF5, 0xBD, 0x42, 0xB7, 0x78 };

	uint32_t MRSMCCPacketReceivedCount = 0;		// Running count of telemetry packets received from the MRS MCC (not used?)
	uint32_t SaveMRSMCCPacketReceivedCount = 0;	// (not used?)
	uint64_t MCCPacketReceiptInterval = 0;		// Time in ms between receipt of the last two telemetry packets from the MRS MCC 
	uint64_t LastMCCPacketReceivedTime = 0;		// Used to calculate interval between receipt of telemetry packets from he MRS MCC
	bool MRSMCCESPNOWLinkStatus = false;		// Flag indicating the state of health of the telemetry uplink from the MRS to the CSSM
	
	bool WiFiStatus = false;
	 
	bool LocalDisplayStatus = false;
	bool SysDrvDisplayState = false;			// 0/false: display systems pages; 1/true: display drive mode pages
	 
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

