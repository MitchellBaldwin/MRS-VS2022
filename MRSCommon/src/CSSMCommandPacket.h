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
protected:
	uint8_t PacketType = 0x24;					// Identifies packet type; fixed for all CSSMCommandPackets

public:

};

//extern CSSMCommandPacket ;

#endif

