/*	CSSMS3Controls.cpp
*	CSSMS3Controls - Base class for managing data from sensors on the MRS Control Stick Steering Module
*
*/

#include "CSSMS3Controls.h"

uint32_t CSSMS3Controls::ReadCalibratedADC1(int rawADC1)
{
	return esp_adc_cal_raw_to_voltage(rawADC1, &ADC1Chars);
}

bool CSSMS3Controls::Init()
{


}

void CSSMS3Controls::Update()
{
}

CSSMS3Controls cssmS3Controls;

