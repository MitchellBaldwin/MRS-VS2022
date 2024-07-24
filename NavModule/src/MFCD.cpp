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

	currentPage->Activate(true);
	
	return true;
}

void MFCDClass::Update()
{
	Brightness = NMControls.BRTSetting;
	SetBrightness(Brightness);
	
	if (NMControls.BRTSelected)
	{
		NMControls.ToggleBRTSelected();

		if (Brightness >= 255)
		{
			SetBrightness(0);
		}
		else if (Brightness > 191)
		{
			SetBrightness(255);
		}
		else if (Brightness > 127)
		{
			SetBrightness(192);
		}
		else if (Brightness > 63)
		{
			SetBrightness(128);
		}
		else if (Brightness >= 0)
		{
			SetBrightness(64);
		}
	}

	currentPage->Update();

}

void MFCDClass::Control(NMCommands::Commands command)
{
	OSBSet::OSBIDs losb = NMControls.LOSBs.NewOSBPressed();
	OSBSet::OSBIDs rosb = NMControls.ROSBs.NewOSBPressed();
	
	// If one of the buttons from the left OSB array was pressed, get the its associated command and execute accordingly:
	if (losb != OSBSet::OSBIDs::NoOsb)
	{
		command = NMControls.LOSBs.GetOSBPressCommand(losb);
	}
	else if (rosb != OSBSet::OSBIDs::NoOsb)
	{
		if (NMControls.ROSBs.OSBInfo[rosb]->Caps & SoftOSBClass::OSBCaps::Boxable)
		{
			NMControls.ROSBs.OSBInfo[rosb]->State = (SoftOSBClass::States)(NMControls.ROSBs.OSBInfo[rosb]->State ^ SoftOSBClass::States::Boxed);
			currentPage->DrawROSB(rosb);
		}
		NMCommands::Commands pageSpecificCommand = NMControls.ROSBs.GetOSBPressCommand(rosb);
		// Pass command to the current page to handle:
		currentPage->Control(pageSpecificCommand);
	}
	
	switch (command)
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
	case NMCommands::NoCommand:
		Update();
		break;
	default:
		break;
	}
}

void MFCDClass::ActivatePage(PageIDs page)
{
	lastPage = currentPage;
	currentPage = Pages[page];

	currentPage->Activate(true);

}

void MFCDClass::SetBrightness(byte brightness)
{
	NMControls.SetLocalDisplayBrightness(brightness);

	//TODO: Try using ledc library
	analogWrite(MFCDLEDPin, Brightness);
}


MFCDClass MFCD;

