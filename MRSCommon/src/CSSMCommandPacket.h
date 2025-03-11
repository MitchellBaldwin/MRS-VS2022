/* CSSMCommandPacket.h
* CSSMCommandPacket class - CSSM to MRS command packet
*
*
*
*
* Mitchell Baldwin copyright 2025
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _CSSMCommandPacket_h
#define _CSSMCommandPacket_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMCommandPacket
{
public:
	enum CSSMCommandCodes
	{
		NoCommand = 0x00,

		ResetMCOdometer = 0x10,
		ResetMCTrip1 = 0x11,
		ResetMCTrip2 = 0x12,
		
		StartMSOdometerCalibration = 0x18,

	};
protected:
	uint8_t PacketType = 0x24;								// Identifies packet type; fixed for all CSSMCommandPackets

public:
	CSSMCommandCodes command = CSSMCommandCodes::NoCommand;

};

//extern CSSMCommandPacket ;

#endif

