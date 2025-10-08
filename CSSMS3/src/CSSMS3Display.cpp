/*	CSSMS3Display.cpp
*	CSSMS3Display - Base class for managing function and presenting data and graphics through the AMOLED
*	display integrated with the module MCU (LilyGO T-Display S3 )
*
*/

#include "CSSMS3Display.h"
#include "CSSMS3Controls.h"
#include "CSSMS3EnvSensors.h"
#include <I2CBus.h>
#include <WiFi.h>

/// <summary>
/// Builds a string representing time in hh:mm:ss format from a value supplied in ms
/// </summary>
/// <param name="msTime">
/// The time in milliseconds to convert to a time string
/// </param>
/// <param name="timeString">
/// Reference to the destination String
/// </param>
void CSSMS3Display::GetTimeString(uint64_t msTime, String* timeString)
{
	char buf[32];
	uint64_t allSeconds = msTime / 1000;
	int hours = allSeconds / 3600;
	int secondsRemaining = allSeconds % 3600;
	int minutes = secondsRemaining / 60;
	int seconds = secondsRemaining % 60;
	sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
	*timeString = buf;
	return;
}

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

void CSSMS3Display::DrawDashboard(int32_t xTC, int32_t yTC, bool showDriveData, bool showProximityData, bool showHDGBox, bool showCRSBox)
{
	// Control inputs line:
	int32_t cursorY = yTC;
	if (showDriveData)
	{
		tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
		tft.setTextDatum(TC_DATUM);
		sprintf(buf, "%s L%+06.1f%% R%+06.1f%% Vbat %4.1fV Imot %4.2fA",
			DriveModeHeadings[CSSMS3Status.cssmDrivePacket.DriveMode],
			CSSMS3Status.cssmDrivePacket.LThrottle,
			CSSMS3Status.cssmDrivePacket.RThrottle,
			CSSMS3Status.mcStatus.SupBatV,
			(CSSMS3Status.mcStatus.M1Current > CSSMS3Status.mcStatus.M2Current) ? CSSMS3Status.mcStatus.M1Current : CSSMS3Status.mcStatus.M2Current);
		tft.drawString(buf, xTC + 2, cursorY);

		// Basic motion data line (from MRS telemetry):
		cursorY += 10;
		tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		sprintf(buf, "GSpd (%+6.1f%%)%+6.1fmm/s wXY (%+6.1f%%)%+6.3frad/s",
			CSSMS3Status.cssmDrivePacket.SpeedSettingPct,
			CSSMS3Status.mcStatus.GroundSpeed,
			CSSMS3Status.cssmDrivePacket.OmegaXYSettingPct,
			CSSMS3Status.mcStatus.TurnRate);
		tft.drawString(buf, xTC + 2, cursorY);

	}

	if (showProximityData)
	{
		// Proximity sensor line (from MRS telemetry):
		cursorY = yTC + 20;
		sprintf(buf, "%s", "CLEAR");
		tft.setTextDatum(TC_DATUM);
		tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
		tft.drawString(buf, tft.width() / 2, cursorY, 2);
	}

	if (showHDGBox)
	{
		// Heading (HDG) box (lower left corner):
		tft.setTextDatum(TC_DATUM);
		tft.drawRect(1, tft.height() - 30, 33, 29, TFT_ORANGE);
		tft.setTextColor(0xd4c5, TFT_BLACK, true);
		sprintf(buf, "%03D", CSSMS3Status.cssmDrivePacket.HeadingSetting);
		tft.drawString(buf, 16, tft.height() - 27);
		tft.setTextColor(0xf5e8, TFT_BLACK, true);
		sprintf(buf, "%03D", (int)CSSMS3Status.mcStatus.Heading);
		tft.drawString(buf, 16, tft.height() - 12);
	}

	if (showCRSBox)
	{
		// Course (CRS) box (lower right corner):
		tft.setTextDatum(TC_DATUM);
		tft.drawRect(tft.width() - 34, tft.height() - 30, 33, 29, TFT_SKYBLUE);
		tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
		sprintf(buf, "%03D", CSSMS3Status.cssmDrivePacket.CourseSetting);
		tft.drawString(buf, tft.width() - 16, tft.height() - 27);
		tft.setTextColor(0x7fbe, TFT_BLACK, true);
		sprintf(buf, "%s", "TO");
		tft.drawString(buf, tft.width() - 16, tft.height() - 12);
	}
}

