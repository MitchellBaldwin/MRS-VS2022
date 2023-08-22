/*	CSSMStatus.h
*	CSSMStatusClass - Base class for module level status and parameters
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/

#ifndef _CSSMSTATUS_h
#define _CSSMSTATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMStatusClass
{
 protected:


 public:
	 uint8_t MajorVersion = 1;
	 uint8_t MinorVersion = 1;

	 bool UART0Status = false;
	 bool UART1Status = false;
	 bool UART2Status = false;

	 bool LocalDisplayStatus = false;
	 
	 bool BME280Status = false;
	 bool IMUStatus = false;

	 void Init();
};

extern CSSMStatusClass CSSMStatus;

#endif

