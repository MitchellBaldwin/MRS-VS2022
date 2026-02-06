/*	MRSChassisSensors.h
*	MRSChassisSensorsClass - Class to maintain status information on the MRS Sensors module
*
*
*
*	Mitchell Baldwin copyright 2026
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MRSCHASSISSENSORS_h
#define _MRSCHASSISSENSORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "INA219.h"
constexpr byte defaultINA219Address = 0x41;			// I2C address of INA219 sensor on WaveShare UPS 3S module

class MRSChassisSensorsClass
{
protected:
	INA219* WSUPS3SINA219 = new INA219(defaultINA219Address);


public:
	bool Init();
	bool Update();

};

extern MRSChassisSensorsClass MRSChassisSensors;

#endif

