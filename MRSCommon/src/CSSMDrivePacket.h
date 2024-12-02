/* CSSMDrivePacket.h
* CSSMDrivePacket class - Packet structure used by the Control Stick Steering Module
* to send drive commands and navigation info to the MRS
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

#ifndef _CSSMDrivePacket_h
#define _CSSMDrivePacket_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMDrivePacket
{
public:
	enum DriveModes
	{
		DRV,
		HDG,
		WPT,
		SEQ,

		NoDriveMode
	};

protected:


public:
	DriveModes DriveMode = DriveModes::DRV;		// CSSM Drive Mode
	int HeadingSetting = 0;						// Commanded HDG
	int CourseSetting = 0;						// Commanded CRS
	float OmegaXY = 0.0;						// Commanded turn rate
	float Throttle = 0.0f;						// Commanded throttle setting

};

#endif

