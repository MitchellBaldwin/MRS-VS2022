/*	PCMStatus.h
*	PCMStatusClass - Base class for module level status and parameters
*
*	Mitchell Baldwin copyright 2024
*
*/
#ifndef _PCMSTATUS_h
#define _PCMSTATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class PCMStatusClass
{
 protected:


 public:
	 // Power Control Module firmware version:
	 uint8_t MajorVersion = 1;
	 uint8_t MinorVersion = 1;

	 enum ComModes
	 {
		 IDCPktSerial,	// COBS encoded packet exchange with MRS RC MCC through UART2 (Default mode)
		 WiFiTCP,		// For the CSSM, direct WiFi connection to MRS MCC, bypassing MRS RC MCC 
		 ESPNOW,		// For the CSSM, direct ESP-NOW connection with MRS MCC, bypassing MRS RC MCC

		 NoComs
	 };
	 ComModes ComMode = ComModes::IDCPktSerial;

	 bool UART0Status = false;
	 bool UART1Status = false;
	 bool UART2Status = false;

	 bool WiFiStatus = false;

	 bool LocalDisplayStatus = false;
	 bool DebugDisplayStatus = false;

	 bool BME280Status = false;

	 void Init();

};

extern PCMStatusClass PCMStatus;

#endif

