/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*	Mitchell Baldwin copyright 2023-2024
*/

#include "LocalDisplay.h"
#include "I2CBus.h"
#include "NMStatus.h"
#include "NMControls.h"

#include <WiFi.h>

void LocalDisplayClass::DrawPageHeaderAndFooter()
{
	char upArrow[1] = { 0x18 };
	char downArrow[1] = { 0x19 };

	//tft.setCursor();	// Used with 'printxx' statements; not needed when using drawString()
	tft.setTextSize(1);
	tft.setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft.setTextDatum(TL_DATUM);
	tft.drawString("MRS RC NM", 2, 2, 1);

	////Test code:
	//tft.drawString(upArrow, 2, 12);	// Displays nothing...

	tft.setTextDatum(TR_DATUM);
	sprintf(buf, "%s", PageTitles[currentPage]);
	tft.drawString(buf, tft.width() - 2, 2);
	sprintf(buf, "v%d.%d", NMStatus.MajorVersion, NMStatus.MinorVersion);
	tft.drawString(buf, tft.width() - 2, 12);

	// Draw footer:
	tft.setTextDatum(BC_DATUM);
	tft.setTextColor(TFT_MAGENTA, TFT_BLACK, false);
	sprintf(buf, "%s", PageMenus[currentPage]);
	tft.drawString(buf, tft.width() / 2, tft.height() - 2);

	//Test code:
	TestFonts();
	
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

		char upArrow[1] = { 0x18 };
		char downArrow[1] = { 0x19 };

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
		sprintf(buf, "UART0 %s", NMStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART2 %s", NMStatus.UART2Status ? "OK" : "NO");
		tft.drawString(buf, tft.width() - 2, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[NMStatus.ComMode], 2, 40);

		tft.setTextColor(TFT_ORANGE);
		sprintf(buf, "WiFi %s", NMStatus.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, 2, 50);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	tft.setCursor(10, tft.height() / 2);
	tft.setTextSize(2);
	tft.setTextColor(TFT_YELLOW);
	tft.setTextDatum(CL_DATUM);
	tft.print("5.00 V");

	sprintf(buf, "%03D", NMControls.HDGSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
	tft.setTextDatum(BR_DATUM);
	tft.drawString(buf, tft.width() - 1, tft.height() - 1);
	//tft.setTextDatum(TL_DATUM);
	//tft.drawString(buf, tft.width() - tft.textWidth(buf) - 1, tft.height() - 9);
	if (NMControls.HDGSelected)
	{
		tft.drawRect(tft.width() - tft.textWidth(buf) - 2, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_GOLD);
	}
	else
	{
		tft.drawRect(tft.width() - tft.textWidth(buf) - 2, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_BLACK);
	}

	sprintf(buf, "%03D", NMControls.CRSSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
	tft.setTextDatum(BL_DATUM);
	tft.drawString(buf, 1, tft.height() - 1);
	if (NMControls.CRSSelected)
	{
		tft.drawRect(0, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_GOLD);
	}
	else
	{
		tft.drawRect(0, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_BLACK);
	}

	sprintf(buf, "BRT: %03D", NMControls.BRTSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft.setTextDatum(TL_DATUM);
	tft.drawString(buf, 2, 12);

	//sprintf(buf, "%03D", NMControls.TRRSetting);
	//tft.setTextSize(1);
	//tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	//tft.setTextDatum(TR_DATUM);
	//tft.drawString(buf, tft.width() - 50, 12);

	// Test OSB arrays:
	//TODO: Sset up OSB arrays either in the main NavModule code block or in NMControls
	constexpr byte LOSBAnalogPin = 34;
	constexpr byte ROSBAnalogPin = 35;

	tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	uint16_t OSBADC = analogRead(LOSBAnalogPin);
	sprintf(buf, "LOSB: %04D", OSBADC);
	tft.drawString(buf, 2, tft.height() - 24);
	OSBADC = analogRead(ROSBAnalogPin);
	sprintf(buf, "ROSB: %04D", OSBADC);
	tft.drawString(buf, tft.width() - tft.textWidth(buf), tft.height() - 24);
	

}

void LocalDisplayClass::DrawNAVPage()
{
	currentPage = NAV;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();


		lastPage = currentPage;
	}

	// Update dynamic displays:

}

void LocalDisplayClass::DrawCOMPage()
{
	currentPage = COM;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();


		lastPage = currentPage;
	}

	// Update dynamic displays:

}

void LocalDisplayClass::DrawDBGPage()
{
	currentPage = DBG;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		lastPage = currentPage;
	}

	// Update dynamic displays:

}

void LocalDisplayClass::DrawNONEPage()
{
	currentPage = NONE;
	tft.fillScreen(TFT_BLACK);
	lastPage = NONE;
}

bool LocalDisplayClass::Init()
{
	pinMode(LEDPin, OUTPUT);
	SetBrightness(Brightness);
	
	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	Control(LocalDisplayClass::SYSPage);

	return true;
}

void LocalDisplayClass::SetBrightness(byte brightness)
{
	Brightness = brightness;
	analogWrite(LEDPin, Brightness);
}

bool LocalDisplayClass::TestFonts()
{
	tft.setTextDatum(TL_DATUM);
	tft.setTextColor(TFT_GOLD, TFT_BLACK, true);
	for (byte i = 1; i <= MaxFonts; ++i)
	{
		tft.setTextSize(1);
		sprintf(buf, "Font %d", i);
		tft.drawString(buf, 1, 50 + 20 * i, i);

	}
	
	return true;
}

void LocalDisplayClass::Update()
{
	Brightness = NMControls.BRTSetting;
	SetBrightness(Brightness);
	
	switch (currentPage)
	{
	case SYS:
		DrawSYSPage();
		break;
	case NAV:
		DrawCOMPage();
		break;
	case COM:
		DrawCOMPage();
		break;
	case DBG:
		DrawDBGPage();
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
	case NAVPage:
		DrawDBGPage();
		break;
	case COMPage:
		DrawDBGPage();
		break;
	case DBGPage:
		DrawDBGPage();
		break;

	default:
		break;
	}
}

LocalDisplayClass::Pages LocalDisplayClass::GetCurrentPage()
{
	return currentPage;
}

LocalDisplayClass LocalDisplay;
