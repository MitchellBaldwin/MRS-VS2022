/*	MRSSENLocDisplay.cpp
*	MRSSENLocDisplay - Base class for managing setup and display of status information on the local oLED display
*	of the MRS Sensors module
*
*/

#include "MRSSENLocDisplay.h"
#include "DEBUG Macros.h"
#include "MRSSENStatus.h"

bool MRSSENLocDisplay::Init()
{
	// Initialize local display:
	display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	bool success = display->begin(SSD1306_SWITCHCAPVCC, DefaultOLEDAddress);
	if (!success)
	{
#ifdef _DEBUG_
		_PP(F("\nSSD1306 initialization failed; "));
		snprintf(buf, 31, "Firmware version: %02d.%02d", mrsSENStatus.MajorVersion, mrsSENStatus.MinorVersion);
		_PL(buf);
#endif
		return success;
	}
	else
	{
		display->clearDisplay();
		display->setCursor(0, 0);
		display->cp437();
		display->setTextSize(1);
		display->setTextColor(SSD1306_WHITE);

		snprintf(buf, 31, "MRS Sensors v%d.%d", mrsSENStatus.MajorVersion, mrsSENStatus.MinorVersion);
		display->write(buf);
		display->display();
	}

	return success;
}

bool MRSSENLocDisplay::Test()
{
	bool success = !display->getWriteError();
	if (success)
	{
		display->clearDisplay();
		display->setCursor(0, 0);
		display->cp437();
		display->setTextSize(1);
		display->setTextColor(SSD1306_WHITE);

		// If all the characters will not fit on the display then the
		// library will draw what it can and the rest will be clipped.
		int16_t charCode = 0x30;				// ASCII '0'
		for (int16_t i = 0x00; i < 0x10; i++)
			for (int16_t j = 0x00; j < 0x0A; j++)
			{
				charCode = j + 0x30;
				if (charCode == '\n')	display->write(' ');
				else					display->write(charCode);
			}

		return true;
	}
	else
	{
		return false;
	}
}

Adafruit_SSD1306* MRSSENLocDisplay::GetDisplay()
{
	return display;
}

void MRSSENLocDisplay::AddLine(const int lineNo, char* text)
{
	display->setCursor(0, lineNo * 10);
	display->write(text);
	display->display();
}


MRSSENLocDisplay mrsSENLocDisplay;