void CSSMS3Display::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);

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
		lastSystemPage = currentPage;
	}

	// Update dynamic displays:

	DrawDashboard(tft.width() / 2, tft.height() - 50);
	
	tft.setTextColor(TFT_PINK, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "MCC Downlink  %s", CSSMS3Status.ESPNOWStatus ? "OK" : "NO");
	tft.drawString(buf, tft.width() / 2, 50);
	sprintf(buf, "Downlink retries %5d", CSSMS3Status.SendRetries);
	tft.drawString(buf, tft.width() / 2, 60);


	int16_t cursorY = tft.height() / 2 + 10;
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "KP %4d %s", cssmS3Controls.GetKPRawADC(), cssmS3Controls.GetKPVoltageString());
	tft.drawString(buf, 2, cursorY);

	cursorY += 10;
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	sprintf(buf, "VMCU    %s", cssmS3Controls.GetMCUVoltageString());
	tft.drawString(buf, 2, cursorY);

	// Display odometer time from MRS MCC telemetry:
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	String str;
	GetTimeString(CSSMS3Status.mcStatus.OdometerTime, &str);
	sprintf(buf, "MC ODO dt %s", str);
	//sprintf(buf, "MC ODO dt:%8.1f s", (float)CSSMS3Status.mcStatus.OdometerTime / 1000.0f);
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

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 2, halfScreenHeight - 10);

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
		sprintf(buf, "WiFi Ch %d %s %s", WiFi.channel(), CSSMS3Status.WiFiStatus ? "SSID:" : "NO", WiFi.SSID().c_str());
		tft.drawString(buf, 2, halfScreenHeight + 20);

		sprintf(buf, "IP: %s %d dBm", WiFi.localIP().toString(), WiFi.RSSI());
		tft.drawString(buf, 2, halfScreenHeight + 30);

		// Draw footer menu:
		if (cssmS3Controls.CommsMenu != nullptr)
		{
			cssmS3Controls.CommsMenu->Draw();
		}

		lastPage = currentPage;
		lastSystemPage = currentPage;
	}

	// Update dynamic displays:
	DrawDashboard(tft.width() / 2, tft.height() - 50);

	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "%s %s %5d ms", "MCC Uplink   ", CSSMS3Status.ESPNOWStatus ? "OK" : "NO", CSSMS3Status.MCCPacketReceiptInterval);
	tft.drawString(buf, tft.width() / 2, 40);

	tft.setTextColor(TFT_PINK, TFT_BLACK, true);
	sprintf(buf, "%s %s", "MCC Downlink ", CSSMS3Status.ESPNOWStatus ? "OK" : "NO");
	tft.drawString(buf, tft.width() / 2, 50);

	sprintf(buf, "%s %5d", "Downlink retries", CSSMS3Status.SendRetries);
	tft.drawString(buf, tft.width() / 2, 60);


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
		lastSystemPage = currentPage;
	}

	// Update dynamic displays:
	//DrawDashboard(tft.width() / 2, tft.height() - 50, false);


}

