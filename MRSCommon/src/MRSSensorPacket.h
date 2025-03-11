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
	float BME280Temp = 0.0f;
	float BME280Pbaro = 0.0f;
	float BME280RH = 0.0f;
	float BME280Gas = 0.0f;
	float BME280Alt = 0.0f;

};

//extern MRSSensorPacket ;

#endif

