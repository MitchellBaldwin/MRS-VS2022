/*	NMControls.h
*	NMControlsClass - Base class for managing data from control panel of the MRS RC Navigation Module
*
*
*
*
*	Mitchell Baldwin copyright 2023-2024
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
//NOTE: GPIO04, on the DOIT ESP32 DEVKIT V1, does not work as expected with INPUT_PULLUP
// or INPUT_PULLDOWN modes selected; e.g., when set to INPUT_PULLUP mode connecting GPIO04 
// directly to ground raised current draw ~50 mA, while doing the same on GPIO05 raises
// current draw by just a few uA.
//constexpr byte DefaultRightRockerSwitchPin = 0x04;	// GPIO04
constexpr byte DefaultRightRockerSwitchPin = 0x05;	// GPIO05

#include <seesaw_neopixel.h>
#include <Adafruit_seesaw.h>
constexpr byte SS_BUTTON = 0x18;
constexpr byte SS_NEOPIX = 0x06;

#include <AceButton.h>
class SSButtonConfig : public ace_button::ButtonConfig
{
public:
	SSButtonConfig(Adafruit_seesaw& enc) : encoder(enc)
	{
	}

protected:
	int readButton(uint8_t /* pin */) override
	{
		return encoder.digitalRead(SS_BUTTON);
	}

private:
	Adafruit_seesaw& encoder;
};

constexpr byte DefaultHDGEncoderI2CAddress = 0x36;
constexpr byte DefaultCRSEncoderI2CAddress = 0x37;
constexpr byte DefaultBRTEncoderI2CAddress = 0x38;
constexpr byte DefaultTRREncoderI2CAddress = 0x39;

#include "OSBSet.h"
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

	ezButton* RightRockerSwitch;

	Adafruit_seesaw HDGEncoder;
	//bool HDGButtonState = false;
	seesaw_NeoPixel HDGNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* HDGButtonConfig;
	ace_button::AceButton* HDGButton;
	static void HandleHDGButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState);
	
	Adafruit_seesaw CRSEncoder;
	//bool CRSButtonState = false;
	seesaw_NeoPixel CRSNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* CRSButtonConfig;
	ace_button::AceButton* CRSButton;
	static void HandleCRSButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState);

	Adafruit_seesaw BRTEncoder;
	//bool BRTButtonState = false;
	seesaw_NeoPixel BRTNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* BRTButtonConfig;
	ace_button::AceButton* BRTButton;
	static void HandleBRTButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState);
	
	Adafruit_seesaw TRREncoder;
	//bool TRRButtonState = false;
	seesaw_NeoPixel TRRNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* TRRButtonConfig;
	ace_button::AceButton* TRRButton;
	static void HandleTRRButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState);

public:

	enum Commands
	{
		NoCommand,
		Clear,
		Refresh,
		SYSPage,
		NAVPage,
		COMPage,
		DBGPage,
		I2CScan,

		Prev,
		Next,

		Last
	};

	OSBSet LOSBs;
	OSBSet ROSBs;

	int32_t RightRockerSwitchState = 0;

	int32_t HDGSetting = 0;			// Selected Heading (HDG); bottom right rotary encoder
	static bool HDGSelected;
	int32_t CRSSetting = 0;			// Selected Course (CRS); bottom left rotary encoder
	static bool CRSSelected;
	int32_t BRTSetting = 127;		// Display brightness (BRT) setting; top left rotary encoder
	static bool BRTSelected;
	int32_t TRRSetting = 0;			// Top right rotary encoder; function TBD
	static bool TRRSelected;

	void Init();
	void Init(
		byte rightRockerSwitchPin,
		byte hdgEncoderI2CAddress,
		byte crsEncoderI2CAddress,
		byte brtEncoderI2CAddress,
		byte trrEncoderI2CAddress);
	
	void Update();

	//bool HDGButtonWasPressed();
	//void ToggleHDGSelected();
	//bool CRSButtonWasPressed();
	//void ToggleCRSSelected();
	//bool BRTButtonWasPressed();
	void ToggleBRTSelected();
	//bool TRRButtonWasPressed();
	//void ToggleTRRSelected();

	void SetLocalDisplayBrightness(byte brightness);

	uint32_t ColorWheel(byte WheelPos);
};

extern NMControlsClass NMControls;

#endif

