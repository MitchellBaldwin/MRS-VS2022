/*	CSSMS3Display.cpp
*	CSSMS3Display - Base class for managing function and presenting data and graphics through the AMOLED
*	display integrated with the module MCU (LilyGO T-Display S3 )
*
*/

#include "CSSMS3Display.h"
#include "CSSMS3Controls.h"
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
	sprintf(buf, "MRS RC CSSMS3 v%d.%d", CSSMS3Status.MajorVersion, CSSMS3Status.MinorVersion);
	tft.drawString(buf, 2, 2);

	tft.setTextColor(TFT_SKYBLUE, TFT_BLACK, false);
	tft.setTextDatum(TC_DATUM);
	sprintf(buf, "%s", PageTitles[currentPage]);
	tft.drawString(buf, tft.width() / 2, 2);

	tft.setTextColor(TFT_CYAN);
	tft.setTextDatum(TR_DATUM);
	tft.drawString(ComModeHeadings[CSSMS3Status.ComMode], tft.width() - 2, 2);

}

void CSSMS3Display::DrawDashboard()
{
	tft.setTextDatum(CL_DATUM);
	int16_t cursorY = tft.height() / 2 - 20;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	sprintf(buf, "%s LThr %+6.1f%% RThr %+6.1f%% ",
		DriveModeHeadings[CSSMS3Status.cssmDrivePacket.DriveMode],
		CSSMS3Status.cssmDrivePacket.LThrottle,
		CSSMS3Status.cssmDrivePacket.RThrottle);
	tft.drawString(buf, 2, cursorY);

	cursorY = tft.height() / 2 - 10;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
	sprintf(buf, "HDG %+04d CRS %+04d wXY %+06.1f%% Speed %+06.1f%% ",
		CSSMS3Status.cssmDrivePacket.HeadingSetting,
		CSSMS3Status.cssmDrivePacket.CourseSetting,
		CSSMS3Status.cssmDrivePacket.OmegaXY,
		CSSMS3Status.cssmDrivePacket.Speed);
	tft.drawString(buf, 2, cursorY);
}

void CSSMS3Display::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);

		//tft.setTextColor(TFT_LIGHTGREY);
		//tft.setTextDatum(CL_DATUM);
		//sprintf(buf, "UART0 %s", CSSMS3Status.UART0Status ? "OK" : "NO");
		//tft.drawString(buf, 2, 30);

		//tft.setTextDatum(CR_DATUM);
		//sprintf(buf, "UART1 %s", CSSMS3Status.UART1Status ? "OK" : "NO");
		//tft.drawString(buf, tft.width() / 2 - 2, 30);

		tft.setTextColor(TFT_ORANGE);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "WiFi %s  IP: %s", CSSMS3Status.WiFiStatus ? "OK" : "NO", WiFi.localIP().toString());
		tft.drawString(buf, 2, 30);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CR_DATUM);
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, tft.width() - 2, 30);

		tft.setTextColor(TFT_CYAN);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, 40);

		// Draw footer menu:
		if (cssmS3Controls.MainMenu != nullptr)
		{
			cssmS3Controls.MainMenu->Draw();
		}

		lastPage = currentPage;
	}

	// Update dynamic displays:

	DrawDashboard();
	
	//tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	//tft.setTextDatum(CC_DATUM);
	//sprintf(buf, "%5d ms", CSSMS3Status.CSSMPacketReceiptInterval);
	//tft.drawString(buf, tft.width() / 2, 40);

	tft.setTextColor(TFT_PINK, TFT_BLACK, true);
	tft.setTextDatum(CR_DATUM);
	sprintf(buf, "%s %s", "CSSM Uplink ", CSSMS3Status.MRSMCCESPNOWLinkStatus ? "OK" : "NO");
	tft.drawString(buf, tft.width() - 2, 40);


	int16_t cursorY = tft.height() / 2 + 10;
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "KP %4d %s", cssmS3Controls.GetKPRawADC(), cssmS3Controls.GetKPVoltageString());
	tft.drawString(buf, 2, cursorY);

	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	sprintf(buf, "VMCU %s", cssmS3Controls.GetMCUVoltageString());
	tft.drawString(buf, tft.width() / 2, cursorY);

}

void CSSMS3Display::DrawCOMPage()
{
	currentPage = COM;

	if (lastPage != currentPage)
	{
		DrawPageHeaderAndFooter();

		int32_t halfScreenWidth = tft.width() / 2;
		int32_t halfScreenHeight = tft.height() / 2;

		tft.setTextSize(1);

		tft.setTextDatum(CL_DATUM);
		tft.setTextColor(TFT_LIGHTGREY);
		sprintf(buf, "UART0 %s", CSSMS3Status.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", CSSMS3Status.UART1Status ? "OK" : "NO");
		tft.drawString(buf, halfScreenWidth, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[CSSMS3Status.ComMode], 2, 40);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);	//DONE: setTextDatum has NO AFFECT on print() output; print() effectively uses default TL_DATUM
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, 2, halfScreenHeight);

		tft.setTextColor(TFT_GREENYELLOW);
		sprintf(buf, "ESPNow %s", CSSMS3Status.ESPNOWStatus ? "OK" : "NO");
		tft.drawString(buf, 2, halfScreenHeight + 10);

		tft.setTextDatum(CL_DATUM);
		tft.setTextColor(TFT_GREEN);
		sprintf(buf, "WiFi Ch %d %s %s", WiFi.channel(), CSSMS3Status.WiFiStatus ? "SSID:" : "NO", WiFi.SSID());
		tft.drawString(buf, 2, halfScreenHeight + 20);

		sprintf(buf, "IP: %s %d dBm", WiFi.localIP().toString(), WiFi.RSSI());
		tft.drawString(buf, 2, halfScreenHeight + 30);

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, halfScreenHeight + 50);

		// Draw footer menu:
		if (cssmS3Controls.CommsMenu != nullptr)
		{
			cssmS3Controls.CommsMenu->Draw();
		}

		lastPage = currentPage;
	}
}