void CSSMS3Display::DrawSENPage()
{
	int32_t halfScreenWidth = tft.width() / 2;
	int32_t halfScreenHeight = tft.height() / 2;
	String TempString;
	String PbaroString;
	String RHString;

	currentPage = SEN;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);


		// Draw footer menu:
		if (cssmS3Controls.MainMenu != nullptr)
		{
			cssmS3Controls.MainMenu->Draw();
		}

		lastPage = currentPage;
		lastSystemPage = currentPage;
	}

	// Update dynamic displays:
	DrawDashboard(tft.width() / 2, tft.height() - 50);

	tft.setTextSize(1);
	int32_t cursorX = 2;
	int32_t cursorY = 30;

	EnvSensors.BME280Data.GetTchipString(&TempString);
	EnvSensors.BME280Data.GetPbaroString(&PbaroString);
	EnvSensors.BME280Data.GetRHString(&RHString);
	tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "CSSM BME280 %s %s %s", TempString, RHString, PbaroString);
	tft.drawString(buf, cursorX, cursorY, 1);

	cursorY += 10;
	sprintf(buf, "MRS  BME680 %7.2f %cC  %6.2f %%RH  %6.2f hPa",
			CSSMS3Status.mrsSensorPacket.BME280Temp, 0xF7,
			CSSMS3Status.mrsSensorPacket.BME280RH,
			CSSMS3Status.mrsSensorPacket.BME280Pbaro);
	tft.drawString(buf, cursorX, cursorY, 1);

	cursorX += 80;
	cursorY += 10;
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	sprintf(buf, "Alt:%7.0f m  Gas:%7.2f ohm",
			CSSMS3Status.mrsSensorPacket.BME280Alt,
			CSSMS3Status.mrsSensorPacket.BME280Gas);
	tft.drawString(buf, cursorX, cursorY, 1);

}

void CSSMS3Display::DrawMRSPage()
{
	int32_t cursorX = 2, cursorY = 20;
	currentPage = MRS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);
		tft.setTextDatum(TL_DATUM);
		tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
		tft.drawString("LUPS Vbus", cursorX, cursorY);
		cursorX += 30;
		cursorY += 10;
		tft.drawString("Vload", cursorX, cursorY);
		cursorY += 10;
		tft.drawString("Current", cursorX, cursorY);
		cursorY += 10;
		tft.drawString("Power", cursorX, cursorY);

		cursorX = tft.width() / 2 + 15;
		cursorY = 20;
		tft.drawString("RUPS Vbus", cursorX, cursorY);
		cursorX += 30;
		cursorY += 10;
		tft.drawString("Vload", cursorX, cursorY);
		cursorY += 10;
		tft.drawString("Current", cursorX, cursorY);
		cursorY += 10;
		tft.drawString("Power", cursorX, cursorY);

		// Draw footer menu:
		if (cssmS3Controls.MainMenu != nullptr)
		{
			cssmS3Controls.MainMenu->Draw();
		}

		lastPage = currentPage;
		lastSystemPage = currentPage;
	}

	// Update dynamic displays:
	DrawDashboard(tft.width() / 2, tft.height() - 50);

	tft.setTextSize(1);
	tft.setTextDatum(TR_DATUM);
	tft.setTextColor(TFT_RED, TFT_BLACK, true);
	cursorX = tft.width() / 2 - 20;
	cursorY = 20;
	sprintf(buf, "%5.2F  V", CSSMS3Status.mrsSensorPacket.INA219VBus);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
	cursorY += 10;
	sprintf(buf, "%5.2F  V", CSSMS3Status.mrsSensorPacket.INA219VLoad);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
	cursorY += 10;
	sprintf(buf, "%5.1F mA", CSSMS3Status.mrsSensorPacket.INA219Current);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
	cursorY += 10;
	sprintf(buf, "%5.0F mW", CSSMS3Status.mrsSensorPacket.INA219Power);
	tft.drawString(buf, cursorX, cursorY);	// Right justified

	tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
	cursorX = tft.width() - 5;
	cursorY = 20;
	sprintf(buf, "%5.2F  V", CSSMS3Status.mrsSensorPacket.INA219VBus);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
	cursorY += 10;
	sprintf(buf, "%5.2F  V", CSSMS3Status.mrsSensorPacket.INA219VLoad);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
	cursorY += 10;
	sprintf(buf, "%5.1F mA", CSSMS3Status.mrsSensorPacket.INA219Current);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
	cursorY += 10;
	sprintf(buf, "%5.0F mW", CSSMS3Status.mrsSensorPacket.INA219Power);
	tft.drawString(buf, cursorX, cursorY);	// Right justified
}

