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

constexpr byte SS_BUTTON = 0x18;
constexpr byte SS_NEOPIX = 0x06;

class NMControlsClass
{
protected:
	char buf[64];

	byte RightRockerSwitchPin = 0x04;
	byte HDGEncoderI2CAddress = 0x36;
	byte CRSEncoderI2CAddress = 0x37;

	ezButton* RightRockerSwitch;
	Adafruit_seesaw HDGEncoder;
	bool HDGButtonState = false;
	seesaw_NeoPixel HDGNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	Adafruit_seesaw CRSEncoder;
	bool CRSButtonState = false;
	seesaw_NeoPixel CRSNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

public:
	int32_t RightRockerSwitchState = 0;
	int32_t HDGSetting = 0;
	bool HDGSelected = false;
	int32_t CRSSetting = 0;
	bool CRSSelected = false;

	void Init();
	void Init(byte rightRockerSwitchPin, byte hdgEncoderI2CAddress, byte crsEncoderI2CAddress);
	void Update();

	bool HDGButtonWasPressed();
	void ToggleHDGSelected();
	bool CRSButtonWasPressed();
	void ToggleCRSSelected();

	uint32_t ColorWheel(byte WheelPos);
};

extern NMControlsClass NMControls;

#endif

