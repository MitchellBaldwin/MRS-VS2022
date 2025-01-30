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
		DRV,		// CSS speed (±mm/s) and turn rate (±rad/s)
		HDG,		// Heading hold
		WPT,		// Navigate and move to waypoint
		SEQ,		// Navigate and move along a sequence of waypoints
		DRVTw,		// CSS throttle (±100%) and turn rate (±100%)
		DRVLR,		// CSS left & right throttle (±100%)

		NoDriveMode
	};

protected:
	uint8_t PacketType = 0x20;		// Identifies packet type; fixed for all CSSMDrivePackets

public:
	DriveModes DriveMode = DriveModes::DRV;		// CSSM Drive Mode
	int HeadingSetting = 0;						// Commanded HDG (0 - 359)
	int CourseSetting = 0;						// Commanded CRS (0 - 359)
	float OmegaXY = 0.0f;						// Commanded turn rate (±rad/s)
	float Speed = 0.0f;							// Commanded throttle setting (±mm/s)
	float LThrottle = 0.0f;						// Commanded left throttle setting (±100.0%)
	float RThrottle = 0.0f;						// Commanded right throttle setting (±100.0%)

};

#endif

