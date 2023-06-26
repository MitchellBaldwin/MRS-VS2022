/*	LocalDisplayClass - Base class for implementing paged graphical display interface for
*	Arduino and ESP32 based embedded control systems.
*
*
*
*
*	Mitchell Baldwin copyright 2023
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
 protected:
	 Adafruit_SSD1306 display;
	 uint8_t I2CAddress = 0x00;
	 char buf[32];

	 enum Pages
	 {
		 SYS,
		 POW,
		 COM,
		 I2C,

		 NONE
	 };
	 Pages currentPage = SYS;
	 Pages lastPage = NONE;		// Aid to determine when a complete page redraw is needed

	 const char* PageTitles[NONE] =
	 {
		 "MRS CSS        System",
		 "MRS CSS         Power",
		 "MRS CSS         Comms",
		 "MRS CSS           I2C",

	 };

	 const char* PageMenus[NONE] =
	 {
		 "<NONE    SYS     POW>",
		 "<SYS     POW     COM>",
		 "<POW     COM     I2C>",
		 "<COM     I2C     nxt>",

	 };

	 void DrawPageHeaderAndFooter();
	 void DrawSYSPage();
	 void DrawPOWPage();
	 void DrawCOMPage();
	 void DrawI2CPage();

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
		Prev,
		Next,

		Last
	};

	LocalDisplayClass();			// Default constructor
	bool Init(uint8_t address);
	bool Test();
	void Update();
	void Control(uint8_t command);

};

extern LocalDisplayClass LocalDisplay;

#endif

