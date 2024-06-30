/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*	Pages supported:
*		System		(SYS)	Default summary of navigation systems, power and I/O status
*		Navigation	(NAV)	Primary navigation display
*		Comms		(COM)	Communications with MRS RC MCC
*		Debug		(DBG)	Development and debugging
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

#include "NMStatus.h"
#include <TFT_eSPI.h>

constexpr byte MaxFonts = 4;
constexpr byte LEDPin = 27;
constexpr byte InitialBrightness = 127;

class LocalDisplayClass
{
public:
	enum Pages
	{
		SYS,
		NAV,
		COM,
		DBG,

		NONE
	};

protected:
	char buf[64];
	byte Brightness = InitialBrightness;

	TFT_eSPI tft = TFT_eSPI();

	Pages currentPage = SYS;
	Pages lastPage = NONE;				// Aid to determine when a complete page redraw is needed

	const char* PageTitles[NONE] =
	{
		"    System",
		"Navigation",
		"     Comms",
		"     Debug",
	};

	const char* PageMenus[NONE] =
	{
		"                 Next",	// Rotary menu for the SYS page: select next LoaclDisplay page
		"                 Next",	// Rotary menu for the NAV page: select next LoaclDisplay page
		"                 Next",	// Rotary menu for the COM page: select next LoaclDisplay page
		"                 Next",	// Rotary menu for the DBG page: select next LoaclDisplay page
	};

	const char* ComModeHeadings[NMStatusClass::ComModes::NoComs] =
	{
		"MRS RC MCC UART2",
		"Direct WiFi/TCP ",
		"Direct ESP-NOW  ",
	};

	void DrawPageHeaderAndFooter();
	void DrawSYSPage();
	void DrawNAVPage();
	void DrawCOMPage();
	void DrawDBGPage();

	void DrawNONEPage();

public:
	enum Commands
	{
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

	bool Init();

	void SetBrightness(byte brightness);

	bool TestFonts();

	void Update();
	void Control(uint8_t command);
	Pages GetCurrentPage();

};

extern LocalDisplayClass LocalDisplay;

#endif