void CSSMS3Display::DrawDRVPage()
{
	currentPage = DRV;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);

		LTrackBarGauge.DrawFrame();
		RTrackBarGauge.DrawFrame();
		MRSTrackBarGauge.DrawFrame();

		// Draw footer menu:
		if (cssmS3Controls.DRVPageMenu != nullptr)
		{
			cssmS3Controls.DRVPageMenu->Draw();
		}

		lastPage = currentPage;
	}

	// Update dynamic displays:

	DrawDashboard(tft.width() / 2, tft.height() - 50, false);

	int16_t cursorY = 15;
	tft.setTextDatum(TL_DATUM);
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
	sprintf(buf, "Mode %s", DriveModeHeadings[CSSMS3Status.cssmDrivePacket.DriveMode]);
	tft.drawString(buf, 2, cursorY);

	// Display odometry from MRS MCC telemetry:
	cursorY += 10;
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	tft.drawString("MC ODO UpLnk:", 2, cursorY);

	// Display throttle settings from CSSMS3 control packet:
	sprintf(buf, "L%+06.1f%% R%+06.1f%%", CSSMS3Status.cssmDrivePacket.LThrottle, CSSMS3Status.cssmDrivePacket.RThrottle);
	tft.setTextDatum(TC_DATUM);
	tft.drawString(buf, 210, cursorY);
	
	// Home plate symbol, resembling a delta, is character code 0x7F
	cursorY += 10;
	String str;
	GetTimeString(CSSMS3Status.mcStatus.OdometerTime, &str);
	sprintf(buf, "%ct:  %s", 0x7F, str);
	tft.setTextDatum(TL_DATUM);
	tft.drawString(buf, 12, cursorY);

	tft.setTextColor(TFT_DARKCYAN, TFT_BLACK, true);
	if (CSSMS3Status.mcStatus.Temp1 > 25.0f)
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
	}
	else if (CSSMS3Status.mcStatus.Temp1 > 30.0f)
	{
		tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
	}
	else if (CSSMS3Status.mcStatus.Temp1 > 40.0f)
	{
		tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
	}
	else if (CSSMS3Status.mcStatus.Temp1 > 50.0f)
	{
		tft.setTextColor(TFT_RED, TFT_BLACK, true);
	}
	sprintf(buf, "T1 %4.1f%c", CSSMS3Status.mcStatus.Temp1, 0xF7);
	tft.drawString(buf, 108, cursorY);

	cursorY += 10;
	sprintf(buf, "%cd:%8.3f m", 0x7F, CSSMS3Status.mcStatus.OdometerDist);
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	tft.drawString(buf, 12, cursorY);

	tft.setTextColor(TFT_DARKCYAN, TFT_BLACK, true);
	if (CSSMS3Status.mcStatus.Temp2 > 25.0f)
	{
		tft.setTextColor(TFT_GREEN, TFT_BLACK, true);
	}
	else if (CSSMS3Status.mcStatus.Temp2 > 30.0f)
	{
		tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);
	}
	else if (CSSMS3Status.mcStatus.Temp2 > 40.0f)
	{
		tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
	}
	else if (CSSMS3Status.mcStatus.Temp2 > 50.0f)
	{
		tft.setTextColor(TFT_RED, TFT_BLACK, true);
	}
	sprintf(buf, "T2 %4.1f%c", CSSMS3Status.mcStatus.Temp2, 0xF7);
	tft.drawString(buf, 108, cursorY);

	cursorY += 10;
	sprintf(buf, "Vbat: %5.1f V", CSSMS3Status.mcStatus.SupBatV);
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	tft.drawString(buf, 12, cursorY);

	// Display ground speed and turn rate from MRS MCC telemetry:
	int16_t cursorX = 12;
	cursorY = 65;
	tft.setTextDatum(TL_DATUM);
	tft.setTextColor(TFT_GOLD, TFT_BLACK, true);
	sprintf(buf, "GSpd:%+6.1f mm/s", CSSMS3Status.mcStatus.GroundSpeed);
	tft.drawString(buf, cursorX, cursorY);
	cursorY += 10;
	sprintf(buf, "Turn:%+6.3f rad/s", CSSMS3Status.mcStatus.TurnRate);
	tft.drawString(buf, cursorX, cursorY);

	cursorY = 96;
	tft.setTextDatum(CL_DATUM);
	GetTimeString(CSSMS3Status.mcStatus.Trip1Time, &str);
	sprintf(buf, "%ct:  %s", 0x7F, str);
	tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
	tft.drawString(buf, 60, cursorY);

	cursorY += 10;
	sprintf(buf, "%cd:%8.3f m", 0x7F, CSSMS3Status.mcStatus.Trip1Dist);
	tft.drawString(buf, 60, cursorY);

	cursorY = 124;
	GetTimeString(CSSMS3Status.mcStatus.Trip2Time, &str);
	sprintf(buf, "%ct:  %s", 0x7F, str);
	tft.drawString(buf, 60, cursorY);

	cursorY += 10;
	sprintf(buf, "%cd:%8.3f m", 0x7F, CSSMS3Status.mcStatus.Trip2Dist);
	tft.drawString(buf, 60, cursorY);

	LTrackBarGauge.Update(CSSMS3Status.mcStatus.M2Speed, abs(CSSMS3Status.mcStatus.M2Current));
	RTrackBarGauge.Update(CSSMS3Status.mcStatus.M1Speed, abs(CSSMS3Status.mcStatus.M1Current));
	MRSTrackBarGauge.Update(CSSMS3Status.mcStatus.GroundSpeed);
}

