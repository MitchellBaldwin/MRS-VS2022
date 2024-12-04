/*	MCCStatus.h
*	MCCStatusClass - Base class for module level status and parameters
*
*
*
*
*	Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _MCCSTATUS_h
#define _MCCSTATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "C:\Repos\MRS-VS2022\MRSCommon\src\CSSMDrivePacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\RC2x15AMCStatusPacket.h"

constexpr uint8_t MAX_TEXT_LINES = 8;

class MCCStatusClass
{
 protected:
	 char buf[32];
	 uint8_t curDebugTextLine = 0x00;

 public:
	 // MRS MCC firmware version:
	 uint8_t MajorVersion = 1;
	 uint8_t MinorVersion = 0;

	 enum ComModes
	 {
		 IDCPktSerial,	// COBS encoded packet exchange with MRS RC MCC through UART2 (Default mode)
		 WiFiTCP,		// For the CSSM, direct WiFi connection to MRS MCC, bypassing MRS RC MCC 
		 ESPNOW,		// For the CSSM, direct ESP-NOW connection with MRS MCC, bypassing MRS RC MCC

		 NoComs
	 };
	 ComModes ComMode = ComModes::ESPNOW;

	 bool UART0Status = false;
	 bool UART1Status = false;
	 bool UART2Status = false;

	 bool ESPNOWStatus = false;
	 int ESPNOWPacketSentCount = 0;
	 int ESPNOWPacketReceivedCount = 0;
	 String IncomingPacketMACString;
	 bool WiFiStatus = false;

	 bool LocalDisplayStatus = false;
	 bool DebugDisplayStatus = false;

	 bool BME280Status = false;
	 bool IMUStatus = false;

	 String debugTextLines[MAX_TEXT_LINES];

	 CSSMDrivePacket cssmDrivePacket;
	 RC2x15AMCStatusPacket mcStatus;

	 void Init();
	 void AddDebugTextLine(String newLine);
	 void ClearDebugText();
};

extern MCCStatusClass MCCStatus;

#endif

