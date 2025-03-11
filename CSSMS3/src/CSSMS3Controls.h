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
#include "esp_adc_cal.h"

#include <esp_now.h>

constexpr uint32_t defaultVRef = 1100;

constexpr byte defaultTS2Pin = GPIO_NUM_21;

constexpr byte defaultKPSensePin = GPIO_NUM_1;
constexpr byte defaultLThrottlePin = GPIO_NUM_2;
constexpr byte defaultRThrottlePin = GPIO_NUM_3;
constexpr byte defaultVMCUPin = GPIO_NUM_4;

#include <seesaw_neopixel.h>
#include <Adafruit_seesaw.h>
constexpr byte SS_BUTTON = 0x18;
constexpr byte SS_NEOPIX = 0x06;

constexpr byte defaultNavEncoderI2CAddress = 0x37;	// Left rotary encoder, used primarily for navigating controls on the display
constexpr byte defaultFuncEncoderI2CAddress = 0x36;	// Right rotary encoder, used primarily for manipulating control settings

#include <AceButton.h>
using namespace ace_button;

class SSButtonConfig : public ButtonConfig
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

#include "OSBArray.h"
constexpr byte OSBCount = 4;
constexpr byte OSBLevelCount = OSBCount + 1;


#include <TFTMenu.h>

class CSSMS3Controls
{
public:
	typedef void (*OSBHandler)(int);	// Function pointer for OnPress event handler

protected:
	AceButton* TS2;
	static void HandleDefaultButtonEvents(AceButton* button, uint8_t eventType, uint8_t buttonState);

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
	AceButton* NavButton;
	static void HandleNavButtonEvents(AceButton* b, uint8_t eventType, uint8_t buttonState);

	Adafruit_seesaw FuncEncoder;
	seesaw_NeoPixel FuncNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
	SSButtonConfig* FuncButtonConfig;
	AceButton* FuncButton;
	static void HandleFuncButtonEvents(AceButton* b, uint8_t eventType, uint8_t buttonState);

	static void DriveOSBHandler(int value);
	static void HeadingOSBHandler(int value);
	static void WaypointOSBHandler(int value);
	static void StopOSBHandler(int value);
	
	OSBArrayClass* OSBs;
	uint16_t OSBLevels[OSBLevelCount] = { 0, 763, 1572, 2389, 3248 };
	OSBHandler OSBHandlers[OSBCount] = { DriveOSBHandler, HeadingOSBHandler, WaypointOSBHandler, StopOSBHandler };

	TFT_eSPI* tft;

	uint32_t ReadCalibratedADC1(int rawADC1);	// Returns calibrated ADC1 measurement in mV

	float GetLThrottleActual();					// Get unmasked left throttle setting
	float GetLThrottle();						// Get left throttle setting adjusted for dead zone(s)
	float GetRThrottleActual();					// Get unmasked right throttle setting
	float GetRThrottle();						// Get right throttle setting adjusted for dead zone(s)

	static void NextDriveMode(int value);		// Cycle to next drive mode
	static void SetHDG(int value);				// Set the HDG from the value field of the HDGSetMenuItem object
	static void CaptureHDG(int value);			// Capture current heading and enter HDG drive mode
	static void SetSPD(int value);				// Set the Speed from the value field of the SPDSetMenuItem object
	static void T1Reset(int value);				// Send command to MRS resetting Trip 1 odometer measurements
	static void T2Reset(int value);				// Send command to MRS resetting Trip 1 odometer measurements

public:
	enum NavEncoderModes
	{
		MenuNavEncMode,
		HDGSetMode,
		CRSSetMode,

		NoNavEncMode
	};
	static NavEncoderModes navEncoderMode;

	enum FuncEncoderModes
	{
		MenuFuncEncMode,
		SteerMode,

		NoFuncEncMode
	};
	static FuncEncoderModes funcEncoderMode;

	uint32_t NavSetting = 0;
	static bool NavSelected;
	int FuncSetting = 0;
	static bool FuncSelected;

	TFTMenuClass* MainMenu;				// SYS page menu
	MenuItemClass* ESPNMenuItem;
	MenuItemClass* DriveModeMenuItem;
	MenuItemClass* BRTMenuItem;
	MenuItemClass* NextPageMenuItem;

	TFTMenuClass* CommsMenu;			// COM page menu
	MenuItemClass* WiFiMenuItem;
	
	TFTMenuClass* DebugMenu;			// DBG page menu
	MenuItemClass* ReportMemoryMenuItem;
	MenuItemClass* MCCalibMenuItem;
	MenuItemClass* ShowFontMenuItem;

	TFTMenuClass* HDGPageMenu;			// HDG page menu
	MenuItemClass* HDGHoldMenuItem;
	MenuItemClass* HDGSetMenuItem;
	MenuItemClass* SPDSetMenuItem;

	TFTMenuClass* DRVPageMenu;			// DRV page menu
	MenuItemClass* CaptureHDGMenuItem;
	MenuItemClass* T1ResetMenuItem;
	MenuItemClass* T2ResetMenuItem;

	TFTMenuClass* WPTPageMenu;			// WPT page menu
	MenuItemClass* DirectToMenuItem;

	TFTMenuClass* SEQPageMenu;			// SEQ page menu
	MenuItemClass* ActivateMenuItem;
	MenuItemClass* PreviousWPTMenuItem;
	MenuItemClass* NextWPTMenuItem;
	MenuItemClass* CRSSetMenuItem;

	
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
	bool GetTS2State();

	static void ToggleNavSelected();
	static void ToggleFuncSelected();

	static void SetESPNOW(int value);
	bool GetESPNowStatus();
	void SetESPNOWStatus(bool newStatus);

	static void SetWiFi(int value);
	static void StartMCCalib(int value);

};

extern CSSMS3Controls cssmS3Controls;

#endif

