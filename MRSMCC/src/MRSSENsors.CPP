/*	MRSSENsors.cpp
*	MRSSENsorsClass - Base class for controlling and accessing MRS Sensors module sensors
*
*	Mitchell Baldwin copyright 2026
*
*/

#include "MRSSENsors.h"

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
	Wire.beginTransmission(_i2caddress);
	if (Wire.endTransmission() == 0x00)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int MRSSENsorsClass::GetFWDLIDARRangeMM()
{
	int fwdLIDARRangeMM = 0;
	uint8_t bytesToRead = sizeof(fwdLIDARRangeMM);
	uint8_t data[bytesToRead];

	Wire.beginTransmission(_i2caddress);						// Address the I2C device
	Wire.write(MRSSEN_FWDLIDARRangeAddress);                    // Send register address to read
	Wire.endTransmission();										// Close transmission
	Wire.requestFrom(_i2caddress, sizeof(fwdLIDARRangeMM));		// Request 4 bytes of data
	Wire.readBytes(data, sizeof(fwdLIDARRangeMM));
	memcpy(&fwdLIDARRangeMM, data, sizeof(fwdLIDARRangeMM));
	return fwdLIDARRangeMM;
	
	//return getInt(MRSSEN_FWDLIDARRangeAddress);
}
