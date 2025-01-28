/*	CSSMS3Display.h
*	CSSMS3Display - Base class for managing function and presenting data and graphics through the AMOLED
*	display integrated with the module MCU (LilyGO T-Display S3 )
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _CSSMS3Display_h
#define _CSSMS3Display_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CSSMS3Display
{
 protected:


 public:
	bool Init();
	void Update();

};

extern CSSMS3Display cssmS3Display;

#endif

