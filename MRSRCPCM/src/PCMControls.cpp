/*	PCMControls.h
*	PCMControlsClass - Base class for managing data from control panel of the MRS RC Power Control Module
*
*	Mitchell Baldwin copyright 2023-2024
*
*/

#include "PCMControls.h"
#include "PCMStatus.h"
#include "DEBUG Macros.h"
#include "LocalDisplay.h"

void PCMControlsClass::HandleNavButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventPressed)
	{
		//ToggleNavSelected();
		NavSelected = true;
		_PL("Navigation encoder button clicked");
	}
}

void PCMControlsClass::HandleFuncButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventClicked)
	{
		ToggleFuncSelected();
		_PL("Function encoder button clicked");
	}
}

void PCMControlsClass::ControlCSSMPower(byte value)
{
	digitalWrite(CSSMPowerEnablePin, value);
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
		//NavButtonConfig->setFeature(ace_button::ButtonConfig::kFeatureClick);
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

	CSSMMenuItem = new MenuItemClass("CSSM", 48, 157, 56, 12, MenuItemClass::MenuItemTypes::OffOn);
	MainMenu->AddItem(CSSMMenuItem);
	CSSMMenuItem->SetOnExecuteHandler(ControlCSSMPower);

	NMMenuItem = new MenuItemClass("NM", 112, 157, 56, 12, MenuItemClass::MenuItemTypes::OffOn);
	MainMenu->AddItem(NMMenuItem);

	BRTMenuItem = new MenuItemClass("BRT", 178, 157, 50, 12, MenuItemClass::MenuItemTypes::Numeric);
	MainMenu->AddItem(BRTMenuItem);
	BRTMenuItem->SetOnExecuteHandler(LocalDisplay.SetDisplayBrightness);
	BRTMenuItem->SetMinValue(0);
	BRTMenuItem->SetMaxValue(255);
	BRTMenuItem->SetNumericStepSize(16);
	BRTMenuItem->SetValue(LocalDisplay.GetDisplayBrightness());

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
			NavSelected = false;
		}
		else if (delta < 0)
		{
			currentItem = MainMenu->PrevItem();
			NavSelected = false;
		}
		NavButton->check();
	}

	//if (NavSelected ^ NavWasSelected) // Nav encoder button was pressed...
	if (NavSelected) // Nav encoder button was pressed...
	{
		// Check display brightness setting; if very low or 0 then set to a dim but readable value:
		if (LocalDisplay.GetDisplayBrightness() < 16)
		{
			LocalDisplay.SetDisplayBrightness(63);
			BRTMenuItem->SetValue(LocalDisplay.GetDisplayBrightness());
			BRTMenuItem->Draw(tft, true);
			NavSelected = false;
		}
		else if (currentItem != nullptr)
		{
			switch (currentItem->MenuItemType)
			{
			case MenuItemClass::MenuItemTypes::Action:
				currentItem->InvokeOnExecuteHandler();
				NavSelected = false;
				break;
			case MenuItemClass::MenuItemTypes::OffOn:
				//_PL(currentItem->GetValue())
				currentItem->SetValue(currentItem->GetValue() ? 0 : 1);
				//if (currentItem->GetValue() == 0)
				//{
				//	currentItem->SetValue(1);
				//}
				//else
				//{
				//	currentItem->SetValue(0);
				//}
				//_PL(currentItem->GetValue())
				currentItem->Draw(tft, true);
				currentItem->InvokeOnExecuteHandler();
				NavSelected = false;
				break;
			case MenuItemClass::MenuItemTypes::Numeric:
				// Item is selected for data entry, so set FuncEncoder accordingly:
				FuncSetting = currentItem->GetValue();
				currentItem->Draw(tft, true);
				// Do not unselect NavSelect until control input using Func encoder is complete:
				// (this will be reset when the Func encoder button is pressed)
				//NavSelected = false;
				break;
			default:
				break;
			}
		}
	}

	if (PCMStatus.FuncEncoderStatus)
	{
		int delta = FuncEncoder.getEncoderDelta();
		if (currentItem != nullptr && delta != 0 && NavSelected)
		{
			if (currentItem->MenuItemType == MenuItemClass::MenuItemTypes::Numeric)
			{
				delta > 0 ? FuncSetting += currentItem->GetNumericStepSize() : FuncSetting -= currentItem->GetNumericStepSize();
			}
			else 
			{
				FuncSetting = FuncSetting + delta;
			}

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

	if (FuncSelected ^ FuncWasSelected) // Func encoder button was pressed...
	{
		if (currentItem != nullptr)
		{
			switch (currentItem->MenuItemType)
			{
			case MenuItemClass::MenuItemTypes::Action:
				currentItem->InvokeOnExecuteHandler();
				break;
			case MenuItemClass::MenuItemTypes::Numeric:
				// Do something with the current Value of the current item:
				// The OnExecute handler (of type MenuItemOnExecuteHandler) is passed the current Value when invoked
				currentItem->InvokeOnExecuteHandler();
				// Reset selection of this item for input:
				NavSelected = false;
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

// Global and static declarations:
PCMControlsClass PCMControls;
bool PCMControlsClass::NavSelected = false;
bool PCMControlsClass::FuncSelected = false;
byte PCMControlsClass::CSSMPowerEnablePin = DefaultCSSMPowerEnablePin;
byte PCMControlsClass::NMPowerEnablePin = DefaultNMPowerEnablePin;
