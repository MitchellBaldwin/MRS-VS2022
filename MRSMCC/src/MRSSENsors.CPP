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

bool MRSSENsorsClass::Update()
{
	/*!
	  @brief     Read entire MRSSensorPacket structure from I2C
	  @details   Wrapper for getData to read the full MRSSensorPacket structure
	  @param[in] packet Reference to MRSSensorPacket structure to fill
	  @return    True if read successful, false otherwise
	*/
	cssmCommandPacket.command = CSSMCommandPacket::NoCommand;
	Wire.beginTransmission(_i2caddress);
	Wire.write((uint8_t*)&cssmCommandPacket, sizeof(CSSMCommandPacket));
	Wire.endTransmission();
	uint8_t bytesRead = getData<MRSSensorPacket>(0x00, mrsSensorPacket);
	return (bytesRead == sizeof(MRSSensorPacket));
}

bool MRSSENsorsClass::getMRSSensorPacket(MRSSensorPacket& packet) {
	packet = mrsSensorPacket;
	return true;
}

int MRSSENsorsClass::GetFWDLIDARRangeMM()
{
	return mrsSensorPacket.FWDVL53L1XRange;
}
