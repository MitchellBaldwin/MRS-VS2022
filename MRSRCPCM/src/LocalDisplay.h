/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Power Control Module
*
*	Pages supported:
*		System (SYS)	Default summary of system, power supply voltages and I/O ststus
*		Comms (COM)		Communications with MRS RC MCC
*
*	Mitchell Baldwin copyright 2023-2024
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

#include "PCMStatus.h"
#include <TFT_eSPI.h>
#include "MenuItem.h"

constexpr byte DefaultDisplayBrightness = 128;

class LocalDisplayClass
{
public:
	enum Pages
	{
		SYS,
		COM,

		NONE
	};

protected:
	char buf[32];

	TFT_eSPI tft = TFT_eSPI();
	static byte Brightness;

	Pages currentPage = SYS;
	Pages lastPage = NONE;				// Aid to determine when a complete page redraw is needed

	const char* PageTitles[NONE] =
	{
		"  System",
		"   Comms",
	};

	const char* PageMenus[NONE] =
	{
		"DbNx  DRVm       LoNx",	// Rotary menu for the SYS page: select next DebugDisplay page, enter DRIVE mode, select next LoaclDisplay page
		"SYSp  Mode       LoNx",	// OSB menu for the COM page: select SYS page, select current drive DriveMode page, 
	};

	const char* ComModeHeadings[PCMStatusClass::ComModes::NoComs] =
	{
		"MRS RC MCC UART1",
		"Direct WiFi/TCP ",
		"Direct ESP-NOW  ",
	};

	void DrawPageHeaderAndFooter();
	void DrawSYSPage();
	void DrawCOMPage();

	void DrawNONEPage();

public:
	enum Commands
	{
		Clear,
		Refresh,
		SYSPage,
		COMPage,
		I2CScan,

		Prev,
		Next,

		Last
	};

	bool Init();

	bool Test();
	void Update();
	void Control(uint8_t command);
	Pages GetCurrentPage();
	TFT_eSPI* GetTFT();
	byte GetDisplayBrightness();
	static void SetDisplayBrightness(byte brightness);

};

extern LocalDisplayClass LocalDisplay;

#endif

