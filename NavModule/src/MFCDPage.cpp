/*	MFCDPage.h
*	MFCDPageClass - Base class implementing the concept of a single Page of a Multifunction Color Display (MFCD)
*
*	Mitchell Baldwin copyright 2024
*
*/

#include "MFCDPage.h"
#include "NMStatus.h"
#include "NMControls.h"

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
}

//void MFCDPageClass::Control(SoftOSBClass::OSBPositions position)
//{
//}
//
//NMControlsClass::Commands MFCDPageClass::GetOBSCommand(SoftOSBClass::OSBPositions position)
//{
//	//return OSBs[position]->Command;
//	return NMControlsClass::Commands::NoCommand;
//}

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
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB1, "POW");
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB2, "CSS");
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB3, "NOP");
	NMControls.ROSBs.InitOSB(OSBSet::OSBIDs::OSB4, "NOP");

	MFCDPageClass::Activate();

	tft->setTextColor(TFT_GOLD, TFT_BLACK, false);
	tft->setTextDatum(CC_DATUM);
	tft->drawString("Testing...", tft->width()/2, tft->height()/2, 2);
}

bool DebugPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Debug";

	return true;
}

void DebugPageClass::Activate()
{
	MFCDPageClass::Activate();

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


