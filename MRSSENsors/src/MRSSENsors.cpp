/*	MRSSENsors.cpp
*	MRSSENsorsClass - Base class for controlling and accessing MRS Sensors module sensors
*
*	Mitchell Baldwin copyright 2026
*
*/

#include "MRSSENsors.h"
#include <Wire.h>

MRSSENsorsClass::MRSSENsorsClass()
{
}

bool MRSSENsorsClass::Init()
{
	return Init(defaultMRSSENAddress);
}

bool MRSSENsorsClass::Init(uint8_t i2cAddress)
{
	_i2caddress = i2cAddress;
	Wire.BeginTransmission(_i2caddress);
	if (Wire.EndTransmission() == 0x00)
	{
		return true;
	}
	else
	{
		return false;
	}
}