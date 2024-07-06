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
constexpr byte DefaultRightRockerSwitchPin = 0x04;	// GPIO04

#include <seesaw_neopixel.h>
#include <Adafruit_seesaw.h>
constexpr byte SS_BUTTON = 0x18;
constexpr byte SS_NEOPIX = 0x06;

constexpr byte DefaultHDGEncoderI2CAddress = 0x36;
constexpr byte DefaultCRSEncoderI2CAddress = 0x37;
constexpr byte DefaultBRTEncoderI2CAddress = 0x38;
constexpr byte DefaultTRREncoderI2CAddress = 0x39;

#include "OSBArray.h"
constexpr byte DefaultLOSBSensePin = 0x22;			// GPIO34	(ADC1 CH6)
constexpr byte DefaultROSBSensePin = 0x23;			// GPIO35	(ADC1 CH7)

constexpr uint8_t LOSB_NUM_BUTTONS = 4;
constexpr uint8_t LOSB_NUM_LEVELS = LOSB_NUM_BUTTONS + 1;
constexpr uint16_t LOSB_LEVELS[LOSB_NUM_LEVELS] =
{
  0,
  1090,
  1680,
  2330,
  3100
};

constexpr uint8_t ROSB_NUM_BUTTONS = 4;
constexpr uint8_t ROSB_NUM_LEVELS = ROSB_NUM_BUTTONS + 1;
constexpr uint16_t ROSB_LEVELS[ROSB_NUM_LEVELS] =
{
  0,
  1250,
  1920,
  2650,
  3440
};


class NMControlsClass
{
protected:
	char buf[64];

	OSBArrayClass LOSBArray;
	OSBArrayClass ROSBArray;
	byte LOSBSensePin = DefaultLOSBSensePin;
	byte ROSBSensePin = DefaultROSBSensePin;
	OSBArrayClass::OSBs LastLOSBPressed = OSBArrayClass::OSBs::NoOsb;
	OSBArrayClass::OSBs LastROSBPressed = OSBArrayClass::OSBs::NoOsb;

	ezButton* RightRockerSwitch;

	Adafruit_seesaw HDGEncoder;
	bool HDGButtonState = false;
	seesaw_NeoPixel HDGNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	Adafruit_seesaw CRSEncoder;
	bool CRSButtonState = false;
	seesaw_NeoPixel CRSNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	Adafruit_seesaw BRTEncoder;
	bool BRTButtonState = false;
	seesaw_NeoPixel BRTNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	Adafruit_seesaw TRREncoder;
	bool TRRButtonState = false;
	seesaw_NeoPixel TRRNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

public:
	int LeftOSBADCReading = 0;
	int RightOSBADCReading = 0;
	bool NewLOSBPress = false;
	bool NewROSBPress = false;

	int32_t RightRockerSwitchState = 0;

	int32_t HDGSetting = 0;			// Selected Heading (HDG); bottom right rotary encoder
	bool HDGSelected = false;
	int32_t CRSSetting = 0;			// Selected Course (CRS); bottom left rotary encoder
	bool CRSSelected = false;
	int32_t BRTSetting = 127;		// Display brightness (BRT) setting; top left rotary encoder
	bool BRTSelected = false;
	int32_t TRRSetting = 0;			// Top right rotary encoder; function TBD
	bool TRRSelected = false;

	void Init();
	void Init(
		byte rightRockerSwitchPin,
		byte hdgEncoderI2CAddress,
		byte crsEncoderI2CAddress,
		byte brtEncoderI2CAddress,
		byte trrEncoderI2CAddress,
		byte losbSensePin,
		byte rosbSensePin);
	
	void Update();

	OSBArrayClass::OSBs NewLOSBKeyWasPressed();
	OSBArrayClass::OSBs NewROSBKeyWasPressed();
	
	bool HDGButtonWasPressed();
	void ToggleHDGSelected();
	bool CRSButtonWasPressed();
	void ToggleCRSSelected();
	bool BRTButtonWasPressed();
	void ToggleBRTSelected();
	bool TRRButtonWasPressed();
	void ToggleTRRSelected();

	void SetLocalDisplayBrightness(byte brightness);

	uint32_t ColorWheel(byte WheelPos);
};

extern NMControlsClass NMControls;

#endif

