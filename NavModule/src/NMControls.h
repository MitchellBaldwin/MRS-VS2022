/*	NMControls.h
*	NMControlsClass - Base class for managing data from control panel of the MRS RC Navigation Module
*
*
*
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.00:	Initial data structure
*	v
*
*/
#ifndef _NMCONTROLS_h
#define _NMCONTROLS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ezButton.h>
#include <seesaw_neopixel.h>
#include <Adafruit_seesaw.h>

class NMControlsClass
{
protected:
	byte RightRockerSwitchPin = 0x04;

	ezButton* RightRockerSwitch;

public:
	int RightRockerSwitchState = 0;

	void Init();
	void Init(byte rightRockerSwitchPin);
	void Update();

};

extern NMControlsClass NMControls;

#endif

