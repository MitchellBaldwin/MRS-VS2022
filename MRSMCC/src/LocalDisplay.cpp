/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS Master Communications Controller (MRS MCC)
*
*	Mitchell Baldwin copyright 2024-2025
*/

#include "LocalDisplay.h"
#include <I2CBus.h>
#include "MCCStatus.h"
#include "MCCControls.h"
#include "MCCSensors.h"
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

	// Draw footer menu:
	if (mccControls.MainMenu != nullptr)
	{
		mccControls.MainMenu->Draw();
	}
}

void LocalDisplayClass::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);

		tft.setTextColor(TFT_LIGHTGREY);
		tft.setTextDatum(CL_DATUM);
		sprintf(buf, "UART0 %s", MCCStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "RC2x15AUART %s", MCCStatus.RC2x15AUARTStatus ? "OK" : "NO");
		tft.drawString(buf, tft.width() / 2 - 2, 30);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(CL_DATUM);
		tft.drawString(ComModeHeadings[MCCStatus.ComMode], 2, 40);

		tft.setTextColor(TFT_ORANGE);
		sprintf(buf, "WiFi %s", MCCStatus.WiFiStatus ? "OK" : "NO");
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

	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	tft.setTextDatum(CC_DATUM);
	sprintf(buf, "%5d ms", MCCStatus.CSSMPacketReceiptInterval);
	tft.drawString(buf, tft.width() / 2, 40);

	tft.setTextColor(TFT_PINK, TFT_BLACK, true);
	tft.setTextDatum(CR_DATUM);
	sprintf(buf, "%s %s", "CSSM Uplink ", MCCStatus.CSSMESPNOWLinkStatus ? "OK" : "NO");
	tft.drawString(buf, tft.width() - 2, 40);

	tft.setTextDatum(CL_DATUM);
	int16_t cursorY = tft.height() / 2 - 20;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	switch (MCCStatus.cssmDrivePacket.DriveMode)
	{
	case CSSMDrivePacket::DriveModes::DRV:
		sprintf(buf, "%s HDG %+04d CRS %+04d wXY %+6.1f%% THR %+6.1f%% ", 
			DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode], 
			MCCStatus.cssmDrivePacket.HeadingSetting, 
			MCCStatus.cssmDrivePacket.CourseSetting, 
			MCCStatus.cssmDrivePacket.OmegaXYSetting, 
			MCCStatus.cssmDrivePacket.SpeedSetting);
		break;
	case CSSMDrivePacket::DriveModes::HDG:
		sprintf(buf, "%s",
			DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode]);

		break;
	case CSSMDrivePacket::DriveModes::WPT:
		sprintf(buf, "%s",
			DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode]);

		break;
	case CSSMDrivePacket::DriveModes::SEQ:
		sprintf(buf, "%s",
			DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode]);

			break;
	case CSSMDrivePacket::DriveModes::DRVTw:
		sprintf(buf, "%s HDG %+04d CRS %+04d wXY %+6.1f%% THR %+6.1f%% ", 
			DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode], 
			MCCStatus.cssmDrivePacket.HeadingSetting, 
			MCCStatus.cssmDrivePacket.CourseSetting, 
			MCCStatus.cssmDrivePacket.OmegaXYSetting, 
			MCCStatus.cssmDrivePacket.SpeedSetting);
		break;
	case CSSMDrivePacket::DriveModes::DRVLR:
		sprintf(buf, "%s HDG %+04d CRS %+04d LThr %+06.1f%% RThr %+06.1f%% ", 
			DriveModeHeadings[MCCStatus.cssmDrivePacket.DriveMode], 
			MCCStatus.cssmDrivePacket.HeadingSetting, 
			MCCStatus.cssmDrivePacket.CourseSetting, 
			MCCStatus.cssmDrivePacket.LThrottle, 
			MCCStatus.cssmDrivePacket.RThrottle);
		break;
	default:
			break;
	}
	tft.drawString(buf, 2, cursorY);

	//tft.drawString(MCCStatus.IncomingCSSMPacketMACString, 2, 70);

	// Display status of motor controller
	cursorY += 10;
	if (MCCStatus.RC2x15AUARTStatus)
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
		sprintf(buf, "RC2x15A connected   ");
		tft.drawString(buf, 2, cursorY);
		
	}
	else
	{
		tft.setTextColor(TFT_RED, TFT_BLACK, true);
		sprintf(buf, "RC2x15A disconnected");
		tft.drawString(buf, 2, cursorY);
	}

	// Two ways to display the ° character: as a string (%s) or as a char (%c)
	char degreeSymbol[2] = { 0xF7, 0x00 };
	cursorY += 10;
	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	if (MCCStatus.mcStatus.VBATValid)
	{
		sprintf(buf, "Vbat: %4.1f  T2: %4.1f%sC  T1: %4.1f%cC", MCCStatus.mcStatus.SupBatV, MCCStatus.mcStatus.Temp2, degreeSymbol, MCCStatus.mcStatus.Temp1, 0xF7);
	}
	else
	{
		sprintf(buf, "Vbat: ----  T2: ----%cC  T1: ----%cC", 0xF7, 0xF7);
	}
	tft.drawString(buf, 2, cursorY);
		
	cursorY += 10;
	if (MCCStatus.mcStatus.ENCPOSValid)
	{
		sprintf(buf, "POS: %12d %12d qp", MCCStatus.mcStatus.M2Encoder, MCCStatus.mcStatus.M1Encoder);
	}
	else
	{
		sprintf(buf, "POS:            -            - qp");
	}
	tft.drawString(buf, 2, cursorY);

	cursorY += 10;
	if (MCCStatus.mcStatus.SPEEDSValid)
	{
		sprintf(buf, "SPD: %+5d(%+5d) %+5d(%+5d) qpps", MCCStatus.mcStatus.M2Speed, MCCStatus.mcStatus.M2SpeedSetting, MCCStatus.mcStatus.M1Speed, MCCStatus.mcStatus.M1SpeedSetting);
	}
	else
	{
		sprintf(buf, "SPD:     -(    -)     -(    -) qpps");
	}
	tft.drawString(buf, 2, cursorY);

	cursorY += 10;
	if (MCCStatus.mcStatus.IMOTValid)
	{
		sprintf(buf, "Cur: %12.2f %12.2f A", MCCStatus.mcStatus.M2Current, MCCStatus.mcStatus.M1Current);
	}
	else
	{
		sprintf(buf, "Cur:            -            - A");
	}
	tft.drawString(buf, 2, cursorY);

	cursorY = tft.height() / 2 + 50;
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	sprintf(buf, "VMCU %s", mccSensors.GetMCUVoltageString());
	tft.drawString(buf, tft.width() / 2, cursorY);

	// Display rotary encoder settings:
	tft.setTextDatum(TL_DATUM);
	tft.setTextSize(1);
	tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	sprintf(buf, "%04D", mccControls.NavSetting);
	tft.drawString(buf, 2, 147);
	tft.setTextDatum(TR_DATUM);
	sprintf(buf, "%04D", mccControls.FuncSetting);
	tft.drawString(buf, tft.width() - 2, 147);

}

