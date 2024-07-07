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

	OSBs[SoftOSBClass::OSBPositions::LOSB1] = new SoftOSBClass("NAV", SoftOSBClass::OSBPositions::LOSB1, NMControlsClass::Commands::NAVPage);
	OSBs[SoftOSBClass::OSBPositions::LOSB2] = new SoftOSBClass("COM", SoftOSBClass::OSBPositions::LOSB2, NMControlsClass::Commands::COMPage);
	OSBs[SoftOSBClass::OSBPositions::LOSB3] = new SoftOSBClass("SYS", SoftOSBClass::OSBPositions::LOSB3, NMControlsClass::Commands::SYSPage);
	OSBs[SoftOSBClass::OSBPositions::LOSB4] = new SoftOSBClass("DBG", SoftOSBClass::OSBPositions::LOSB4, NMControlsClass::Commands::DBGPage);

	//OSBs[SoftOSBClass::OSBPositions::ROSB1] = new SoftOSBClass("POW", SoftOSBClass::OSBPositions::ROSB1);
	//OSBs[SoftOSBClass::OSBPositions::ROSB2] = new SoftOSBClass("CSS", SoftOSBClass::OSBPositions::ROSB2);
	//OSBs[SoftOSBClass::OSBPositions::ROSB3] = new SoftOSBClass("MOD", SoftOSBClass::OSBPositions::ROSB3);
	//OSBs[SoftOSBClass::OSBPositions::ROSB4] = new SoftOSBClass("ORD", SoftOSBClass::OSBPositions::ROSB4);


	return true;
}

void MFCDPageClass::Draw()
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
	for (byte i = 0; i <= SoftOSBClass::OSBPositions::LOSB4; ++i)
	{
		if (OSBs[i] != nullptr)
		{
			tft->setTextDatum(CL_DATUM);
			tft->setTextColor(TFT_GREENYELLOW);

			tft->drawString(OSBs[i]->LabelText, 2, 50 + 70 * i, 2);

		}
	}

	// Draw page specific (right side) OSBs:
	for (byte i = SoftOSBClass::OSBPositions::ROSB1; i <= SoftOSBClass::OSBPositions::ROSB4; ++i)
	{
		tft->setTextDatum(CR_DATUM);
		tft->setTextColor(TFT_PINK);

		tft->drawString(OSBs[i]->LabelText, tft->width() - 1, 50 + 70 * (i - SoftOSBClass::OSBPositions::ROSB1), 2);
	}
}

void MFCDPageClass::Update()
{
}

void MFCDPageClass::Control(SoftOSBClass::OSBPositions position)
{
}

NMControlsClass::Commands MFCDPageClass::GetOBSCommand(SoftOSBClass::OSBPositions position)
{
	return OSBs[position]->Command;
}

//MFCDPageClass MFCDPage;

bool NavigationPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Navigation";
	//OSBs[SoftOSBClass::OSBPositions::LOSB1]->SetOnPressHandler(&Draw);

	return true;
}

void NavigationPageClass::Draw()
{
	MFCDPageClass::Draw();

}

bool CommunicationsPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Comms";
	//OSBs[SoftOSBClass::OSBPositions::LOSB2]->SetOnPressHandler(&CommunicationsPageClass::Draw);

	return true;
}

void CommunicationsPageClass::Draw()
{
	MFCDPageClass::Draw();

}

bool SystemPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "System";

	OSBs[SoftOSBClass::OSBPositions::ROSB1] = new SoftOSBClass("POW", SoftOSBClass::OSBPositions::ROSB1);
	OSBs[SoftOSBClass::OSBPositions::ROSB2] = new SoftOSBClass("CSS", SoftOSBClass::OSBPositions::ROSB2);
	OSBs[SoftOSBClass::OSBPositions::ROSB3] = new SoftOSBClass("MOD", SoftOSBClass::OSBPositions::ROSB3);
	OSBs[SoftOSBClass::OSBPositions::ROSB4] = new SoftOSBClass("ORD", SoftOSBClass::OSBPositions::ROSB4);

	return true;
}

void SystemPageClass::Draw()
{
	MFCDPageClass::Draw();

	tft->setTextColor(TFT_GOLD, TFT_BLACK, false);
	tft->setTextDatum(CC_DATUM);
	tft->drawString("Testing...", tft->width()/2, tft->height()/2, 2);
}

bool DebugPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Debug";
	//OSBs[SoftOSBClass::OSBPositions::LOSB4]->SetOnPressHandler(&DebugPageClass::Draw);

	return true;
}

void DebugPageClass::Draw()
{
	MFCDPageClass::Draw();

}

NavigationPageClass NavigationPage;
CommunicationsPageClass CommunicationsPage;
SystemPageClass SystemPage;
DebugPageClass DebugPage;

