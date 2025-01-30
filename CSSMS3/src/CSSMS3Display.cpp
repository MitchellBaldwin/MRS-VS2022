/*	CSSMS3Display.cpp
*	CSSMS3Display - Base class for managing function and presenting data and graphics through the AMOLED
*	display integrated with the module MCU (LilyGO T-Display S3 )
*
*/

#include "CSSMS3Display.h"
#include <I2CBus.h>
#include <WiFi.h>

void CSSMS3Display::DrawPageHeaderAndFooter()
{
	// Clear display:
	tft.fillScreen(TFT_BLACK);

	// Draw header:
	tft.setTextSize(1);
	tft.setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft.setTextDatum(TL_DATUM);
	tft.drawString("MRSRC CSSMS3", 2, 2);
	sprintf(buf, "v%d.%d", CSSMS3Status.MajorVersion, CSSMS3Status.MinorVersion);
	tft.drawString(buf, 2, 12);

	tft.setTextDatum(TR_DATUM);
	sprintf(buf, "%s", PageTitles[currentPage]);
	tft.drawString(buf, tft.width() - 2, 2);

}

void CSSMS3Display::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);

		tft.setTextColor(TFT_LIGHTGREY);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "UART0 %s", CSSMS3Status.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", CSSMS3Status.UART1Status ? "OK" : "NO");
		tft.drawString(buf, tft.width() / 2 - 2, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[CSSMS3Status.ComMode], 2, 40);

		tft.setTextColor(TFT_ORANGE);
		sprintf(buf, "WiFi %s", CSSMS3Status.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, 2, 50);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "IP: %s", WiFi.localIP().toString());
		tft.drawString(buf, 50, 50);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CR_DATUM);
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, tft.width() - 2, 50);

		tft.setTextColor(TFT_CYAN);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, tft.height() / 2 + 50);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	//tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	//tft.setTextDatum(CC_DATUM);
	//sprintf(buf, "%5d ms", CSSMS3Status.CSSMPacketReceiptInterval);
	//tft.drawString(buf, tft.width() / 2, 40);

	tft.setTextColor(TFT_PINK, TFT_BLACK, true);
	tft.setTextDatum(CR_DATUM);
	sprintf(buf, "%s %s", "CSSM Uplink ", CSSMS3Status.CSSMESPNOWLinkStatus ? "OK" : "NO");
	tft.drawString(buf, tft.width() - 2, 40);

	tft.setTextDatum(CL_DATUM);
	int16_t cursorY = tft.height() / 2 - 20;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	sprintf(buf, "%s LThr %+6.1f%% RThr %+6.1f%% ",
		DriveModeHeadings[CSSMS3Status.cssmDrivePacket.DriveMode],
		CSSMS3Status.cssmDrivePacket.LThrottle,
		CSSMS3Status.cssmDrivePacket.RThrottle);
	tft.drawString(buf, 2, cursorY);

	cursorY = tft.height() / 2 - 10;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	sprintf(buf, "HDG %+04d CRS %+04d wXY %+06.1f%% Speed %+06.1f%% ",
		CSSMS3Status.cssmDrivePacket.HeadingSetting,
		CSSMS3Status.cssmDrivePacket.CourseSetting,
		CSSMS3Status.cssmDrivePacket.OmegaXY,
		CSSMS3Status.cssmDrivePacket.Speed);
	tft.drawString(buf, 2, cursorY);

}

void CSSMS3Display::DrawCOMPage()
{
}

void CSSMS3Display::DrawDBGPage()
{
}

void CSSMS3Display::DrawNONEPage()
{
	currentPage = NONE;
	tft.fillScreen(TFT_BLACK);
	lastPage = NONE;
}

bool CSSMS3Display::Init()
{
	// Display power is not eanbled by default when the board is powered through the LiPo battery connector
	//so must explicitly turn LCD power on:
	pinMode(LCD_POWER_ON, OUTPUT);
	digitalWrite(LCD_POWER_ON, HIGH);
	SetDisplayBrightness(DefaultDisplayBrightness);

	tft.init();
	tft.setRotation(3);

	Control(CSSMS3Display::Commands::SYSPage);

	return true;
}

bool CSSMS3Display::Test()
{
	return true;
}

void CSSMS3Display::Update()
{
	switch (currentPage)
	{
	case SYS:
		DrawSYSPage();
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

void CSSMS3Display::Control(uint8_t command)
{
	switch (command)
	{
	case Clear:
		DrawNONEPage();
		break;
	case SYSPage:
		DrawSYSPage();
		break;
	case COMPage:
		DrawCOMPage();
		break;
	case DBGPage:
		DrawDBGPage();
		break;
	case Next:
		NextPage(0);
		break;
	case Prev:
		PrevPage(0);
		break;

	default:
		break;
	}
}

void CSSMS3Display::SetCurrentPage(Pages page)
{
	CSSMS3Display::currentPage = page;
}

CSSMS3Display::Pages CSSMS3Display::GetCurrentPage()
{
	return currentPage;
}

void CSSMS3Display::RefreshCurrentPage()
{
	lastPage = NONE;
	SetCurrentPage(currentPage);
	Update();
}

void CSSMS3Display::RefreshPage(Pages page)
{
	if (page == currentPage)
	{
		RefreshCurrentPage();
	}
}

void CSSMS3Display::PrevPage(byte /*value*/)
{
	//TODO: Untested
	if (currentPage <= CSSMS3Display::Pages::SYS)
	{
		currentPage = CSSMS3Display::Pages::NONE;
	}
	else
	{
		int page = currentPage;
		page--;
		currentPage = (CSSMS3Display::Pages)page;
	}
}

void CSSMS3Display::NextPage(byte /*value*/)
{
	if (currentPage >= CSSMS3Display::Pages::NONE)
	{
		currentPage = CSSMS3Display::Pages::SYS;
	}
	else
	{
		int page = currentPage;
		page++;
		currentPage = (CSSMS3Display::Pages)page;
	}
}

TFT_eSPI* CSSMS3Display::GetTFT()
{
	return &tft;
}

byte CSSMS3Display::GetDisplayBrightness()
{
	return Brightness;
}

void CSSMS3Display::SetDisplayBrightness(byte brightness)
{
	Brightness = brightness;
	analogWrite(LCD_BL, Brightness);
}

void CSSMS3Display::AddDebugTextLine(String newLine)
{
	CSSMS3Status.AddDebugTextLine(newLine);
	RefreshPage(DBG);
}

void CSSMS3Display::ReportHeapStatus()
{
	sprintf(buf, "Heap (F/T): %d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
	CSSMS3Status.AddDebugTextLine(buf);
	RefreshPage(DBG);
}


CSSMS3Display cssmS3Display;
byte CSSMS3Display::Brightness = DefaultDisplayBrightness;
CSSMS3Display::Pages CSSMS3Display::currentPage = CSSMS3Display::Pages::SYS;

