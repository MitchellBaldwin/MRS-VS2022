/*	CSSMS3EnvSensors.h
*	CSSMS3EnvSensors - Base class for managing measurements and sensor data from the
*	MRS Control Stick Steering Module S3
*
*/

#include "CSSMS3EnvSensors.h"
#include "CSSMS3Status.h"

bool CSSMS3EnvSensors::Init()
{
	CSSMS3Status.BME280Status = BME280Data.Init();

	return CSSMS3Status.BME280Status;
}

void CSSMS3EnvSensors::Update()
{
	BME280Data.ReadENVData();

}

CSSMS3EnvSensors EnvSensors;

