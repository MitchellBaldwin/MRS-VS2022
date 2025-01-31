/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS Master Communications Controller (MRS MCC)
*
*	Pages supported:
*		System (SYS)	Default summary of system, power supply voltages and I/O ststus
*		Comms (COM)		Communications with MRS RC MCC or MRS RC CSSM
*
*	Mitchell Baldwin copyright 2024-2025
*
*	v 0.00:	Initial command set
*	v
*
*/

#ifndef _LOCALDISPLAY_h
#define _LOCALDISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MCCStatus.h"
#include <TFT_eSPI.h>

constexpr byte DefaultDisplayBrightness = 128;

class LocalDisplayClass
{
public:
	enum Pages
	{
		SYS,
		COM,
		DBG,

		NONE
	};

protected:
	char buf[64];

	TFT_eSPI tft = TFT_eSPI();
	static byte Brightness;

	static Pages currentPage;
	Pages lastPage = NONE;				// Aid to determine when a complete page redraw is needed

	const char* PageTitles[NONE] =
	{
		"  System",
		"   Comms",
		"   Debug",

	};

	const char* ComModeHeadings[MCCStatusClass::ComModes::NoComs] =
	{
		"MRS RC MCC UART1",
		"Direct WiFi/TCP ",
		"Direct ESP-NOW  ",
	};

	const char* DriveModeHeadings[CSSMDrivePacket::DriveModes::NoDriveMode] =
	{
		"DRIVE  ",
		"HDG    ",
		"WPT    ",
		"SEQ    ",
		"DRV T/w",
		"DRV L/R",
	};

	void DrawPageHeaderAndFooter();
	void DrawSYSPage();
	void DrawCOMPage();
	void DrawDBGPage();

	void DrawNONEPage();

public:
	enum Commands
	{
		Clear,
		Refresh,
		SYSPage,
		COMPage,
		DBGPage,
		I2CScan,

		Prev,
		Next,

		Last
	};

	bool Init();

	bool Test();
	void Update();
	void Control(uint8_t command);

	void SetCurrentPage(Pages page);
	Pages GetCurrentPage();
	void RefreshCurrentPage();
	void RefreshPage(Pages page);
	static void PrevPage(byte value);
	static void NextPage(byte value);

	TFT_eSPI* GetTFT();
	byte GetDisplayBrightness();
	static void SetDisplayBrightness(byte brightness);

	void AddDebugTextLine(String newLine);
	void ReportHeapStatus();

};

extern LocalDisplayClass LocalDisplay;

#endif

