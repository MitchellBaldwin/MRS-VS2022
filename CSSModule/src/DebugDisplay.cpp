/*	DebugDisplayClass - Base class for implementing paged graphical display interface for
*	Arduino and ESP32 based embedded control systems.
*
*	Mitchell Baldwin copyright 2023
*
*/

#include "DebugDisplay.h"
#include "CSSMStatus.h"
#include "CSSMSensorData.h"
#include <WiFi.h>

void DebugDisplayClass::DrawPageHeaderAndFooter()
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

void DebugDisplayClass::DrawHOMPage()
{
	currentPage = HOME;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		lastPage = currentPage;
	}

	// Update dynamic displays:
	display.fillRect(0, 16, 128, 32, SSD1306_BLACK);
	for (int i = 0; i < MAX_TEXT_LINES; ++i)
	{
		display.setCursor(0, 16 + i * 8);
		display.write(lines[i].c_str());
	}
	//if (!CSSMStatus.WiFiStatus)
	//{
	//	snprintf(buf, 22, "Initializing WiFi");
	//	display.setCursor(0, 32);
	//	display.write(buf);
	//}

	display.fillRect(0, 48, 128, 8, SSD1306_BLACK);
	snprintf(buf, 22, "KP %04d %s", SensorData.GetKBRaw(), SensorData.GetKPString("%#5.2f"));
	display.setCursor(0, 48);
	display.write(buf);

	display.display();
}

void DebugDisplayClass::DrawESPPage()
{
	currentPage = ESPP;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		snprintf(buf, 22, "%s %d core", ESP.getChipModel(), ESP.getChipCores());
		display.setCursor(0, 16);
		display.write(buf);
		
		snprintf(buf, 22, "CPU v%d %d MHz", ESP.getChipRevision(), ESP.getCpuFreqMHz());
		display.setCursor(0, 24);
		display.write(buf);
		
		uint8_t mac[6];
		esp_efuse_mac_get_default(mac);		// Returns the same MAC as does WiFi.madAddress(uint8_t*)
		//esp_efuse_mac_get_custom(mac);
		snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		//snprintf(buf, 22, "MAC:" MACSTR, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		display.setCursor(0, 32);
		display.write(buf);

		WiFi.macAddress(mac);
		snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		display.setCursor(0, 40);
		display.write(buf);
		
		lastPage = currentPage;
	}

	// Update dynamic displays:

	display.display();
}

void DebugDisplayClass::DrawMEMPage()
{
	currentPage = MEM;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();
		
		snprintf(buf, 22, "Heap: %d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
		display.setCursor(0, 16);
		display.write(buf);

		snprintf(buf, 22, "Prog: %d/%d", ESP.getSketchSize(), ESP.getFreeSketchSpace());
		display.setCursor(0, 24);
		display.write(buf);
		
		if (ESP.getPsramSize() > 0)
		{
			snprintf(buf, 22, "PSRAM: %d/%d", ESP.getFreePsram(), ESP.getPsramSize());
		}
		else
		{
			snprintf(buf, 22, "No PSRAM");
		}
		display.setCursor(0, 32);
		display.write(buf);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	display.display();
}

void DebugDisplayClass::DrawWIFIPage()
{
	currentPage = WIFI;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		snprintf(buf, 22, "%s %d dBm", WiFi.SSID().c_str(), WiFi.RSSI());
		display.setCursor(0, 16);
		display.write(buf);

		snprintf(buf, 22, "IP: %s", WiFi.localIP().toString());
		display.setCursor(0, 24);
		display.write(buf);

		uint8_t mac[6];
		WiFi.macAddress(mac);
		snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		display.setCursor(0, 40);
		display.write(buf);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	display.display();
}

void DebugDisplayClass::DrawNONEPage()
{
	currentPage = NONE;
	display.clearDisplay();
	lastPage = NONE;
	display.display();
}

DebugDisplayClass::DebugDisplayClass()
{
	display = Adafruit_SSD1306(DEBUG_SCREEN_WIDTH, DEBUG_SCREEN_HEIGHT, &Wire, DEBUG_OLED_RESET);
}

bool DebugDisplayClass::Init(uint8_t address)
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

bool DebugDisplayClass::Test()
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

void DebugDisplayClass::Update()
{
	switch (currentPage)
	{
	case HOME:
		DrawHOMPage();
		break;
	case ESPP:
		DrawESPPage();
		break;
	case MEM:
		DrawMEMPage();
		break;
	case WIFI:
		DrawWIFIPage();
		break;

	default:
		DrawNONEPage();
	}
}

void DebugDisplayClass::Control(uint8_t command)
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
	case HOMPage:
		DrawHOMPage();
		break;
	case ESPPage:
		DrawESPPage();
		break;
	case MEMPage:
		DrawMEMPage();
		break;
	case WIFIPage:
		DrawWIFIPage();
		break;
	case Commands::Prev:
		if (currentPage > Pages::HOME)
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
			currentPage = Pages::HOME;
		}
		Update();
		break;
	default:
		return;
	}
}

void DebugDisplayClass::AddTextLine(String textLine)
{
	if (curLine >= MAX_TEXT_LINES)
	{
		for (curLine = 0; curLine < MAX_TEXT_LINES - 1; ++curLine)
		{
			lines[curLine] = lines[curLine + 1];
		}
	}
	lines[curLine++] = textLine;
}

void DebugDisplayClass::ClearText()
{
	for (int i = 0; i < MAX_TEXT_LINES; ++i)
	{
		lines[i] = "";
	}
}


DebugDisplayClass DebugDisplay;

