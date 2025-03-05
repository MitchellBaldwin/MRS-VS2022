#include "RC2x15AMCStatusPacket.h"

void RC2x15AMCStatusPacket::ResetOdometer()
{
	OdometerTime = millis();
	OdometerDist = 0.0f;
}