void LocalDisplayClass::DrawPOWPage()
{
	int16_t cursorX, cursorY;
	int16_t halfScreenWidth = tft.width() / 2;
	int16_t halfScreenHeight = tft.height() / 2;

	currentPage = POW;

	if (lastPage != currentPage)	// Clear display and redraw static elements of the page format:
	{
		DrawPageHeaderAndFooter();

		cursorY = tft.height() - 20;
		tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background

		tft.setTextDatum(BL_DATUM);
		tft.setTextSize(2);
		cursorX = halfScreenWidth + 2;
		tft.drawString("V", cursorX, cursorY);
		cursorX += tft.textWidth("V", 2) + 1;
		tft.setTextSize(1);
		tft.drawString("MCU", cursorX, cursorY);	// Subscript

		tft.setTextSize(2);
		cursorX = halfScreenWidth + 2;
		cursorY -= 20;
		tft.drawString("P", cursorX, cursorY);
		cursorX += tft.textWidth("V", 2) + 1;
		tft.setTextSize(1);
		tft.drawString("Bus", cursorX, cursorY);	// Subscript

		tft.setTextSize(2);
		cursorX = halfScreenWidth + 2;
		cursorY -= 20;
		tft.drawString("I", cursorX, cursorY);
		cursorX += tft.textWidth("V", 2) + 1;
		tft.setTextSize(1);
		tft.drawString("Bus", cursorX, cursorY);	// Subscript

		tft.setTextSize(2);
		cursorX = halfScreenWidth + 2;
		cursorY -= 20;
		tft.drawString("V", cursorX, cursorY);
		cursorX += tft.textWidth("V", 2) + 1;
		tft.setTextSize(1);
		tft.drawString("Load", cursorX, cursorY);	// Subscript

		tft.setTextSize(2);
		cursorX = halfScreenWidth + 2;
		cursorY -= 20;
		tft.drawString("V", cursorX, cursorY);
		cursorX += tft.textWidth("V", 2) + 1;
		tft.setTextSize(1);
		tft.drawString("Bus", cursorX, cursorY);	// Subscript

		lastPage = currentPage;
	}

	// Update dynamic displays:

	cursorY = tft.height() - 20;
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	tft.setTextSize(2);
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F  V", mccSensors.GetMCUVoltageReal());
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	cursorY -= 20;
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.0F mW", MCCStatus.mrsSensorPacket.INA219Power);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	cursorY -= 20;
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.1F mA", MCCStatus.mrsSensorPacket.INA219Current);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	cursorY -= 20;
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F  V", MCCStatus.mrsSensorPacket.INA219VLoad);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

	cursorY -= 20;
	tft.setTextDatum(BR_DATUM);
	sprintf(buf, "%5.2F  V", MCCStatus.mrsSensorPacket.INA219VBus);
	tft.drawString(buf, tft.width() - 2, cursorY);	// Right justified

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
		sprintf(buf, "IP: %s Ch: %d %s %d dBm", WiFi.localIP().toString(), WiFi.channel(), WiFi.SSID().c_str(), WiFi.RSSI());
		tft.drawString(buf, 2, halfScreenHeight + 40);

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, halfScreenHeight + 50);

		tft.setTextColor(TFT_LIGHTGREY);
		sprintf(buf, "UART0 %s", MCCStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 2, 30);

		tft.setTextDatum(CR_DATUM);
		sprintf(buf, "UART1 %s", MCCStatus.RC2x15AUARTStatus ? "OK" : "NO");
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

	// Update dynamic displays:
	tft.setTextColor(TFT_PINK, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "%s %s %5u ms", "CSSM Downlink", MCCStatus.ESPNOWStatus ? "OK" : "NO", MCCStatus.CSSMPacketReceiptInterval);
	tft.drawString(buf, tft.width() / 2, 40);

	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "%s %s", "CSSM Uplink ", MCCStatus.CSSMESPNOWLinkStatus ? "OK" : "NO");
	tft.drawString(buf, tft.width() / 2, 50);

	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "%s %5d", "Uplink retries  ", MCCStatus.SendRetries);
	tft.drawString(buf, tft.width() / 2, 60);

	sprintf(buf, "CSSM D/L time    %5u ms", MCCStatus.CSSMPacketReceiptInterval);
	tft.drawString(buf, tft.width() / 2, 80);

	//_PL(MCCStatus.CSSMPacketReceiptInterval)
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
		sprintf(buf, "UART1 %s", MCCStatus.RC2x15AUARTStatus ? "OK" : "NO");
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

