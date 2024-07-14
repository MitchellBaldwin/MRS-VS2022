/*	MFCDPage.h
*	MFCDPageClass - Base class implementing the concept of a single Page of a Multifunction Color Display (MFCD)
*
*	Mitchell Baldwin copyright 2024
*
*/

#include "MFCDPage.h"
#include "NMStatus.h"
#include "NMControls.h"

#include <WiFi.h>

MFCDPageClass::MFCDPageClass()
{
}

MFCDPageClass::MFCDPageClass(TFT_eSPI* parent_tft)
{
	tft = parent_tft;
}

bool MFCDPageClass::Init(TFT_eSPI* parent_tft)
{
	tft = parent_tft;

	// Set up left OSB array (which stays constant for all, or at least most, pages):
	// [Move to overrides of MFCDPageClass::Activate() if in the future the functions of the left OSBs become page-specific]
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB1, "NAV", NMCommands::Commands::NAVPage);
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB2, "COM", NMCommands::Commands::COMPage);
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB3, "SYS", NMCommands::Commands::SYSPage);
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB4, "DBG", NMCommands::Commands::DBGPage);

	return true;
}

void MFCDPageClass::Activate()
{
	tft->fillScreen(TFT_BLACK);

	// Draw page header and footer:
	tft->setTextSize(1);
	tft->setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft->setTextDatum(TL_DATUM);
	sprintf(buf, "MRS RC NM v%d.%d", NMStatus.MajorVersion, NMStatus.MinorVersion);
	tft->drawString(buf, 2, 2, 1);
	tft->setTextDatum(TR_DATUM);
	tft->drawString(Title, tft->width(), 2);

	// Draw Fixed (left side) OSBs:
	tft->setTextDatum(CL_DATUM);
	tft->setTextColor(TFT_GREENYELLOW);
	for (byte osb = OSBSet::OSBIDs::OSB1; osb <= NMControls.LOSBs.OSBCount; ++osb)
	{
		tft->drawString(NMControls.LOSBs.OSBInfo[osb]->LabelText, 2, 50 + 70 * (osb - 1), 2);
	}
	
	// Draw page specific (right side) OSBs:
	tft->setTextDatum(CR_DATUM);
	for (byte osb = OSBSet::OSBIDs::OSB1; osb <= NMControls.ROSBs.OSBCount; ++osb)
	{
		tft->drawString(NMControls.ROSBs.OSBInfo[osb]->LabelText, tft->width() - 2, 50 + 70 * (osb - 1), 2);
	}
}

void MFCDPageClass::Update()
{
	sprintf(buf, "HDG: %03D", NMControls.HDGSetting);
	tft->setTextSize(1);
	tft->setTextColor(TFT_ORANGE, TFT_BLACK, true);
	tft->setTextDatum(BR_DATUM);
	tft->drawString(buf, tft->width() - 1, tft->height() - 1);
	if (NMControls.HDGSelected)
	{
		tft->drawRect(tft->width() - tft->textWidth(buf) - 2, tft->height() - 11, tft->textWidth(buf) + 2, 11, TFT_CYAN);
	}
	else
	{
		tft->drawRect(tft->width() - tft->textWidth(buf) - 2, tft->height() - 11, tft->textWidth(buf) + 2, 11, TFT_BLACK);
	}

	sprintf(buf, "CRS: %03D", NMControls.CRSSetting);
	tft->setTextSize(1);
	tft->setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft->setTextDatum(BL_DATUM);
	tft->drawString(buf, 1, tft->height() - 1);
	if (NMControls.CRSSelected)
	{
		tft->drawRect(0, tft->height() - 11, tft->textWidth(buf) + 2, 11, TFT_GOLD);
	}
	else
	{
		tft->drawRect(0, tft->height() - 11, tft->textWidth(buf) + 2, 11, TFT_BLACK);
	}

	sprintf(buf, "BRT: %03D", NMControls.BRTSetting);
	tft->setTextSize(1);
	tft->setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft->setTextDatum(TL_DATUM);
	tft->drawString(buf, 40, 16);

	sprintf(buf, "TRR: %03D", NMControls.TRRSetting);
	tft->setTextSize(1);
	tft->setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft->setTextDatum(TR_DATUM);
	tft->drawString(buf, tft->width() - 40, 16);

}

void MFCDPageClass::Control(NMCommands::Commands command)
{
}

bool NavigationPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Navigation";

	return true;
}

void NavigationPageClass::Activate()
{
	MFCDPageClass::Activate();

}

bool CommunicationsPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Comms";

	return true;
}

void CommunicationsPageClass::Activate()
{
	MFCDPageClass::Activate();

}

bool SystemPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "System";

	return true;
}

