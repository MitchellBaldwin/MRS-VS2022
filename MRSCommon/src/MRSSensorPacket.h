/* MRSSensorPacket.h
* MRSSensorPacket class - MRS sensor telemetry packet
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

#ifndef _MRSSensorPacket_h
#define _MRSSensorPacket_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class MRSSensorPacket
{
protected:
	uint8_t PacketType = 0x32;					// Identifies packet type; fixed for all MRSSensorPackets

public:
};

//extern MRSSensorPacket ;

#endif