void CSSMS3Display::DrawDBGPage()
{
	currentPage = DBG;

	if (lastPage != currentPage)
	{
		DrawPageHeaderAndFooter();

		int32_t halfScreenWidth = tft.width() / 2;
		int32_t halfScreenHeight = tft.height() / 2;

		tft.setTextSize(1);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "%s %d core", ESP.getChipModel(), ESP.getChipCores());
		tft.drawString(buf, 2, 30, 1);
		sprintf(buf, "CPU v%d %d MHz", ESP.getChipRevision(), ESP.getCpuFreqMHz());
		tft.drawString(buf, 2, 40, 1);

		tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
		sprintf(buf, "Heap (F/T): %d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
		tft.drawString(buf, 2, 50, 1);
		sprintf(buf, "Prog (U/F): %d/%d", ESP.getSketchSize(), ESP.getFreeSketchSpace());
		tft.drawString(buf, 2, 60, 1);
		if (ESP.getPsramSize() > 0)
		{
			sprintf(buf, "PSRAM: %d/%d", ESP.getFreePsram(), ESP.getPsramSize());
		}
		else
		{
			sprintf(buf, "No PSRAM");
		}
		tft.drawString(buf, 2, 70, 1);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, 2, halfScreenHeight + 20);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "SSID: %s %d dBm", WiFi.SSID().c_str(), WiFi.RSSI());
		tft.drawString(buf, 2, halfScreenHeight + 30, 1);
		sprintf(buf, "IP: %s", WiFi.localIP().toString());
		tft.drawString(buf, 2, halfScreenHeight + 40);

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, halfScreenHeight + 50);

		tft.setTextColor(TFT_LIGHTGREY);
		sprintf(buf, "UART0 %s", CSSMS3Status.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, halfScreenHeight);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", CSSMS3Status.UART1Status ? "OK" : "NO");
		tft.drawString(buf, halfScreenWidth, halfScreenHeight);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "ESPNow %s", CSSMS3Status.ESPNOWStatus ? "OK" : "NO");
		tft.drawString(buf, 2, halfScreenHeight + 10);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "WiFi %s", CSSMS3Status.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, halfScreenWidth, halfScreenHeight + 10);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);
		for (int i = 0; i < MAX_DEBUG_TEXT_LINES; ++i)
		{
			tft.drawString(CSSMS3Status.debugTextLines[i].c_str(), halfScreenWidth + 2, 20 + i * 10);
		}

		if (ShowingFontTable)
		{
			// Draw 16x16 font table:
			ShowFontTable(halfScreenWidth + 10, 10);
		}

		// Draw footer menu:
		if (cssmS3Controls.DebugMenu != nullptr)
		{
			cssmS3Controls.DebugMenu->Draw();
		}

		lastPage = currentPage;
	}
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

	tft.init();
	tft.setRotation(3);

	//Control(CSSMS3Display::Commands::SYSPage);
	cssmS3Display.Control(CSSMS3Display::Commands::DBGPage);
	SetDisplayBrightness(DefaultDisplayBrightness);

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

void CSSMS3Display::ReportHeapStatus(byte /*value*/)
{
	char buf[64];

	sprintf(buf, "Heap (F/T): %d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
	CSSMS3Status.AddDebugTextLine(buf);
	cssmS3Display.RefreshPage(CSSMS3Display::Pages::DBG);
}

void CSSMS3Display::ShowFontTableFixed(byte /*value*/)
{
	cssmS3Display.ShowingFontTable = !cssmS3Display.ShowingFontTable;
	cssmS3Display.RefreshCurrentPage();
	//cssmS3Display.ShowFontTable(cssmS3Display.tft.width() / 2 + 10, 10);
}

void CSSMS3Display::ShowFontTable(int32_t xTL, int32_t yTL)
{
	yTL -= 20;	// Subtract starting row index x row height (2 * 10 = 20)
	tft.fillRect(xTL, yTL, 128, 160, TFT_BLACK);
	tft.setTextDatum(TL_DATUM);
	tft.setTextColor(TFT_GOLD, TFT_BLACK, true);
	for (byte i = 2; i < 16; ++i)
	{
		for (byte j = 0; j < 16; ++j)
		{
			tft.drawString(String((char)(i * 16 + j)), xTL + 8 * j, yTL + 10 * i);
		}
	}
}


CSSMS3Display cssmS3Display;
byte CSSMS3Display::Brightness = DefaultDisplayBrightness;
CSSMS3Display::Pages CSSMS3Display::currentPage = CSSMS3Display::Pages::SYS;