void CSSMS3Display::DrawHDGPage()
{
	currentPage = HDG;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);


		// Draw footer menu:
		if (cssmS3Controls.HDGPageMenu != nullptr)
		{
			cssmS3Controls.HDGPageMenu->Draw();
		}

		lastPage = currentPage;
	}

	// Update dynamic displays:

	DrawDashboard(tft.width() / 2, tft.height() - 50);


}

void CSSMS3Display::DrawWPTPage()
{
	currentPage = WPT;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);


		// Draw footer menu:
		if (cssmS3Controls.MainMenu != nullptr)
		{
			cssmS3Controls.MainMenu->Draw();
		}

		lastPage = currentPage;
	}

	// Update dynamic displays:

	DrawDashboard(tft.width() / 2, tft.height() - 50);


}

void CSSMS3Display::DrawSEQPage()
{
	currentPage = SEQ;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);


		// Draw footer menu:
		if (cssmS3Controls.MainMenu != nullptr)
		{
			cssmS3Controls.MainMenu->Draw();
		}

		lastPage = currentPage;
	}

	// Update dynamic displays:

	DrawDashboard(tft.width() / 2, tft.height() - 50);


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

	LTrackBarGauge.Init(&tft, 180, 76, BarGauge::BarGaugeLayoutTypes::PowerBarLeft);
	LTrackBarGauge.SetLabel("LTrk");
	LTrackBarGauge.SetLimits(-7500.0f, 7500.0f);
	LTrackBarGauge.SetPowerLimit(1.0f);

	MRSTrackBarGauge.Init(&tft, 210, 76);
	MRSTrackBarGauge.SetLimits(-600.0f, 600.0f);	// Indicates MRS ground speed (mm/s)
	MRSTrackBarGauge.SetLabel("MRS");

	RTrackBarGauge.Init(&tft, 240, 76, BarGauge::BarGaugeLayoutTypes::PowerBarRight);
	RTrackBarGauge.SetLabel("RTrk");
	RTrackBarGauge.SetLimits(-7500.0f, 7500.0f);
	RTrackBarGauge.SetPowerLimit(1.0f);

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
	case SEN:
		DrawSENPage();
		break;
	case MRS:
		DrawMRSPage();
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
	case SENPage:
		DrawSENPage();
		break;
	case MRSPage:
		DrawMRSPage();
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

CSSMS3Display::Pages CSSMS3Display::GetLastSystemsPage()
{
	return lastSystemPage;
}

