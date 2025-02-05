/*	PCMControls.h
*	PCMControlsClass - Base class for managing data from control panel of the MRS RC Power Control Module
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
#ifndef _PCMCONTROLS_h
#define _PCMCONTROLS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
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

constexpr byte DefaultNavEncoderI2CAddress = 0x36;	// Left rotary encoder, used primarily for navigating controls on the display
constexpr byte DefaultFuncEncoderI2CAddress = 0x37;	// Right rotary encoder, used primarily for manipulating control settings
//constexpr byte DefaultNavEncoderI2CAddress = 0x38;	// Left rotary encoder, used primarily for navigating controls on the display
//constexpr byte DefaultFuncEncoderI2CAddress = 0x3A;	// Right rotary encoder, used primarily for manipulating control settings

constexpr byte DefaultCSSMPowerEnablePin = GPIO_NUM_11;
constexpr byte DefaultNMPowerEnablePin = GPIO_NUM_12;

#include <TFTMenu.h>

class PCMControlsClass
{
protected:
	char buf[64];

	static byte CSSMPowerEnablePin;
	static byte NMPowerEnablePin;

	Adafruit_seesaw NavEncoder;
	seesaw_NeoPixel NavNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* NavButtonConfig;
	ace_button::AceButton* NavButton;
	static void HandleNavButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState);

	Adafruit_seesaw FuncEncoder;
	seesaw_NeoPixel FuncNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* FuncButtonConfig;
	ace_button::AceButton* FuncButton;
	static void HandleFuncButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState);

	TFT_eSPI* tft;

	static void ControlCSSMPower(byte value);
	static void ControlNMPower(byte value);
	static void ToggleWSUPS3SINA219(byte value);

public:
	uint32_t NavSetting = 0;
	static bool NavSelected;
	uint32_t FuncSetting = 0;
	static bool FuncSelected;

	TFTMenuClass* MainMenu;
	MenuItemClass* UPSMenuItem;
	//MenuItemClass* CSSMMenuItem;
	MenuItemClass* NMMenuItem;
	MenuItemClass* BRTMenuItem;
	MenuItemClass* NextPageMenuItem;
	
	void Init(TFT_eSPI* parentTFT);
	void Init(byte navEncoderI2CAddress, byte funcEncoderI2CAddress);
	void Init(byte navEncoderI2CAddress, byte funcEncoderI2CAddress, byte cssmPowerEnablePin, byte nmPowerEnablePin);

	void CheckButtons();	// This function is not being used?
	void Update();

	static void ToggleNavSelected();
	static void ToggleFuncSelected();

};

extern PCMControlsClass PCMControls;

#endif

