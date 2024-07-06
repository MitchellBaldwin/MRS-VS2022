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

	OSBs[SoftOSBClass::OSBPositions::LOSB1] = new SoftOSBClass("NAV", SoftOSBClass::OSBPositions::LOSB1);
	OSBs[SoftOSBClass::OSBPositions::LOSB2] = new SoftOSBClass("COM", SoftOSBClass::OSBPositions::LOSB2);
	OSBs[SoftOSBClass::OSBPositions::LOSB3] = new SoftOSBClass("SYS", SoftOSBClass::OSBPositions::LOSB3);
	OSBs[SoftOSBClass::OSBPositions::LOSB4] = new SoftOSBClass("DBG", SoftOSBClass::OSBPositions::LOSB4);

	//OSBs[SoftOSBClass::OSBPositions::LOSB1]->SetOnPressHandler(&NavigationPageClass::Draw);
	//OSBs[SoftOSBClass::OSBPositions::LOSB2]->SetOnPressHandler(&CommunicationsPageClass::Draw);
	//OSBs[SoftOSBClass::OSBPositions::LOSB3]->SetOnPressHandler(&SystemPageClass::Draw);
	//OSBs[SoftOSBClass::OSBPositions::LOSB4]->SetOnPressHandler(&DebugPageClass::Draw);

	return true;
}

void MFCDPageClass::Draw()
{
	char buf[64];
	tft->fillScreen(TFT_BLACK);

	tft->setTextSize(1);
	tft->setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft->setTextDatum(TL_DATUM);
	sprintf(buf, "MRS RC NM v%d.%d", NMStatus.MajorVersion, NMStatus.MinorVersion);
	tft->drawString(buf, 2, 2, 1);

}

void MFCDPageClass::Update()
{
}

//MFCDPageClass MFCDPage;

bool NavigationPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Navigation";

	return true;
}

void NavigationPageClass::Draw()
{
	
}

bool CommunicationsPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Comms";

	return true;
}

void CommunicationsPageClass::Draw()
{

}

bool SystemPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "System";

	return true;
}

void SystemPageClass::Draw()
{
	MFCDPageClass::Draw();

	return;
}

bool DebugPageClass::Init(TFT_eSPI* parent_tft)
{
	MFCDPageClass::Init(parent_tft);
	Title = "Debug";

	return true;
}

void DebugPageClass::Draw()
{

}

NavigationPageClass NavigationPage;
CommunicationsPageClass CommunicationsPage;
SystemPageClass SystemPage;
DebugPageClass DebugPage;

