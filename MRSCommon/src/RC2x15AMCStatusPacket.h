/* RC2x15AMCStatusPacket.h
* RC2x15AMCStatusPacket class - Packet structure used to maintain status information
* for the RoboClaw 2x 15A Motor Controller
*
*
*
*
* Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _RC2x15AMCStatusPacket_h
#define _RC2x15AMCStatusPacket_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class RC2x15AMCStatusPacket
{
public:

protected:
	uint8_t PacketType = 0x21;		// Identifies packet type; fixed for all RC2x15AMCStatusPackets


public:

	float SupBatV;		// Supply (battery) voltage in V
	float Temp1;		// Temperature sensor 1 measurement
	float Temp2;		// Temperature sensor 2 measurement (if supported for this motor controller)
	float M1Current;	// Motor 1 current in mA
	float M2Current;	// Motor 2 current in mA
	int M1Encoder;		// Motor 1 encoder reading
	int M2Encoder;		// Motor 2 encoder reading
	int M1Speed;		// Motor 1 speed (qpps - qradrature counts / s)
	int M2Speed;		// Motor 2 speed (qpps - qradrature counts / s)
	int M1SpeedSetting;	// Motor 1 speed setting (qpps - qradrature counts / s)
	int M2SpeedSetting;	// Motor 2 speed setting (qpps - qradrature counts / s)

	bool VBATValid = false;
	bool T1Valid = false;
	bool T2Valid = false;
	bool IMOTValid = false;
	bool ENCPOSValid = false;
	bool SPEEDSValid = false;

};

#endif