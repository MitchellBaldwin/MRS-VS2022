/*	CSSMS3Display.h
*	CSSMS3Display - Base class for managing function and presenting data and graphics through the AMOLED
*	display integrated with the module MCU (LilyGO T-Display S3 )
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _CSSMS3Display_h
#define _CSSMS3Display_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CSSMS3Status.h"
#include <TFT_eSPI.h>

constexpr byte DefaultDisplayBrightness = 128;

class CSSMS3Display
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

	const char* ComModeHeadings[CSSMS3StatusClass::ComModes::NoComs] =
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

extern CSSMS3Display cssmS3Display;

#endif

