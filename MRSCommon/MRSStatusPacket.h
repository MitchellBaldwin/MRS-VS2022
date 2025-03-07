/* MRSStatusPacket.h
* MRSStatusPacket class - MRS status telemetry packet
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

#ifndef _MRSStatusPacket_h
#define _MRSStatusPacket_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class MRSStatusPacket
{
protected:
	uint8_t PacketType = 0x31;					// Identifies packet type; fixed for all MRSStatusPackets

public:

};

//extern MRSStatusPacket ;

#endif

