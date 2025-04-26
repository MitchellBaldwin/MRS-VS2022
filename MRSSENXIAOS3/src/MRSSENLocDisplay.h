/*	MRSSENLocDisplay.h
*	MRSSENLocDisplay - Base class for managing setup and display of status information on the local oLED display
*	of the MRS Sensors module
*
* Configuration notes:
*	Board: Seeed Studio XIAO ESP32-S3 on a Seeed Studio XIAO Expansion Board 
*	Local display: 128x64 monochrome oLED; Adafruit GFX & SSD1306
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.00:	Initial data structure
*	v
*
*/

#ifndef _MRSSENLocDisplay_h
#define _MRSSENLocDisplay_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
constexpr uint8_t DefaultOLEDAddress = 0x3C;
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET = -1;				// Reset pin # (or -1 if sharing Arduino reset pin)

class MRSSENLocDisplay
{
protected:
	char buf[32];
	Adafruit_SSD1306* display;

public:
	bool Init();
	bool Test();
	Adafruit_SSD1306* GetDisplay();
	void AddLine(const int lineNo, char* text);
	void Update();
};

extern MRSSENLocDisplay mrsSENLocDisplay;

#endif

