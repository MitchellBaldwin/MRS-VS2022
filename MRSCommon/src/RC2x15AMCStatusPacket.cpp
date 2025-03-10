#include "RC2x15AMCStatusPacket.h"

void RC2x15AMCStatusPacket::ResetOdometer()
{
	OdometerTime = millis();
	Trip1Time = OdometerTime;
	Trip2Time = OdometerTime;
	OdometerDist = 0.0f;
	Trip1Dist = 0.0f;
	Trip2Dist = 0.0f;
}

void RC2x15AMCStatusPacket::ResetTrip1()
{
	Trip1Time = millis();;
	Trip1Dist = 0.0f;
}

void RC2x15AMCStatusPacket::ResetTrip2()
{
	Trip2Time = millis();;
	Trip2Dist = 0.0f;
}
