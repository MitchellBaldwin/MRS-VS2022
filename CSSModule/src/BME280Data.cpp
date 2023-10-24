/*	BME280Data.h
*	BME280DataClass - Base class to hold measurements from a BME280 sensor module
*
*/

#include "BME280Data.h"
#include <BME280.h>

void BME280DataClass::Init()
{
}

String BME280DataClass::GetTchipString()
{
	BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
	char degreeSymbol[1] = { 0xF8 };
	char buf[32];
	snprintf(buf, 22, "%7.2f%s", Tchip, degreeSymbol, String(tempUnit == BME280::TempUnit_Celsius ? "C" : "F"));
	return String(buf);

	//return String(Tchip) + String(degreeSymbol) + String(tempUnit == BME280::TempUnit_Celsius ? "C" : "F");
	//return String(Tchip) + String("°" + String(tempUnit == BME280::TempUnit_Celsius ? "C" : "F"));
}

String BME280DataClass::GetPbaroString()
{
	BME280::PresUnit presUnit(BME280::PresUnit_hPa);
	char buf[32];
	snprintf(buf, 22, "%7.2f %s", Pbaro, String(presUnit == BME280::PresUnit_hPa ? "hPa" : "Pa"));
	return String(buf);
}

String BME280DataClass::GetRHString()
{
	char buf[32];
	snprintf(buf, 22, "%7.2f%%", RH);
	return String(buf);
}

BME280DataClass BME280Data;

