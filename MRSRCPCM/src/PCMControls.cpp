/*	PCMControls.h
*	PCMControlsClass - Base class for managing data from control panel of the MRS RC Power Control Module
*
*	Mitchell Baldwin copyright 2023-2024
*
*/

#include "PCMControls.h"
#include "PCMStatus.h"
#include "DEBUG Macros.h"

void PCMControlsClass::HandleNavButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventClicked)
	{
		ToggleNavSelected();
		_PL("Navigation encoder button clicked");
	}
}

void PCMControlsClass::HandleFuncButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	ToggleFuncSelected();
	_PL("Function encoder button clicked");
}

void PCMControlsClass::Init(TFT_eSPI* parentTFT)
{
	tft = parentTFT;
	Init(DefaultNavEncoderI2CAddress, DefaultFuncEncoderI2CAddress);
}

void PCMControlsClass::Init(byte navEncoderI2CAddress, byte funcEncoderI2CAddress)
{
	Init(navEncoderI2CAddress, funcEncoderI2CAddress, DefaultCSSMPowerEnablePin, DefaultNMPowerEnablePin);
}

void PCMControlsClass::Init(byte navEncoderI2CAddress, byte funcEncoderI2CAddress, byte cssmPowerEnablePin, byte nmPowerEnablePin)
{
	if (!NavEncoder.begin(navEncoderI2CAddress) || !NavNeoPix.begin(navEncoderI2CAddress))
	{
		sprintf(buf, "Nav encoder not found at 0x%02X", navEncoderI2CAddress);
		_PL(buf);
		PCMStatus.NavEncoderStatus = false;
	}
	else
	{
		sprintf(buf, "Nav encoder started at 0x%02X; ", navEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((NavEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		NavNeoPix.setPixelColor(0, NavNeoPix.Color(0x00, 0x00, 0x08));
		NavNeoPix.show();

		NavEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		NavSetting = NavEncoder.getEncoderPosition();

		NavButtonConfig = new SSButtonConfig(NavEncoder);
		//NavButtonConfig->setDebounceDelay(50);
		NavButton = new ace_button::AceButton(NavButtonConfig);
		NavButtonConfig->setFeature(ace_button::ButtonConfig::kFeatureClick);
		NavButtonConfig->setEventHandler(HandleNavButtonEvents);

		PCMStatus.NavEncoderStatus = true;
	}

	if (!FuncEncoder.begin(funcEncoderI2CAddress) || !FuncNeoPix.begin(funcEncoderI2CAddress))
	{
		sprintf(buf, "Func encoder not found at 0x%02X", funcEncoderI2CAddress);
		_PL(buf);
		PCMStatus.FuncEncoderStatus = false;
	}
	else
	{
		sprintf(buf, "Func encoder started at 0x%02X; ", funcEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((FuncEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		FuncNeoPix.setPixelColor(0, FuncNeoPix.Color(0x00, 0x00, 0x08));
		FuncNeoPix.show();

		FuncEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		FuncSetting = 0 - FuncEncoder.getEncoderPosition();

		FuncButtonConfig = new SSButtonConfig(FuncEncoder);
		FuncButton = new ace_button::AceButton(FuncButtonConfig);
		FuncButtonConfig->setFeature(ace_button::ButtonConfig::kFeatureClick);
		FuncButtonConfig->setEventHandler(HandleFuncButtonEvents);
		
		PCMStatus.FuncEncoderStatus = true;

	}
	
	CSSMPowerEnablePin = cssmPowerEnablePin;
	pinMode(CSSMPowerEnablePin, OUTPUT);
	digitalWrite(CSSMPowerEnablePin, 0);
	
	NMPowerEnablePin = nmPowerEnablePin;
	pinMode(NMPowerEnablePin, OUTPUT);
	digitalWrite(NMPowerEnablePin, 0);

	MainMenu = new TFTMenuClass(tft);
	MainMenu->AddItem(new MenuItemClass("CSSM", 10, 60, 50, 12, MenuItemClass::MenuItemTypes::OffOn));
	MainMenu->AddItem(new MenuItemClass("NM", 10, 72, 50, 12, MenuItemClass::MenuItemTypes::OffOn));
	MainMenu->AddItem(new MenuItemClass("BRT", 10, 84, 50, 12, MenuItemClass::MenuItemTypes::Numeric));

}

void PCMControlsClass::CheckButtons()
{
	if (PCMStatus.NavEncoderStatus)
	{
		NavButton->check();
	}

	if (PCMStatus.FuncEncoderStatus)
	{
		FuncButton->check();
	}
}

void PCMControlsClass::Update()
{
	bool NavWasSelected = NavSelected;
	bool FuncWasSelected = FuncSelected;
	MenuItemClass* currentItem = MainMenu->GetCurrentItem();

	if (PCMStatus.NavEncoderStatus)
	{
		int delta = NavEncoder.getEncoderDelta();
		if (delta > 0)
		{
			currentItem = MainMenu->NextItem();
		}
		else if (delta < 0)
		{
			currentItem = MainMenu->PrevItem();
		}
		NavButton->check();
	}

	if (PCMStatus.FuncEncoderStatus)
	{
		int delta = FuncEncoder.getEncoderDelta();
		FuncSetting = FuncSetting + delta;
		if (currentItem != nullptr && delta != 0)
		{
			switch (currentItem->MenuItemType)
			{
			case MenuItemClass::MenuItemTypes::Numeric:
				currentItem->SetValue(FuncSetting);
				currentItem->Draw(tft, true);
				break;
			default:
				break;
			}
		}
		FuncButton->check();
	}

	if (NavSelected ^ NavWasSelected) // Nav encoder button was pressed...
	{
		if (currentItem != nullptr)
		{
			switch (currentItem->MenuItemType)
			{
			case MenuItemClass::MenuItemTypes::Action:
				currentItem->InvokeOnExecuteHandler();
				break;
			case MenuItemClass::MenuItemTypes::OffOn:
				currentItem->SetValue(NavSelected ? 1 : 0);
				currentItem->Draw(tft, true);
				digitalWrite(CSSMPowerEnablePin, currentItem->GetValue());
				break;
			case MenuItemClass::MenuItemTypes::Numeric:
				// Item is selected for data entry, so set FuncEncoder accordingly:
				FuncSetting = currentItem->GetValue();
				currentItem->Draw(tft, true);
				break;
			default:
				break;
			}
		}
	}

	if (FuncSelected ^ FuncWasSelected) // Func encoder button was pressed...
	{
		if (currentItem != nullptr)
		{
			switch (currentItem->MenuItemType)
			{
			case MenuItemClass::MenuItemTypes::Numeric:
				// Do something with the current Value of the current item:
				//currentItem->SetValue(FuncSetting);
				break;
			default:
				break;
			}
		}
	}
}

void PCMControlsClass::ToggleNavSelected()
{
	NavSelected = !NavSelected;
}

void PCMControlsClass::ToggleFuncSelected()
{
	FuncSelected = !FuncSelected;
}


PCMControlsClass PCMControls;
bool PCMControlsClass::NavSelected = false;
bool PCMControlsClass::FuncSelected = false;
