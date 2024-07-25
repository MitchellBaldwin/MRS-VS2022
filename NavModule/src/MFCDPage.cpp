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
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB1, "NAV", NMCommands::Commands::NAVPage, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB2, "COM", NMCommands::Commands::COMPage, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB3, "SYS", NMCommands::Commands::SYSPage, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.LOSBs.InitOSB(OSBSet::OSBIDs::OSB4, "DBG", NMCommands::Commands::DBGPage, SoftOSBClass::OSBCaps::NoCaps);

	return true;
}

void MFCDPageClass::Activate(bool reinitialize)
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
	for (byte osb = OSBSet::OSBIDs::OSB1; osb <= NMControls.LOSBs.OSBCount; ++osb)
	{
		DrawLOSB((OSBSet::OSBIDs)osb);
	}
	
	// Draw page specific (right side) OSBs:
	for (byte osb = OSBSet::OSBIDs::OSB1; osb <= NMControls.ROSBs.OSBCount; ++osb)
	{
		DrawROSB((OSBSet::OSBIDs)osb);
	}
}

void MFCDPageClass::Update()
{
	UpdateNavRotaries();
	UpdateTopRoraties();

}

void MFCDPageClass::UpdateNavRotaries()
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
}

void MFCDPageClass::UpdateTopRoraties()
{
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

void MFCDPageClass::DrawLOSB(OSBSet::OSBIDs osb)
{
	// Draw Fixed (left side) OSBs:
	tft->setTextDatum(CL_DATUM);
	tft->setTextColor(TFT_GREENYELLOW);
	tft->drawString(NMControls.LOSBs.OSBInfo[osb]->LabelText, 2, 50 + 70 * (osb - 1), 2);
}

void MFCDPageClass::DrawROSB(OSBSet::OSBIDs osb)
{
	// Draw page specific (right side) OSB:
	SoftOSBClass* osbInfo = NMControls.ROSBs.OSBInfo[osb];
	int osbY = 50 + 70 * (osb - 1);
	tft->setTextColor(TFT_GREENYELLOW);
	tft->setTextDatum(CR_DATUM);
	tft->drawString(osbInfo->LabelText, tft->width() - 2, osbY, 2);
	if (osbInfo->State & SoftOSBClass::States::Boxed)
	{
		tft->drawRect(tft->width() - tft->textWidth(osbInfo->LabelText, 2) - 4, osbY - 8, tft->textWidth(osbInfo->LabelText, 2) + 2, 16, TFT_CYAN);
	}
	else
	{
		//TODO: Consider clearing OSB display area prior to drawing anything and as opposed to erasing the box graphics by 
		//overwriting in the background color
		tft->drawRect(tft->width() - tft->textWidth(osbInfo->LabelText, 2) - 4, osbY - 8, tft->textWidth(osbInfo->LabelText, 2) + 2, 16, TFT_BLACK);
	}
}

bool NavigationPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Navigation";

	return true;
}

void NavigationPageClass::Activate(bool reinitialize)
{
	// Set up right OSB array for the SYS page:
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "MAP", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::Boxable);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "WPT", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "SEQ", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "NOP", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);

	MFCDPageClass::Activate(reinitialize);

}

bool CommunicationsPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Comms";

	return true;
}

void CommunicationsPageClass::Activate(bool reinitialize)
{
	// Set up right OSB array for the SYS page:
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "UART", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "ENOW", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "WiFi", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "I2C", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);

	MFCDPageClass::Activate(reinitialize);

}

bool SystemPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "System";

	return true;
}

void SystemPageClass::Activate(bool reinitialize)
{
	// Set up right OSB array for the SYS page:
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "POW", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "CSS", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "NOP", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "NOP", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);

	MFCDPageClass::Activate(reinitialize);	// Note: Activate() draws all OSBs

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

void DebugPageClass::Activate(bool reinitialize)
{
	if (reinitialize)
	{
		TestingFont = false;
		
		// Set up right OSB array for the SYS page:
		NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "FNT", NMCommands::Commands::TestFont, SoftOSBClass::OSBCaps::Boxable);
		NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "NOP", NMCommands::Commands::NoCommand, SoftOSBClass::OSBCaps::NoCaps);
		NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "CHP", NMCommands::Commands::CheckFreeHeap, SoftOSBClass::OSBCaps::NoCaps);
		NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "CLR", NMCommands::Commands::ClearDebugText, SoftOSBClass::OSBCaps::NoCaps);

	}

	MFCDPageClass::Activate(reinitialize);

	if (TestingFont)
	{
		TestFont();
	}
	else
	{
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

		tft->setTextColor(TFT_ORANGE, TFT_BLACK, true);
		sprintf(buf, "Heap  free: %d", ESP.getFreeHeap());
		tft->drawString(buf, 40, 100, 1);
		sprintf(buf, "     total: %d", ESP.getHeapSize());
		tft->drawString(buf, 40, 110, 1);
		sprintf(buf, "Prog  used: %d", ESP.getSketchSize());
		tft->drawString(buf, 40, 120, 1);
		sprintf(buf, "      free: %d", ESP.getFreeSketchSpace());
		tft->drawString(buf, 40, 130, 1);
		if (ESP.getPsramSize() > 0)
		{
			sprintf(buf, "PSRAM: %d/%d", ESP.getFreePsram(), ESP.getPsramSize());
		}
		else
		{
			sprintf(buf, "No PSRAM");
		}
		tft->drawString(buf, 40, 140, 1);

		tft->setTextColor(TFT_SKYBLUE, TFT_BLACK, true);
		sprintf(buf, "SSID: %s %d dBm", WiFi.SSID().c_str(), WiFi.RSSI());
		tft->drawString(buf, 40, 160, 1);
		sprintf(buf, "IP: %s", WiFi.localIP().toString());
		tft->drawString(buf, 40, 170, 1);

		for (int i = 0; i < MAX_TEXT_LINES; ++i)
		{
			tft->drawString(NMStatus.debugTextLines[i].c_str(), 40, 180 + i * 10);
		}

	}

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
		if (NMControls.ROSBs.OSBInfo[OSBSet::OSBIDs::OSB1]->State & SoftOSBClass::States::Boxed)
		{
			TestingFont = true;
		}
		else
		{
			TestingFont = false;
		}
		Activate(false);
		break;
	case NMCommands::ClearDebugText:
		NMStatus.ClearDebugText();
		Activate(false);
		break;
	case NMCommands::CheckFreeHeap:
		sprintf(buf, "Heap  free: %d", ESP.getFreeHeap());
		NMStatus.AddDebugTextLine(buf);
		Activate(false);
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
			tft->drawString(String((char)(i * 16 + j)), 56 + 8 * j, 40 + 10 * (i - 1));
		}
	}
}

bool NonePageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "";

	return true;
}

void NonePageClass::Activate(bool reinitialize)
{
	MFCDPageClass::Activate(reinitialize);

}

NavigationPageClass NavigationPage;
CommunicationsPageClass CommunicationsPage;
SystemPageClass SystemPage;
DebugPageClass DebugPage;
NonePageClass NonePage;


