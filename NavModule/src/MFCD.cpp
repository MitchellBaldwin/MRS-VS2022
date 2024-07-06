/*	MFCDClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*
*	Mitchell Baldwin copyright 2023-2024
*
*/

#include "MFCD.h"

bool MFCDClass::Init()
{
	//tft = new TFT_eSPI();
	
	Pages[PageIDs::NAV] = &NavigationPage;
	Pages[PageIDs::COM] = &CommunicationsPage;
	Pages[PageIDs::SYS] = &SystemPage;
	Pages[PageIDs::DBG] = &DebugPage;
	Pages[PageIDs::NONE] = nullptr;

	NavigationPage.Init(tft);
	CommunicationsPage.Init(tft);
	SystemPage.Init(tft);
	DebugPage.Init(tft);
	//Pages[PageIDs::NONE]->Init(tft);

	currentPage = Pages[PageIDs::SYS];
	lastPage = Pages[PageIDs::NONE];

	//currentPage->Draw();
	//char buf[64];
	//tft->fillScreen(TFT_BLACK);

	//tft->setTextSize(1);
	//tft->setTextColor(TFT_BLUE, TFT_BLACK, false);
	//tft->setTextDatum(TL_DATUM);
	//sprintf(buf, "MRS RC NM v%d.%d", NMStatus.MajorVersion, NMStatus.MinorVersion);
	//tft->drawString(buf, 2, 2, 1);

}

void MFCDClass::Update()
{
	currentPage->Update();
}

void MFCDClass::Control()
{
}

void MFCDClass::ActivatePage(PageIDs page)
{
	lastPage = currentPage;
	currentPage = Pages[page];

	currentPage->Draw();

}


MFCDClass MFCD;