void SystemPageClass::Activate()
{
	// Set up right OSB array for the SYS page:
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "POW", NMCommands::Commands::NoCommand);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "CSS", NMCommands::Commands::NoCommand);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "NOP", NMCommands::Commands::NoCommand);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "NOP", NMCommands::Commands::NoCommand);

	MFCDPageClass::Activate();	// Note: Activate() draws all OSBs

	tft->setTextSize(1);

	//tft->setTextColor(TFT_PINK);
	//tft->setTextDatum(TL_DATUM);
	//tft->drawString(ComModeHeadings[NMStatus.ComMode], 40, 50);

	tft->setTextDatum(TL_DATUM);
	tft->setTextColor(TFT_LIGHTGREY);
	sprintf(buf, "UART0 %s", NMStatus.UART0Status ? "OK" : "NO");
	tft->drawString(buf, 40, 60);

	tft->setTextDatum(TR_DATUM);
	sprintf(buf, "UART2 %s", NMStatus.UART2Status ? "OK" : "NO");
	tft->drawString(buf, tft->width() - 40, 60);

	tft->setTextDatum(TL_DATUM);
	tft->setTextColor(TFT_ORANGE);
	sprintf(buf, "WiFi %s", NMStatus.WiFiStatus ? "OK" : "NO");
	tft->drawString(buf, 40, 70);

	tft->setTextDatum(TR_DATUM);
	sprintf(buf, "ESP-NOW %s", NMStatus.ESPNOWStatus ? "OK" : "NO");
	tft->drawString(buf, tft->width() - 40, 70);

	tft->setTextDatum(TL_DATUM);
	tft->setTextColor(TFT_GREENYELLOW);
	uint8_t mac[6];
	WiFi.macAddress(mac);
	sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	tft->drawString(buf, 40, 80);

	tft->setTextColor(TFT_GREEN);
	sprintf(buf, "IP: %s", WiFi.localIP().toString());
	tft->drawString(buf, 40, 90);

	tft->setTextColor(TFT_CYAN);
	tft->drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 40, 100);

}

void SystemPageClass::Update()
{
	tft->setCursor(100, tft->height() / 2);
	tft->setTextSize(2);
	tft->setTextColor(TFT_YELLOW);
	tft->setTextDatum(CL_DATUM);
	tft->print("5.00 V");

	MFCDPageClass::Update();
	
	// Test OSB arrays:
	tft->setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft->setTextDatum(CL_DATUM);
	sprintf(buf, "LOSB: %04D", NMControls.LOSBs.OSBADCReading);
	tft->drawString(buf, 2, tft->height() - 24);
	sprintf(buf, "ROSB: %04D", NMControls.ROSBs.OSBADCReading);
	tft->drawString(buf, tft->width() - tft->textWidth(buf), tft->height() - 24);
}

bool DebugPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Debug";

	return true;
}

void DebugPageClass::Activate()
{
	// Set up right OSB array for the SYS page:
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "FNT", NMCommands::Commands::TestFont);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "NOP", NMCommands::Commands::NoCommand);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "NOP", NMCommands::Commands::NoCommand);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "NOP", NMCommands::Commands::NoCommand);

	MFCDPageClass::Activate();

	tft->setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft->setTextDatum(TL_DATUM);

	sprintf(buf, "%s %d core", ESP.getChipModel(), ESP.getChipCores());
	tft->drawString(buf, 40, 50, 1);
	sprintf(buf, "CPU v%d %d MHz", ESP.getChipRevision(), ESP.getCpuFreqMHz());
	tft->drawString(buf, 40, 60, 1);

	uint8_t mac[6];
	esp_efuse_mac_get_default(mac);		// Returns the same MAC as does WiFi.madAddress(uint8_t*)
	sprintf(buf, "efuse MAC %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	tft->drawString(buf, 40, 70, 1);
	WiFi.macAddress(mac);
	sprintf(buf, "WiFi  MAC %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	tft->drawString(buf, 40, 80, 1);

	sprintf(buf, "Heap (free/total) %d / %d", ESP.getFreeHeap(), ESP.getHeapSize());
	tft->drawString(buf, 40, 100, 1);
	sprintf(buf, "Prog (used/free) %d / %d", ESP.getSketchSize(), ESP.getFreeSketchSpace());
	tft->drawString(buf, 40, 110, 1);
	if (ESP.getPsramSize() > 0)
	{
		sprintf(buf, "PSRAM: %d/%d", ESP.getFreePsram(), ESP.getPsramSize());
	}
	else
	{
		sprintf(buf, "No PSRAM");
	}
	tft->drawString(buf, 40, 120, 1);

	sprintf(buf, "SSID: %s %d dBm", WiFi.SSID().c_str(), WiFi.RSSI());
	tft->drawString(buf, 40, 140, 1);
	sprintf(buf, "IP: %s", WiFi.localIP().toString());
	tft->drawString(buf, 40, 150, 1);

}

void DebugPageClass::Update()
{

	MFCDPageClass::Update();

	// Test OSB arrays:
	tft->setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft->setTextDatum(CL_DATUM);
	sprintf(buf, "LOSB: %04D", NMControls.LOSBs.OSBADCReading);
	tft->drawString(buf, 2, tft->height() - 24);
	sprintf(buf, "ROSB: %04D", NMControls.ROSBs.OSBADCReading);
	tft->drawString(buf, tft->width() - tft->textWidth(buf), tft->height() - 24);

}

void DebugPageClass::Control(NMCommands::Commands command)
{
	// Handle page-specific commands:
	switch (command)
	{
	case NMCommands::TestFont:
		TestFont();
		break;
	default:
		break;
	}
}

void DebugPageClass::TestFont()
{
	tft->setTextDatum(TL_DATUM);
	tft->setTextColor(TFT_GOLD, TFT_BLACK, true);
	for (byte i = 0; i < 16; ++i)
	{
		for (byte j = 0; j < 16; ++j)
		{
			tft->drawString(String((char)(i * 16 + j)), 56 + 8 * j, tft->height() / 2 + 10 * (i - 1));
		}
	}
}

bool NonePageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "";

	return true;
}

void NonePageClass::Activate()
{
	MFCDPageClass::Activate();

}
NavigationPageClass NavigationPage;
CommunicationsPageClass CommunicationsPage;
SystemPageClass SystemPage;
DebugPageClass DebugPage;
NonePageClass NonePage;


