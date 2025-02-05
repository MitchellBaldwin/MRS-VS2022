/*	CSSMS3Controls.h
*	CSSMS3Controls - Base class for managing control inputs and sensor data from the 
*	MRS Control Stick Steering Module
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

#ifndef _CSSMS3Controls_h
#define _CSSMS3Controls_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Measurement.h"
#include "BME280Data.h"
//#include <ezButton.h>		// No longer using the ezButton library?
#include "esp_adc_cal.h"
constexpr uint32_t defaultVRef = 1100;

constexpr byte defaultKPSensePin = GPIO_NUM_1;
constexpr byte defaultLThrottlePin = GPIO_NUM_2;
constexpr byte defaultRThrottlePin = GPIO_NUM_3;
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

//#include <TFT_eSPI.h>		// Included in TFTMenu.h
#include <TFTMenu.h>

class CSSMS3Controls
{
protected:
	byte KPSensePin = defaultKPSensePin;
	byte LThrottlePin = defaultLThrottlePin;
	byte RThrottlePin = defaultRThrottlePin;
	byte VMCUPin = defaultVMCUPin;

	MeasurementClass VMCU;						// Analog voltage measured at the MCU battery JST connector
	MeasurementClass KPVoltage;					// Analog voltage from keypad ladder button array
	MeasurementClass LThrottleSetting;			// -100.0 tp 100.0 % left slide throttle setting
	MeasurementClass RThrottleSetting;			// -100.0 tp 100.0 % right slide throttle setting

	float ThrottleDeadZone = 5.0f;				// +/- % dead zone applied around zero (for both throttles)

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

	float GetLThrottleActual();					// Get unmasked left throttle setting
	float GetLThrottle();						// Get left throttle setting adjusted for dead zone(s)
	float GetRThrottleActual();					// Get unmasked right throttle setting
	float GetRThrottle();						// Get right throttle setting adjusted for dead zone(s)

	static void NextDriveMode(byte value);		// Cycle to next drive mode

public:
	enum NavEncoderModes
	{
		MenuMode,
		SteerMode,

		NoMode
	};
	static NavEncoderModes navEncoderMode;

	uint32_t NavSetting = 0;
	static bool NavSelected;
	uint32_t FuncSetting = 0;
	static bool FuncSelected;

	TFTMenuClass* MainMenu;
	MenuItemClass* ESPNMenuItem;
	MenuItemClass* DriveModeMenuItem;
	MenuItemClass* BRTMenuItem;
	MenuItemClass* NextPageMenuItem;

	bool Init(TFT_eSPI* parentTFT);
	bool SetupADC();

	void Update();

	uint16_t GetKPRawADC();
	float GetKPVoltageReal();
	String GetKPVoltageString();
	String GetKPVoltageString(String format);

	uint16_t GetMCURawADC();
	float GetMCUVoltageReal();
	String GetMCUVoltageString();
	String GetMCUVoltageString(String format);

	void CheckButtons();

	static void ToggleNavSelected();
	static void ToggleFuncSelected();
	static void SetESPNOW(byte value);

};

extern CSSMS3Controls cssmS3Controls;

#endif

