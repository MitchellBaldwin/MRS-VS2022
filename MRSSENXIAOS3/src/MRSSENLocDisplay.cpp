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
		display->setRotation(2);
		display->clearDisplay();
		display->cp437();
		display->setTextSize(1);
		display->setTextColor(SSD1306_WHITE);

		snprintf(buf, 31, "MRS Sensors v%d.%d", mrsSENStatus.MajorVersion, mrsSENStatus.MinorVersion);
		display->setCursor(0, 0);
		display->write(buf);
		display->setCursor(0, 10);
		display->write("Initializing:");
		snprintf(buf, 31, "Display %dw x %dh", display->width(), display->height());
		display->setCursor(10, 20);
		display->write(buf);
		display->setCursor(10, 30);
		display->write(mrsSENStatus.OTOSVersion.c_str());
		display->setCursor(10, 40);
		display->write(mrsSENStatus.FwdVL53L1XSWVersion.c_str());
		display->setCursor(10, 50);
		display->write(mrsSENStatus.GetDateTimeString().c_str());

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

void MRSSENLocDisplay::Update()
{
	display->fillRect(0, 40, display->width(), 10, SSD1306_BLACK);
	snprintf(buf, 31, "FwdVL53L1X %4d mm", mrsSENStatus.mrsSensorPacket.FWDVL53L1XRange);
	display->setCursor(10, 40);
	display->write(buf);

	display->fillRect(0, 50, display->width(), 10, SSD1306_BLACK);
	display->setCursor(10, 50);
	display->write(mrsSENStatus.GetDateTimeString().c_str());
	
	display->display();
}


MRSSENLocDisplay mrsSENLocDisplay;

