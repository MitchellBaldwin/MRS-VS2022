// DebugDisplay.h

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

		NONE
	};
	Pages currentPage = HOME;
	Pages lastPage = NONE;		// Aid to determine when a complete page redraw is needed

	const char* PageTitles[NONE] =
	{
		"MRS CSS    Debug Home",

	};

	const char* PageMenus[NONE] =
	{
		"<NONE    HOM    NONE>",

	};

	void DrawPageHeaderAndFooter();
	void DrawHOMPage();

	void DrawNONEPage();

public:
	enum Commands
	{
		Clear,
		Refresh,
		HOMPage,
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

