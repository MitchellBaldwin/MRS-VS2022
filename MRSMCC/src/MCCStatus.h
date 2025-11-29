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
#include "C:\Repos\MRS-VS2022\MRSCommon\src\CSSMCommandPacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\RC2x15AMCStatusPacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\MRSStatusPacket.h"
#include "C:\Repos\MRS-VS2022\MRSCommon\src\MRSSensorPacket.h"

constexpr uint8_t MAX_TEXT_LINES = 14;

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
	 bool RC2x15AUARTStatus = false;
	 bool UART2Status = false;

	 bool WSUPS3SINA219Status = false;

	 bool RC2x15AMCStatus = false;
	 String RC2x15AMCVersionString;
	 float M1kp = 0.0f;
	 float M1ki = 0.0f;
	 float M1kd = 0.0f;
	 uint32_t M1qpps = 0;
	 float M2kp = 0.0f;
	 float M2ki = 0.0f;
	 float M2kd = 0.0f;
	 uint32_t M2qpps = 0;

	 bool ESPNOWStatus = false;
	 uint32_t CSSMPacketSentCount = 0;
	 uint16_t SendRetries = 0;

	 uint32_t CSSMPacketReceivedCount = 0;
	 uint32_t SaveCSSMPacketReceivedCount = 0;
	 uint64_t LastCSSMPacketReceivedTime = 0;		// ms
	 uint64_t CSSMPacketReceiptInterval = 0;		// ms
	 String IncomingCSSMPacketMACString;
	 bool CSSMESPNOWLinkStatus = false;

	 bool WiFiStatus = false;

	 bool LocalDisplayStatus = false;
	 bool DebugDisplayStatus = false;

	 bool NavEncoderStatus = false;
	 bool FuncEncoderStatus = false;

	 bool BME680Status = false;
	 bool IMUStatus = false;

	 String debugTextLines[MAX_TEXT_LINES];

	 CSSMDrivePacket cssmDrivePacket;
	 CSSMDrivePacket lastCSSMDrivePacket;
	 RC2x15AMCStatusPacket mcStatus;
	 MRSStatusPacket mrsStatusPacket;
	 MRSSensorPacket mrsSensorPacket;

	 void Init();
	 void Update();
	 void AddDebugTextLine(String newLine);
	 void ClearDebugText();
};

extern MCCStatusClass MCCStatus;

#endif

