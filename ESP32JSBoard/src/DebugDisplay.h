/*	DebugDisplayClass - Based on template class for implementing paged graphical display interface for
*	Arduino and ESP32 based embedded control systems.
*
*	Extended to serve as local display for the ESP32 based Control Stick Steering (CSS) module
*	Pages supported:
*		Home	(HOME)	Default scrolling display of debug messages
*		ESP		(ESP)	ESP32 chip & module info
*		Memory	(MEM)	Memory status info
*		WiFi	(WIFI)	WiFi, ESP NOW & network status info
* 
*		None	(NONE)	Blank display (to conserve power)
*
*	Mitchell Baldwin copyright 2023
*
*	v 0.00:	Initial command set
*	v
*
*/

#ifndef _DEBUGDISPLAY_h
#define _DEBUGDISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

constexpr int DEBUG_SCREEN_WIDTH = 128;	// OLED display width, in pixels
constexpr int DEBUG_SCREEN_HEIGHT = 64;	// OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
constexpr int DEBUG_OLED_RESET = -1;	// Reset pin # (or -1 if sharing Arduino reset pin)

class DebugDisplayClass
{
protected:
	Adafruit_SSD1306 display;
	uint8_t I2CAddress = 0x00;
	char buf[32];

	enum Pages
	{
		HOME,
		ESPP,
		MEM,
		WIFI,

		NONE
	};
	Pages currentPage = HOME;
	Pages lastPage = NONE;		// Aid to determine when a complete page redraw is needed

	const char* PageTitles[NONE] =
	{
		"MRS CSS    Debug Home",
		"MRS CSS  Debug  ESP32",
		"MRS CSS  Debug Memory",
		"MRS CSS    Debug WiFi",

	};

	const char* PageMenus[NONE] =
	{
		"<NONE    HOM     ESP>",
		"<HOM     ESP     MEM>",
		"<ESP     MEM    WIFI>",
		"<MEM    WIFI    NONE>",

	};

	void DrawPageHeaderAndFooter();
	void DrawHOMPage();
	void DrawESPPage();
	void DrawMEMPage();
	void DrawWIFIPage();

	void DrawNONEPage();

public:
	enum Commands
	{
		Clear,
		Refresh,
		HOMPage,
		ESPPage,
		MEMPage,
		WIFIPage,
		Prev,
		Next,

		Last
	};

	DebugDisplayClass();			// Default constructor
	bool Init(uint8_t address);
	bool Test();
	void Update();
	void Control(uint8_t command);

};

extern DebugDisplayClass DebugDisplay;

#endif

