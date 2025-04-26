/*	MRSSENStatus.h
*	MRSSENStatus - Class to maintain status information on the MRS Sensors module
*
*
*/

#include "MRSSENStatus.h"

bool MRSSENStatus::Init()
{

	return true;
}

String MRSSENStatus::GetDateTimeString()
{
	char buf[32];

	// Build a string representation of the date and time:
	snprintf(buf, 31, "%02d:%02d:%02d %02d-%02d-%02d", RTCtime.hour, RTCtime.minute,
		RTCtime.second, RTCtime.year, RTCtime.month, RTCtime.day);

	return String(buf);
}


MRSSENStatus mrsSENStatus;