void CSSMS3Display::ShowCurrentDriveModePage()
{
	switch (CSSMS3Status.cssmDrivePacket.DriveMode)
	{
	case CSSMDrivePacket::DriveModes::DRV:
		cssmS3Display.SetCurrentPage(Pages::DRV);
		break;
	case CSSMDrivePacket::DriveModes::HDG:
		cssmS3Display.SetCurrentPage(Pages::HDG);
		break;
	case CSSMDrivePacket::DriveModes::WPT:
		cssmS3Display.SetCurrentPage(Pages::WPT);
		break;
	case CSSMDrivePacket::DriveModes::SEQ:
		cssmS3Display.SetCurrentPage(Pages::SEQ);
		break;
	case CSSMDrivePacket::DriveModes::DRVLR:
		cssmS3Display.SetCurrentPage(Pages::DRV);
		break;
	case CSSMDrivePacket::DriveModes::DRVTw:
		cssmS3Display.SetCurrentPage(Pages::DRV);
		break;
	default:
		cssmS3Display.SetCurrentPage(Pages::DRV);
		break;
	}
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

void CSSMS3Display::PrevPage(int /*value*/)
{
	//TODO: Untested
	if (CSSMS3Status.SysDrvDisplayState)
	{
		if (currentPage <= CSSMS3Display::Pages::NONE)
		{
			currentPage = CSSMS3Display::Pages::DRV;
		}
		else
		{
			int page = currentPage;
			page--;
			currentPage = (CSSMS3Display::Pages)page;
		}
	}
	else
	{
		if (currentPage <= CSSMS3Display::Pages::SYS)
		{
			currentPage = CSSMS3Display::Pages::MRS;
		}
		else
		{
			int page = currentPage;
			page--;
			currentPage = (CSSMS3Display::Pages)page;
		}
	}
}

void CSSMS3Display::NextPage(int /*value*/)
{
	if (CSSMS3Status.SysDrvDisplayState)
	{
		if (currentPage >= CSSMS3Display::Pages::NONE)
		{
			currentPage = CSSMS3Display::Pages::DRV;
		}
		else
		{
			int page = currentPage;
			page++;
			currentPage = (CSSMS3Display::Pages)page;
		}
	}
	else
	{
		if (currentPage >= CSSMS3Display::Pages::MRS)
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
}

TFT_eSPI* CSSMS3Display::GetTFT()
{
	return &tft;
}

byte CSSMS3Display::GetDisplayBrightness()
{
	return Brightness;
}

void CSSMS3Display::SetDisplayBrightness(int brightness)
{
	Brightness = brightness;
	analogWrite(LCD_BL, Brightness);
}

void CSSMS3Display::AddDebugTextLine(String newLine)
{
	CSSMS3Status.AddDebugTextLine(newLine);
	RefreshPage(DBG);
}

void CSSMS3Display::ReportHeapStatus(int /*value*/)
{
	char buf[64];

	sprintf(buf, "Heap (F/T): %d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
	CSSMS3Status.AddDebugTextLine(buf);
	cssmS3Display.RefreshPage(CSSMS3Display::Pages::DBG);
}

void CSSMS3Display::ShowFontTableFixed(int /*value*/)
{
	cssmS3Display.ShowingFontTable = !cssmS3Display.ShowingFontTable;
	cssmS3Display.RefreshCurrentPage();
	//cssmS3Display.ShowFontTable(cssmS3Display.tft.width() / 2 + 10, 10);
}

void CSSMS3Display::ShowFontTable(int32_t xTC, int32_t yTC)
{
	yTC -= 20;	// Subtract starting row index x row height (2 * 10 = 20)
	tft.fillRect(xTC, yTC, 128, 160, TFT_BLACK);
	tft.setTextDatum(TL_DATUM);
	tft.setTextColor(TFT_GOLD, TFT_BLACK, true);
	for (byte i = 2; i < 16; ++i)
	{
		for (byte j = 0; j < 16; ++j)
		{
			tft.drawString(String((char)(i * 16 + j)), xTC + 8 * j, yTC + 10 * i);
		}
	}
}


CSSMS3Display cssmS3Display;
byte CSSMS3Display::Brightness = DefaultDisplayBrightness;
CSSMS3Display::Pages CSSMS3Display::currentPage = CSSMS3Display::Pages::SYS;

