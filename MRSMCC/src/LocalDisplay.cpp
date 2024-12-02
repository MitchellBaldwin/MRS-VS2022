/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS Master Communications Controller (MRS MCC)
*
*	Mitchell Baldwin copyright 2024
*/

#include "LocalDisplay.h"
#include <I2CBus.h>
#include "MCCStatus.h"
//#include "PCMSensorData.h"
//#include "PCMControls.h"
#include "DEBUG Macros.h"

#include <WiFi.h>

void LocalDisplayClass::DrawPageHeaderAndFooter()
{
	// Clear display:
	tft.fillScreen(TFT_BLACK);

	// Draw header:
	tft.setTextSize(1);
	tft.setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft.setTextDatum(TL_DATUM);
	tft.drawString("MRSRC PCM", 2, 2);
	sprintf(buf, "v%d.%d", MCCStatus.MajorVersion, MCCStatus.MinorVersion);
	tft.drawString(buf, 2, 12);

	tft.setTextDatum(TR_DATUM);
	sprintf(buf, "%s", PageTitles[currentPage]);
	tft.drawString(buf, tft.width() - 2, 2);

	//// Draw footer menu:
	//if (MCCControls.MainMenu != nullptr)
	//{
	//	MCCControls.MainMenu->Draw();
	//}
	// 
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
		sprintf(buf, "UART0 %s", MCCStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", MCCStatus.UART1Status ? "OK" : "NO");
		tft.drawString(buf, tft.width() / 2 - 2, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[MCCStatus.ComMode], 2, 40);

		tft.setTextColor(TFT_ORANGE);
		sprintf(buf, "WiFi %s", MCCStatus.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, 2, 50);

		lastPage = currentPage;
	}

	// Update dynamic displays:

	int16_t cursorY = tft.height() - 20;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	sprintf(buf, "%s HDG %+04d CRS %+04d wXY %+6.1f%% THR %+6.1f%% ", DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode], MCCStatus.cssmDrivePacket.HeadingSetting, MCCStatus.cssmDrivePacket.CourseSetting, MCCStatus.cssmDrivePacket.OmegaXY, MCCStatus.cssmDrivePacket.Throttle);
	tft.drawString(buf, 2, 60);
	tft.drawString(MCCStatus.IncomingPacketMACString, 2, 70);

}

void LocalDisplayClass::DrawCOMPage()
{
	currentPage = COM;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		int32_t halfScreenWidth = tft.width() / 2;
		int32_t halfScreenHeight = tft.height() / 2;

		tft.setTextSize(1);
		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);	//DONE: setTextDatum has NO AFFECT on print() output; print() effectively uses default TL_DATUM
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, 2, halfScreenHeight + 30);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "IP: %s", WiFi.localIP().toString());
		tft.drawString(buf, 2, halfScreenHeight + 40);

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, halfScreenHeight + 50);

		tft.setTextColor(TFT_LIGHTGREY);
		sprintf(buf, "UART0 %s", MCCStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", MCCStatus.UART1Status ? "OK" : "NO");
		tft.drawString(buf, halfScreenWidth, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[MCCStatus.ComMode], 2, 40);

		tft.setTextColor(TFT_GREENYELLOW);
		sprintf(buf, "ESPNow %s", MCCStatus.ESPNOWStatus ? "OK" : "NO");
		tft.drawString(buf, 2, 50);

		tft.setTextColor(TFT_GREEN);
		sprintf(buf, "WiFi %s", MCCStatus.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, 2, 60);

		lastPage = currentPage;
	}

}

void LocalDisplayClass::DrawDBGPage()
{
	currentPage = DBG;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
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
		tft.setTextDatum(CL_DATUM);	//DONE: setTextDatum has NO AFFECT on print() output; print() effectively uses default TL_DATUM
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
		sprintf(buf, "UART0 %s", MCCStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, halfScreenHeight);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", MCCStatus.UART1Status ? "OK" : "NO");
		tft.drawString(buf, halfScreenWidth, halfScreenHeight);

		//tft.setTextColor(TFT_PINK);
		//tft.setTextDatum(CL_DATUM);
		//tft.drawString(ComModeHeadings[PCMStatus.ComMode], 2, 40);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "ESPNow %s", MCCStatus.ESPNOWStatus ? "OK" : "NO");
		tft.drawString(buf, 2, halfScreenHeight + 10);

		tft.setTextColor(TFT_GREEN);
		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "WiFi %s", MCCStatus.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, halfScreenWidth, halfScreenHeight + 10);

		tft.setTextColor(TFT_GREENYELLOW);
		tft.setTextDatum(CL_DATUM);
		for (int i = 0; i < MAX_TEXT_LINES; ++i)
		{
			tft.drawString(MCCStatus.debugTextLines[i].c_str(), halfScreenWidth + 2, 30 + i * 10);
		}

		lastPage = currentPage;
	}

	// Update dynamic displays:

	// Display rotary encoder settings:
	//tft.setTextDatum(BL_DATUM);
	//tft.setTextSize(1);
	//tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	//sprintf(buf, "%04D", MCCControls.NavSetting);
	//tft.drawString(buf, 2, tft.height() - 2);

	//tft.setTextDatum(BR_DATUM);
	//sprintf(buf, "%04D", MCCControls.FuncSetting);
	//tft.drawString(buf, tft.width() - 2, tft.height() - 2);


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
	SetDisplayBrightness(DefaultDisplayBrightness);

	tft.init();
	tft.setRotation(3);

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

void LocalDisplayClass::SetCurrentPage(Pages page)
{
	LocalDisplayClass::currentPage = page;
}

LocalDisplayClass::Pages LocalDisplayClass::GetCurrentPage()
{
	return currentPage;
}

void LocalDisplayClass::PrevPage(byte /*value*/)
{
	//TODO: Untested
	if (currentPage <= LocalDisplayClass::Pages::SYS)
	{
		currentPage = LocalDisplayClass::Pages::NONE;
	}
	else
	{
		int page = currentPage;
		page--;
		currentPage = (LocalDisplayClass::Pages)page;
	}
}

void LocalDisplayClass::NextPage(byte /*value*/)
{
	if (currentPage >= LocalDisplayClass::Pages::NONE)
	{
		currentPage = LocalDisplayClass::Pages::SYS;
	}
	else
	{
		int page = currentPage;
		page++;
		currentPage = (LocalDisplayClass::Pages)page;
	}
}

void LocalDisplayClass::RefreshCurrentPage()
{
	lastPage = NONE;
	SetCurrentPage(currentPage);
	Update();
}

void LocalDisplayClass::RefreshPage(Pages page)
{
	if (page == currentPage)
	{
		RefreshCurrentPage();
	}
}

void LocalDisplayClass::ReportHeapStatus()
{
	sprintf(buf, "Heap (F/T): %d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
	MCCStatus.AddDebugTextLine(buf);
	RefreshPage(DBG);
}

void LocalDisplayClass::AddDebugTextLine(String newLine)
{
	MCCStatus.AddDebugTextLine(newLine);
	RefreshPage(DBG);
}

TFT_eSPI* LocalDisplayClass::GetTFT()
{
	return &tft;
}

void LocalDisplayClass::SetDisplayBrightness(byte brightness)
{
	Brightness = brightness;
	analogWrite(LCD_BL, Brightness);
}

byte LocalDisplayClass::GetDisplayBrightness()
{
	return Brightness;
}

LocalDisplayClass LocalDisplay;
byte LocalDisplayClass::Brightness = DefaultDisplayBrightness;
LocalDisplayClass::Pages LocalDisplayClass::currentPage = LocalDisplayClass::Pages::SYS;
