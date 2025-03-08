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
#include "BarGauge.h"

constexpr byte DefaultDisplayBrightness = 128;

class CSSMS3Display
{
public:
	enum Pages
	{
		SYS,
		COM,
		DBG,
		SEN,
		MRS,

		DRV,
		HDG,
		WPT,
		SEQ,

		NONE
	};

protected:
	char buf[64];

	TFT_eSPI tft = TFT_eSPI();

	BarGauge LTrackBarGauge;
	BarGauge MRSTrackBarGauge;
	BarGauge RTrackBarGauge;

	static byte Brightness;

	static Pages currentPage;
	Pages lastPage = Pages::NONE;				// Aid to determine when a complete page redraw is needed
	Pages lastSystemPage = Pages::SYS;

	const char* PageTitles[NONE] =
	{
		"System",
		"Comms",
		"Debug",
		"Sensors",
		"MRS",

		"Drive",
		"Heading",
		"Waypoint",
		"Sequence",

	};

	const char* ComModeHeadings[CSSMS3StatusClass::ComModes::NoComs] =
	{
		"MRS RC MCC UART1",
		"Direct WiFi/TCP",
		"Direct ESP-NOW",
	};

	const char* DriveModeHeadings[CSSMDrivePacket::DriveModes::NoDriveMode] =
	{
		"DRIVE  ",
		"HDG    ",
		"WPT    ",
		"SEQ    ",
		"DRV T/w",
		"DRV L/R",
		"E-STOP ",
		"STOP   ",
		"CALIB  ",
		"TEST   ",

	};

	bool ShowingFontTable = false;

	void GetTimeString(uint64_t msTime, String* timeString);

	void DrawPageHeaderAndFooter();
	void DrawDashboard(int32_t xTL, int32_t yTL);
	void DrawSYSPage();
	void DrawCOMPage();
	void DrawDBGPage();
	void DrawSENPage();
	void DrawMRSPage();
	void DrawDRVPage();
	void DrawHDGPage();
	void DrawWPTPage();
	void DrawSEQPage();

	void DrawNONEPage();


public:
	enum Commands
	{
		Clear,
		Refresh,
		SYSPage,
		COMPage,
		DBGPage,
		SENPage,
		MRSPage,
		DRVPage,
		HDGPage,
		WPTPage,
		SEQPage,

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
	Pages GetLastSystemsPage();
	void ShowCurrentDriveModePage();
	void RefreshCurrentPage();
	void RefreshPage(Pages page);
	static void PrevPage(int value);
	static void NextPage(int value);

	TFT_eSPI* GetTFT();
	byte GetDisplayBrightness();
	static void SetDisplayBrightness(int brightness);

	void AddDebugTextLine(String newLine);
	static void ReportHeapStatus(int value);
	static void ShowFontTableFixed(int value);
	void ShowFontTable(int32_t xTL, int32_t yTL);

};

extern CSSMS3Display cssmS3Display;

#endif

