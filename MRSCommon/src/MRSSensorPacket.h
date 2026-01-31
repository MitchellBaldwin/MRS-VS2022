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
	uint8_t PacketType = 0x32;		// Identifies packet type; fixed for all MRSSensorPackets

public:
	float INA219VShunt = 0.0f;		// mV
	float INA219VBus = 0.0f;		// V
	float INA219VLoad = 0.0f;		// V
	float INA219Current = 0.0f;		// mA
	float INA219Power = 0.0f;		// mW

	float BME680Temp = 0.0f;		// °C
	float BME680Pbaro = 0.0f;		// hPa
	float BME680RH = 0.0f;			// %
	float BME680Gas = 0.0f;			// ohm
	float BME680Alt = 0.0f;			// m

	int FWDVL53L1XRange = 0;		// mm

	float ODOSPosX = 0.0f;			// m
	float ODOSPosY = 0.0f;			// m
	float ODOSHdg = 0.0f;			// °

	int TurretPosition = 0;			// °

};

#endif

