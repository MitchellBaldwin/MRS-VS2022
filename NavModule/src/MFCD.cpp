/*	MFCDClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*
*	Mitchell Baldwin copyright 2023-2024
*
*/

#include "MFCD.h"
#include "NMControls.h"
#include "DEBUG Macros.h"

bool MFCDClass::Init()
{
	//tft = new TFT_eSPI();

	pinMode(MFCDLEDPin, OUTPUT);
	SetBrightness(Brightness);

	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	Pages[PageIDs::NAV] = &NavigationPage;
	Pages[PageIDs::COM] = &CommunicationsPage;
	Pages[PageIDs::SYS] = &SystemPage;
	Pages[PageIDs::DBG] = &DebugPage;
	Pages[PageIDs::NONE] = nullptr;

	NavigationPage.Init(&tft);
	CommunicationsPage.Init(&tft);
	SystemPage.Init(&tft);
	DebugPage.Init(&tft);
	//Pages[PageIDs::NONE]->Init(tft);

	currentPage = Pages[PageIDs::SYS];
	lastPage = Pages[PageIDs::NONE];

	currentPage->Draw();
	
	//char buf[64];
	//tft.fillScreen(TFT_BLACK);

	//tft.setTextSize(1);
	//tft.setTextColor(TFT_BLUE, TFT_BLACK, false);
	//tft.setTextDatum(TL_DATUM);
	//sprintf(buf, "MRS RC NM v%d.%d", NMStatus.MajorVersion, NMStatus.MinorVersion);
	//tft.drawString(buf, 2, 2, 1);

	return true;
}

void MFCDClass::Update()
{
	// Map OSBArrayClass::OSBs to SoftOSBClass::Positions:
	OSBArrayClass::OSBs osb = NMControls.NewLOSBKeyWasPressed();
	
	if (osb != OSBArrayClass::OSBs::NoOsb)
	{
		ActivatePage(PageIDs::NAV);
		_PL(osb)
		//osb = osb - OSBArrayClass::OSBs::OSB1;
		switch (currentPage->GetOBSCommand((SoftOSBClass::OSBPositions)osb))
		{
		case NMControlsClass::Commands::NAVPage:
			ActivatePage(PageIDs::NAV);
			break;
		default:
			break;
		}

	}
	
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

void MFCDClass::SetBrightness(byte brightness)
{
	NMControls.SetLocalDisplayBrightness(brightness);

	//TODO: Try using ledc library
	analogWrite(MFCDLEDPin, Brightness);
}


MFCDClass MFCD;