void LocalDisplayClass::DrawSENPage()
{
	int16_t cursorX, cursorY;
	int16_t halfScreenWidth = tft.width() / 2;
	int16_t halfScreenHeight = tft.height() / 2;

	currentPage = SEN;

	if (lastPage != currentPage)	// Clear display and redraw static elements of the page format:
	{
		DrawPageHeaderAndFooter();

		cursorY = tft.height() - 20;
		tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background


		lastPage = currentPage;
	}

	// Update dynamic displays:

	cursorX = 2;
	cursorY = 30;
	tft.setTextDatum(CL_DATUM);
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	sprintf(buf, "Tatm %7.2f %cC", MCCStatus.mrsSensorPacket.BME280Temp, 0xF7);
	tft.drawString("Local BME680:", cursorX, cursorY);

	cursorX = 12;
	cursorY += 10;
	tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
	sprintf(buf, "Tatm %7.2f %cC", MCCStatus.mrsSensorPacket.BME280Temp, 0xF7);
	tft.drawString(buf, cursorX, cursorY);

	cursorY += 10;
	sprintf(buf, "RH   %7.2f %%", MCCStatus.mrsSensorPacket.BME280RH);
	tft.drawString(buf, cursorX, cursorY);

	cursorY += 10;
	sprintf(buf, "Pbar %7.2f hPa", MCCStatus.mrsSensorPacket.BME280Pbaro);
	tft.drawString(buf, cursorX, cursorY);

	cursorY += 10;
	sprintf(buf, "Alt  %7.0f m", MCCStatus.mrsSensorPacket.BME280Alt);
	tft.drawString(buf, cursorX, cursorY);

	cursorY += 10;
	sprintf(buf, "Gas  %7.2f ohm", MCCStatus.mrsSensorPacket.BME280Gas);
	tft.drawString(buf, cursorX, cursorY);

	cursorX = 2;
	cursorY += 20;
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	sprintf(buf, "VMCU %s", mccSensors.GetMCUVoltageString());
	tft.drawString(buf, cursorX, cursorY);

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

	tft.init();
	tft.setRotation(3);

	//Control(LocalDisplayClass::Commands::SYSPage);
	Control(LocalDisplayClass::Commands::DBGPage);
	SetDisplayBrightness(DefaultDisplayBrightness);

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
	case DBG:
		DrawDBGPage();
		break;
	case SEN:
		DrawSENPage();
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
	case SENPage:
		DrawSENPage();
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

void LocalDisplayClass::PrevPage(int /*value*/)
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

void LocalDisplayClass::NextPage(int /*value*/)
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

void LocalDisplayClass::SetDisplayBrightness(int brightness)
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
