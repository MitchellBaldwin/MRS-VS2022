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

#include "esp_adc_cal.h"
constexpr uint32_t defaultVRef = 1100;

constexpr byte defaultVMCUPin = GPIO_NUM_4;

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

constexpr byte defaultNavEncoderI2CAddress = 0x37;	// Left rotary encoder, used primarily for navigating controls on the display
constexpr byte defaultFuncEncoderI2CAddress = 0x36;	// Right rotary encoder, used primarily for manipulating control settings

#include <TFTMenu.h>

#include <Adafruit_INA219.h>

constexpr byte defaultINA219Address = 0x41;			// I2C address of INA219 sensor on WaveShare UPS 3S module

class MCCControls
{
protected:
	MeasurementClass VMCU;						// Analog voltage measured at the MCU battery JST connector

	esp_adc_cal_characteristics_t ADC1Chars;
	uint32_t VRef = defaultVRef;

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

	uint32_t ReadCalibratedADC1(int rawADC1);	// Returns calibrated ADC1 measurement in mV

	Adafruit_INA219* WSUPS3SINA219 = new Adafruit_INA219(defaultINA219Address);

public:
	uint32_t NavSetting = 0;
	static bool NavSelected;
	uint32_t FuncSetting = 0;
	static bool FuncSelected;

	TFTMenuClass* MainMenu;
	MenuItemClass* ESPNMenuItem;
	MenuItemClass* DriveModeMenuItem;
	MenuItemClass* BRTMenuItem;
	MenuItemClass* NextPageMenuItem;

	float INA219VShunt = 0.0f;		// mV
	float INA219VBus = 0.0f;		// V
	float INA219VLoad = 0.0f;		// V
	float INA219Current = 0.0f;		// mA
	float INA219Power = 0.0f;		// mW

	bool Init(TFT_eSPI* parentTFT);
	void Update();

	uint16_t GetMCURawADC();
	float GetMCUVoltageReal();
	String GetMCUVoltageString();
	String GetMCUVoltageString(String format);

	void CheckButtons();

	static void ToggleNavSelected();
	static void ToggleFuncSelected();

};

extern MCCControls mccControls;

#endif

