/*	LocalDisplayClass - Based on template class for implementing paged graphical display interface for
*	Arduino and ESP32 based embedded control systems.
*
*	Extended to serve as local display for the ESP32 based Control Stick Steering (CSS) module
*	Pages supported:
*		System (SYS)	Default summary od system, POW, COM and I/O ststus
*		Power (POW)		Power supply, battery status
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

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "CSSMStatus.h"

constexpr int SCREEN_WIDTH = 128;	// OLED display width, in pixels
constexpr int SCREEN_HEIGHT = 64;	// OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1	// Reset pin # (or -1 if sharing Arduino reset pin)

class LocalDisplayClass
{
public:
	enum Pages
	{
		SYS,
		POW,
		COM,
		I2C,
		ENV,
		IMU,

		DRV,
		HDG,
		WPT,
		SEQ,

		NONE
	};

protected:
	 Adafruit_SSD1306 display;
	 uint8_t I2CAddress = 0x00;
	 char buf[32];

	 Pages currentPage = SYS;
	 Pages lastPage = NONE;		// Aid to determine when a complete page redraw is needed

	 const char* PageTitles[NONE] =
	 {
		 "  System",
		 "   Power",
		 "   Comms",
		 "     I2C",
		 "     ENV",
		 "     IMU",

		 "        ",
		 "        ",
		 "        ",
		 "        ",
	 };

	 //const char* PageTitles[NONE] =
	 //{
		// "MRS CSS DRIVE  System",
		// "MRS CSS DRIVE   Power",
		// "MRS CSS DRIVE   Comms",
		// "MRS CSS DRIVE     I2C",
		// "MRS CSS DRIVE     ENV",
		// "MRS CSS DRIVE     IMU",

		// "MRS CSS DRIVE        ",
		// "MRS CSS  HDG         ",
		// "MRS CSS  WPT         ",
		// "MRS CSS  SEQ         ",
	 //};

	 const char* PageMenus[NONE] =
	 {
		 "DbNx  Mode       LoNx",	// OSB menu for the SYS page: select next DebugDisplay page, enter DRIVE mode, select next LoaclDisplay page
		 "SYSp  Mode       LoNx",	// OSB menu for the POW page: select SYS page, select current drive DriveMode page, 
		 "SYSp  Mode WiFi  LoNx",	// OSB menu for the COM page: select SYS page, select current drive DriveMode page, connect to WiFi, 
		 "SYSp  Mode Scan  LoNx",	// OSB menu for the I2C page: select SYS page, select current drive DriveMode page, rescan I2C bus, 
		 "SYSp  Mode UPDT  LoNx",	// OSB menu for the ENV page: select SYS page, select current drive DriveMode page, force update of environment measurements, 
		 "SYSp  Mode       LoNx",	// OSB menu for the IMU page: select SYS page, select current drive DriveMode page, 

		 "SYSp                >",	// OSB menu for the DRV mode page: select SYS page, 
		 "SYSp                >",	// OSB menu for the HDG mode page: select SYS page, 
		 "SYSp                >",	// OSB menu for the WPT mode page: select SYS page, 
		 "SYSp  Prev Next  >WPT",	// OSB menu for the SEQ mode page: select SYS page, select previous waypoint in sequence, select next waypoint in sequence, 
									//enter WPT mode with the currently selected waypoint as the target

		 //"<NONE    SYS     POW>",
		 //"<SYS     POW     COM>",
		 //"<POW     COM     I2C>",
		 //"<COM     I2C     nxt>",
		 //"<I2C     ENV     IMU>",
		 //"<ENV     IMU     nxt>",

		 //"<       DRIVE       >",
		 //"<   Heading Hold    >",
		 //"<  Waypoint Direct  >",
		 //"< Waypoint Sequence >",

	 };

	 const char* DriveModeHeadings[CSSMStatusClass::DriveModes::NoDriveMode] =
	 {
		 "DRIVE",
		 " HDG ",
		 " WPT ",
		 " SEQ ",
	 };

	 const char* ComModeHeadings[CSSMStatusClass::ComModes::NoComs] =
	 {
		 "MRS RC MCC UART2",
		 "Direct WiFi/TCP ",
		 "Direct ESP-NOW  ",
	 };

	 void DrawPageHeaderAndFooter();
	 void DrawSYSPage();
	 void DrawPOWPage();
	 void DrawCOMPage();
	 void DrawI2CPage();
	 void DrawENVPage();
	 void DrawIMUPage();

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
		POWPage,
		COMPage,
		I2CScan,
		I2CPage,
		ENVPage,
		IMUPage,

		DRVPage,
		HDGPage,
		WPTPage,
		SEQPage,

		Prev,
		Next,

		OSB01Pressed,
		OSB02Pressed,
		OSB03Pressed,
		OSB04Pressed,
		OSB05Pressed,
		OSB06Pressed,

		Last
	};

	LocalDisplayClass();			// Default constructor
	bool Init(uint8_t address);
	bool Test();
	void Update();
	void Control(uint8_t command);
	Pages GetCurrentPage();
	bool IsOnSYSPage();
	bool IsOnPOWPage();
	bool IsOnCOMPage();
	bool IsOnI2CPage();
	bool IsOnENVPage();

	void ShowCurrentDriveModePage();

};

extern LocalDisplayClass LocalDisplay;

#endif

