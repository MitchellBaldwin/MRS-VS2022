// CSSMDrivePacket.h

#ifndef _CSSMDrivePacket_h
#define _CSSMDrivePacket_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMDrivePacket
{
protected:


public:
	int Heading = 0;
	float Throttle = 0.0f;

	void Init();

};

extern CSSMDrivePacket cssmDrivePacket;

#endif

