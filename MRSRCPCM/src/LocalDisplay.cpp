/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Power Control Module
*
*	Mitchell Baldwin copyright 2023-2024
*/

#include "LocalDisplay.h"
#include <I2CBus.h>
#include "PCMSensorData.h"
#include "PCMControls.h"

#include <WiFi.h>

void LocalDisplayClass::DrawPageHeaderAndFooter()
{
	// Draw header:
	tft.setTextSize(1);
	tft.setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft.setTextDatum(TL_DATUM);
	tft.drawString("MRSRC PCM", 2, 2);
	sprintf(buf, "v%d.%d", PCMStatus.MajorVersion, PCMStatus.MinorVersion);
	tft.drawString(buf, 2, 12);

	tft.setTextDatum(TR_DATUM);
	sprintf(buf, "%s", PageTitles[currentPage]);
	tft.drawString(buf, tft.width() - 2, 2);

	//// Draw footer:
	//tft.setTextDatum(BC_DATUM);
	//tft.setTextColor(TFT_MAGENTA, TFT_BLACK, false);
	//sprintf(buf, "%s", PageMenus[currentPage]);
	//tft.drawString(buf, tft.width() / 2, tft.height() - 2);

	// Draw footer menu:
	if (PCMControls.MainMenu != nullptr)
	{
		PCMControls.MainMenu->Draw();
	}
	//// Draw rectangle at screen bounds to aid framing physical display to panel:
	//tft.drawRect(0, 0, tft.getViewportWidth(), tft.getViewportHeight(), TFT_DARKCYAN);
}

void LocalDisplayClass::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);
		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);	//DONE: setTextDatum has NO AFFECT on print() output; print() effectively uses default TL_DATUM
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, 2, tft.height() / 2 + 30);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "IP: %s", WiFi.localIP().toString());
		tft.drawString(buf, 2, tft.height() / 2 + 40);

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, tft.height() / 2 + 50);

		tft.setTextColor(TFT_LIGHTGREY);
		sprintf(buf, "UART0 %s", PCMStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", PCMStatus.UART1Status ? "OK" : "NO");
		tft.drawString(buf, tft.width() / 2 - 2, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[PCMStatus.ComMode], 2, 40);

		tft.setTextColor(TFT_ORANGE);
		sprintf(buf, "WiFi %s", PCMStatus.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, 2, 50);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	int16_t cursorY = tft.height() - 20;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	
	// Measure & display backup battery voltage:
	tft.setTextDatum(BL_DATUM);
	tft.setTextSize(2);
	int32_t cursorX = tft.width() / 2 + 2;
	tft.drawString("V", cursorX, cursorY);
	cursorX += tft.textWidth("V", 2) + 1;
	tft.setTextSize(1);
	tft.drawString("MCU", cursorX, cursorY);	// Subscript
	tft.setTextSize(2);
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F V", SensorData.VMCU);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	// Measure & display backup battery voltage:
	tft.setTextDatum(BL_DATUM);
	tft.setTextSize(2);
	cursorX = tft.width() / 2 + 2;
	cursorY -= 20;
	tft.drawString("V", cursorX, cursorY);
	cursorX += tft.textWidth("V", 2) + 1;
	tft.setTextSize(1);
	tft.drawString("BBAT", cursorX, cursorY);	// Subscript
	tft.setTextSize(2);
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F V", SensorData.VBBat);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	// Measure & display internal 18650 cell voltage:
	tft.setTextDatum(BL_DATUM);
	tft.setTextSize(2);
	cursorX = tft.width() / 2 + 2;
	cursorY -= 20;
	tft.drawString("V", cursorX, cursorY);
	cursorX += tft.textWidth("V", 2) + 1;
	tft.setTextSize(1);
	tft.drawString("BAT", cursorX, cursorY);	// Subscript
	tft.setTextSize(2);
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F V", SensorData.VBat);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	// Measure & display internally regulated (5 v) voltage:
	tft.setTextDatum(BL_DATUM);
	tft.setTextSize(2);
	cursorX = tft.width() / 2 + 2;
	cursorY -= 20;
	tft.drawString("V", cursorX, cursorY);
	cursorX += tft.textWidth("V", 2) + 1;
	tft.setTextSize(1);
	tft.drawString("5", cursorX, cursorY);	// Subscript
	tft.setTextSize(2);
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F V", SensorData.V5);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	// Measure & display external supply voltage:
	tft.setTextDatum(BL_DATUM);
	tft.setTextSize(2);
	cursorX = tft.width() / 2 + 2;
	cursorY -= 20;
	tft.drawString("V", cursorX, cursorY);
	cursorX += tft.textWidth("V", 2) + 1;
	tft.setTextSize(1);
	tft.drawString("EXT", cursorX, cursorY);	// Subscript
	tft.setTextSize(2);
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F V", SensorData.VExt);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	// Display rotary encoder settings:
	tft.setTextDatum(BL_DATUM);
	tft.setTextSize(1);
	tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	sprintf(buf, "%04D", PCMControls.NavSetting);
	tft.drawString(buf, 2, tft.height() - 2);

	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%04D", PCMControls.FuncSetting);
	tft.drawString(buf, tft.width() - 2, tft.height() - 2);
}

void LocalDisplayClass::DrawCOMPage()
{
	currentPage = COM;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		char upArrow[1] = { 0x18 };
		char downArrow[1] = { 0x19 };

		lastPage = currentPage;
	}

}

void LocalDisplayClass::DrawNONEPage()
{
	currentPage = NONE;
	tft.fillScreen(TFT_BLACK);
	lastPage = NONE;
}

bool LocalDisplayClass::Init()
{
	// Display power is not eanbled by default when the board is powered through the LiPo battery connector
	//so must explicitly turn LCD power on:
	pinMode(LCD_POWER_ON, OUTPUT);
	digitalWrite(LCD_POWER_ON, HIGH);
	SetDisplayBrightness(128);

	tft.init();
	tft.setRotation(3);

	tft.fillScreen(TFT_BLACK);

	Control(LocalDisplayClass::SYSPage);

	return true;
}

void LocalDisplayClass::Update()
{
	switch (currentPage)
	{
	case SYS:
		DrawSYSPage();
		break;
	case COM:
		DrawCOMPage();
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

	case SYSPage:
		DrawSYSPage();
		break;

	default:
		break;
	}
}

LocalDisplayClass::Pages LocalDisplayClass::GetCurrentPage()
{
	return currentPage;
}

TFT_eSPI* LocalDisplayClass::GetTFT()
{
	return &tft;
}

void LocalDisplayClass::SetDisplayBrightness(byte brightness)
{
	Brightness = brightness;

}

LocalDisplayClass LocalDisplay;
byte LocalDisplayClass::Brightness = 128;