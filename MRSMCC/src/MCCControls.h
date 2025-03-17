/*	MCCControls.h
*	MCCControls - Base class for managing control inputs from the MRS Master Control Computer module
*
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MCCControls_h
#define _MCCControls_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Measurement.h"

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

// Addresses for MRS MCC:
constexpr byte defaultNavEncoderI2CAddress = 0x37;	// Bottom rotary encoder, used primarily for navigating controls on the display
constexpr byte defaultFuncEncoderI2CAddress = 0x36;	// Top rotary encoder, used primarily for manipulating control settings

//Addresses for experimental MCC box:
//constexpr byte defaultNavEncoderI2CAddress = 0x38;	// Bottom rotary encoder, used primarily for navigating controls on the display
//constexpr byte defaultFuncEncoderI2CAddress = 0x3A;	// Top rotary encoder, used primarily for manipulating control settings

#include <TFTMenu.h>

class MCCControls
{
protected:
	MeasurementClass VMCU;						// Analog voltage measured at the MCU battery JST connector

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

public:
	uint32_t NavSetting = 0;
	static bool NavSelected;
	uint32_t FuncSetting = 0;
	static bool FuncSelected;

	TFTMenuClass* MainMenu;
	MenuItemClass* ESPNMenuItem;
	MenuItemClass* DriveModeMenuItem;
	MenuItemClass* TBDMenuItem;
	MenuItemClass* BRTMenuItem;
	MenuItemClass* NextPageMenuItem;

	bool Init(TFT_eSPI* parentTFT);
	void Update();

	void CheckButtons();

	static void ToggleNavSelected();
	static void ToggleFuncSelected();

	static void SetESPNOW(int value);
	bool GetESPNowStatus();
	void SetESPNOWStatus(bool newStatus);

};

extern MCCControls mccControls;

#endif

