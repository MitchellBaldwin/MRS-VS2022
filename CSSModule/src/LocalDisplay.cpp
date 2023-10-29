/*	LocalDisplayClass - Base class for implementing paged graphical display interface for
*	Arduino and ESP32 based embedded control systems.
*
*	Mitchell Baldwin copyright 2023
*
*/

#include "LocalDisplay.h"
//#include "CSSMGlobals.h"
#include "CSSMStatus.h"
#include "I2CBus.h"
#include "CSSMSensorData.h"

void LocalDisplayClass::DrawPageHeaderAndFooter()
{
	display.clearDisplay();
	display.setCursor(0, 0);
	display.cp437();
	display.setTextSize(1);
	display.write(PageTitles[currentPage]);
	snprintf(buf, 22, "v%d.%d", CSSMStatus.MajorVersion, CSSMStatus.MinorVersion);
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
		char downArrow[1] = { 0x19 };
		sprintf(buf, "U0 %s", CSSMStatus.UART0Status ? upArrow : downArrow);
		display.setCursor(0, 40);
		display.write(buf, 4);
		sprintf(buf, "U2 %s", CSSMStatus.UART2Status ? upArrow : downArrow);
		display.setCursor(32, 40);
		display.write(buf, 4);
		sprintf(buf, "ENV%s", CSSMStatus.BME280Status ? upArrow : downArrow);
		display.setCursor(64, 40);
		display.write(buf, 4);
		sprintf(buf, "IMU%s", CSSMStatus.IMUStatus ? upArrow : downArrow);
		display.setCursor(92, 40);
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
	//snprintf(buf, 22, "U/D %+04d     F/R %+04d", JSPkt.PTJSY, JSPkt.DrvJSY);
	display.setCursor(0, 16);
	//display.write(buf);
	display.fillRect(0, 24, 128, 8, 0x0000);
	//snprintf(buf, 22, "L/R %+04d     L/R %+04d", JSPkt.PTJSX, JSPkt.DrvJSX);
	display.setCursor(0, 24);
	//display.write(buf);

	display.fillRect(0, 32, 128, 8, SSD1306_BLACK);
	snprintf(buf, 22, "KP %04d %#5.2f %s", SensorData.GetKBRaw(), SensorData.KPVoltage.GetAverageRealValue(), SensorData.KPVoltage.Units);
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
	display.fillRect(0, 32, 128, 8, SSD1306_BLACK);
	snprintf(buf, 22, "VIN %#5.2f %s", SensorData.ESP32VIN.GetRealValue(), SensorData.ESP32VIN.Units);
	display.setCursor(0, 32);
	display.write(buf);

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

void LocalDisplayClass::DrawENVPage()
{
	currentPage = ENV;

	if (lastPage != currentPage)
	{
		DrawPageHeaderAndFooter();

		display.setCursor(0, 40);

		display.setCursor(64, 40);

		display.setCursor(0, 48);

		lastPage = currentPage;
	}

	// Update dynamic displays:
	display.fillRect(0, 24, 128, 8, SSD1306_BLACK);
	//snprintf(buf, 22, BME280Data.GetTchipString().c_str());
	display.setCursor(8, 24);
	String OutString = String("Tatm  ") + SensorData.ENVData.GetTchipString();
	display.write(OutString.c_str());

	display.fillRect(0, 32, 128, 8, SSD1306_BLACK);
	display.setCursor(8, 32);
	OutString = String("Pbaro ") + SensorData.ENVData.GetPbaroString();
	display.write(OutString.c_str());

	display.fillRect(0, 40, 128, 8, SSD1306_BLACK);
	display.setCursor(8, 40);
	OutString = String("RH    ") + SensorData.ENVData.GetRHString();
	display.write(OutString.c_str());

	display.display();
}

void LocalDisplayClass::DrawIMUPage()
{
	currentPage = IMU;

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

void LocalDisplayClass::DrawDRVPage()
{
	currentPage = DRV;

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

void LocalDisplayClass::DrawHDGPage()
{
	currentPage = HDG;

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

void LocalDisplayClass::DrawWPTPage()
{
	currentPage = WPT;

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

void LocalDisplayClass::DrawSEQPage()
{
	currentPage = SEQ;

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
		#ifdef _DEBUG_
		_PP(F("\nSSD1306 initialization failed; "));
		snprintf(buf, 31, "Firmware version: %02d.%02d", CSSMStatus.MajorVersion, CSSMStatus.MinorVersion);
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
	case ENV:
		DrawENVPage();
		break;
	case IMU:
		DrawIMUPage();
		break;
	
	case DRV:
		DrawDRVPage();
		break;
	case HDG:
		DrawHDGPage();
		break;
	case WPT:
		DrawWPTPage();
		break;
	case SEQ:
		DrawSEQPage();
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
	case ENVPage:
		DrawENVPage();
		break;
	case IMUPage:
		DrawIMUPage();
		break;

	case DRVPage:
		DrawDRVPage();
		break;
	case HDGPage:
		DrawHDGPage();
		break;
	case WPTPage:
		DrawWPTPage();
		break;
	case SEQPage:
		DrawSEQPage();
		break;

	case Commands::Prev:
		if (currentPage == Pages::SYS)
		{
			currentPage = Pages::NONE;
		}
		else if (currentPage > Pages::SYS)
		{
			currentPage = (Pages)((byte)currentPage - 1);
		}
		else
		{
			currentPage = Pages::NONE;
		}
		Update();
		break;
	case Next:
		if (currentPage == Pages::NONE)
		{
			currentPage = Pages::SYS;
		}
		else if (currentPage < Pages::IMU)
		{
			currentPage = (Pages)((byte)currentPage + 1);
		}
		else
		{
			currentPage = Pages::NONE;
		}
		Update();
		break;
	default:
		return;
	}
}

LocalDisplayClass LocalDisplay;

