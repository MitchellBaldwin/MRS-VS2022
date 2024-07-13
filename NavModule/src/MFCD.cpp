/*	MFCDClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*
*	Mitchell Baldwin copyright 2023-2024
*
*/

#include "MFCD.h"
#include "DEBUG Macros.h"

bool MFCDClass::Init()
{
	pinMode(MFCDLEDPin, OUTPUT);
	SetBrightness(Brightness);

	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	Pages[PageIDs::NAV] = &NavigationPage;
	Pages[PageIDs::COM] = &CommunicationsPage;
	Pages[PageIDs::SYS] = &SystemPage;
	Pages[PageIDs::DBG] = &DebugPage;
	Pages[PageIDs::NONE] = &NonePage;

	NavigationPage.Init(&tft);
	CommunicationsPage.Init(&tft);
	SystemPage.Init(&tft);
	DebugPage.Init(&tft);
	NonePage.Init(&tft);

	currentPage = Pages[PageIDs::SYS];
	lastPage = Pages[PageIDs::NONE];

	currentPage->Activate();
	
	return true;
}

void MFCDClass::Update()
{
	OSBSet::OSBIDs osb = NMControls.LOSBs.NewOSBPressed();
	
	// If one of the buttons from the left OSB array was pressed, get the its associated command and execute accordingly:
	if (osb != OSBSet::OSBIDs::NoOsb)
	{
		switch (NMControls.LOSBs.OSBInfo[osb]->Command)
		{
		case NMCommands::NAVPage:
			ActivatePage(PageIDs::NAV);
			break;
		case NMCommands::COMPage:
			ActivatePage(PageIDs::COM);
			break;
		case NMCommands::SYSPage:
			ActivatePage(PageIDs::SYS);
			break;
		case NMCommands::DBGPage:
			ActivatePage(PageIDs::DBG);
			break;
		default:
			currentPage->Update();
			break;
		}
	}
	
	osb = NMControls.ROSBs.NewOSBPressed();
	// If one of the buttons from the right OSB array was pressed, get the its associated command and execute accordingly:
	if (osb != OSBSet::OSBIDs::NoOsb)
	{

	}
	
}

void MFCDClass::Control()
{
}

void MFCDClass::ActivatePage(PageIDs page)
{
	lastPage = currentPage;
	currentPage = Pages[page];

	// Should instead call currentPage->Activate()?
	currentPage->Activate();

}

void MFCDClass::SetBrightness(byte brightness)
{
	NMControls.SetLocalDisplayBrightness(brightness);

	//TODO: Try using ledc library
	analogWrite(MFCDLEDPin, Brightness);
}


MFCDClass MFCD;

