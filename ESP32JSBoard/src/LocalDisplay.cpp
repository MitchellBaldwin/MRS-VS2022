/*	LocalDisplayClass - Base class for implementing paged graphical display interface for
*	Arduino and ESP32 based embedded control systems.
*
*	Mitchell Baldwin copyright 2023
*
*/

#include "LocalDisplay.h"
#include "CSSMGlobals.h"
#include "I2CBus.h"
#include "CSSMSensorData.h"

void LocalDisplayClass::DrawPageHeaderAndFooter()
{
	display.clearDisplay();
	display.setCursor(0, 0);
	display.cp437();
	display.setTextSize(1);
	display.write(PageTitles[currentPage]);
	snprintf(buf, 22, "v%d.%d", MajorVersion, MinorVersion);
	display.setCursor(0, 8);
	display.write(buf);

	display.setCursor(0, 56);
	display.write(PageMenus[currentPage]);
}

void LocalDisplayClass::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		char upArrow[1] = { 0x18 };
		sprintf(buf, "U0 %s", upArrow);
		display.setCursor(0, 40);
		display.write(buf, 4);
		sprintf(buf, "U2 %s", upArrow);
		display.setCursor(64, 40);
		display.write(buf, 4);
		display.setCursor(0, 48);
		snprintf(buf, 22, "I2C %02X %02X %02X %02X %02X %02X",
			I2CBus.ActiveI2CDeviceAddresses[0],
			I2CBus.ActiveI2CDeviceAddresses[1],
			I2CBus.ActiveI2CDeviceAddresses[2],
			I2CBus.ActiveI2CDeviceAddresses[3],
			I2CBus.ActiveI2CDeviceAddresses[4],
			I2CBus.ActiveI2CDeviceAddresses[5]);
		display.write(buf);

		lastPage = currentPage;
	}

	// Update dynamic displays:
	display.fillRect(0, 16, 128, 8, SSD1306_BLACK);
	//snprintf(buf, 22, "C1: %+04d     C2: %+04d", SensorData.GetKBRaw(), SensorData.C2);
	display.setCursor(0, 16);
	//display.write(buf);
	display.fillRect(0, 24, 128, 8, 0x0000);
	//snprintf(buf, 22, "L/R %+04d     L/R %+04d", JSPkt.PTJSX, JSPkt.DrvJSX);
	display.setCursor(0, 24);
	//display.write(buf);

	display.fillRect(0, 32, 128, 8, SSD1306_BLACK);
	snprintf(buf, 22, "KP %05d", SensorData.GetKBRaw());
	display.setCursor(0, 32);
	display.write(buf);

	display.display();
}

void LocalDisplayClass::DrawPOWPage()
{
	currentPage = POW;

	if (lastPage != currentPage)
	{
		DrawPageHeaderAndFooter();

		display.setCursor(0, 40);

		display.setCursor(64, 40);

		display.setCursor(0, 48);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	display.display();
}

void LocalDisplayClass::DrawCOMPage()
{
	currentPage = COM;

	if (lastPage != currentPage)
	{
		DrawPageHeaderAndFooter();

		display.setCursor(0, 40);

		display.setCursor(64, 40);

		display.setCursor(0, 48);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	display.display();
}

void LocalDisplayClass::DrawI2CPage()
{
	currentPage = I2C;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		display.setCursor(0, 40);

		display.setCursor(64, 40);

		display.setCursor(0, 48);
		snprintf(buf, 22, "I2C %02X %02X %02X %02X %02X %02X",
			I2CBus.ActiveI2CDeviceAddresses[0],
			I2CBus.ActiveI2CDeviceAddresses[1],
			I2CBus.ActiveI2CDeviceAddresses[2],
			I2CBus.ActiveI2CDeviceAddresses[3],
			I2CBus.ActiveI2CDeviceAddresses[4],
			I2CBus.ActiveI2CDeviceAddresses[5]);
		display.write(buf);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	display.display();
}

void LocalDisplayClass::DrawNONEPage()
{
	currentPage = NONE;
	display.clearDisplay();
	lastPage = NONE;
	display.display();
}

LocalDisplayClass::LocalDisplayClass()
{
	display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

bool LocalDisplayClass::Init(uint8_t address)
{
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, address))
	{
		#if defined(_DEBUG_)
		_P(F("\nSSD1306 initialization failed; "));
		snprintf(buf, 31, "Firmware version: %02d.%02d", MajorVersion, MinorVersion);
		_PL(buf);
		#endif
		return false;
	}
	display.clearDisplay();
	display.setCursor(0, 0);
	display.cp437();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);

	display.display();

	return true;
}

bool LocalDisplayClass::Test()
{
	Wire.beginTransmission(I2CAddress);
	bool error = Wire.endTransmission();
	if (error) return false;

	display.clearDisplay();					// Clear the buffer
	display.setTextSize(1);					// Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE);	// Draw white text
	display.setCursor(0, 0);				// Start at top-left corner
	display.cp437(true);					// Use full 256 char 'Code Page 437' font

	// If all the characters will not fit on the display then the
	// library will draw what it can and the rest will be clipped.
	int16_t charCode = 0x30;				// ASCII '0'
	for (int16_t i = 0x00; i < 0x10; i++)
		for (int16_t j = 0x00; j < 0x0A; j++)
		{
			charCode = j + 0x30;
			if (charCode == '\n')	display.write(' ');
			else					display.write(charCode);
		}
	display.display();
	//delay(2000); // Pause for 2 seconds

	return true;
}

void LocalDisplayClass::Update()
{
	switch (currentPage)
	{
	case SYS:
		DrawSYSPage();
		break;
	case POW:
		DrawPOWPage();
		break;
	case COM:
		DrawCOMPage();
		break;
	case I2C:
		DrawI2CPage();
		break;

	default:
		DrawNONEPage();
	}
}

void LocalDisplayClass::Control(uint8_t command)
{
	switch (command)
	{
	case Clear:
		DrawNONEPage();
		break;
	case Refresh:
		lastPage = Pages::NONE;		// Will not refresh if currentPage == NONE...
		display.clearDisplay();		// This might fix refreshing when currentPage == NONE
		Update();
		break;
	case SYSPage:
		DrawSYSPage();
		break;
	case POWPage:
		DrawPOWPage();
		break;
	case COMPage:
		DrawCOMPage();
		break;
	case I2CScan:
		I2CBus.Scan();
		DrawI2CPage();
		break;
	case I2CPage:
		DrawI2CPage();
		break;
	case Commands::Prev:
		if (currentPage > Pages::SYS)
		{
			currentPage = (Pages)((byte)currentPage - 1);
		}
		else
		{
			currentPage = Pages::NONE;
		}
		break;
	case Next:
		if (currentPage < Pages::NONE)
		{
			currentPage = (Pages)((byte)currentPage + 1);
		}
		else
		{
			currentPage = Pages::SYS;
		}
		Update();
		break;
	default:
		return;
	}
}

LocalDisplayClass LocalDisplay;

