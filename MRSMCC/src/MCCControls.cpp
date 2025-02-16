/*	MCCControls.h
*	MCCControls - Base class for managing control inputs from the MRS Master Control Computer module
*
*/

#include "MCCControls.h"
#include "MCCStatus.h"
#include "LocalDisplay.h"

void MCCControls::HandleNavButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventPressed)
	{
		NavSelected = true;
		//_PL("Navigation encoder button clicked");
	}
}

void MCCControls::HandleFuncButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventPressed)
	{
		FuncSelected = true;
		//_PL("Function encoder button clicked");
	}
}

uint32_t MCCControls::ReadCalibratedADC1(int rawADC1)
{
	return esp_adc_cal_raw_to_voltage(rawADC1, &ADC1Chars);
}

bool MCCControls::Init(TFT_eSPI* parentTFT)
{
	char buf[64];
	
	tft = parentTFT;

	if (!NavEncoder.begin(defaultNavEncoderI2CAddress) || !NavNeoPix.begin(defaultNavEncoderI2CAddress))
	{
		sprintf(buf, "Nav encoder not found at 0x%02X", defaultNavEncoderI2CAddress);
		_PL(buf);
		MCCStatus.NavEncoderStatus = false;
	}
	else
	{
		sprintf(buf, "Nav encoder started at 0x%02X; ", defaultNavEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((NavEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		NavNeoPix.setPixelColor(0, NavNeoPix.Color(0x08, 0x08, 0x00));
		NavNeoPix.show();

		NavEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		NavSetting = NavEncoder.getEncoderPosition();

		NavButtonConfig = new SSButtonConfig(NavEncoder);
		//NavButtonConfig->setDebounceDelay(50);
		NavButton = new ace_button::AceButton(NavButtonConfig);
		//NavButtonConfig->setFeature(ace_button::ButtonConfig::kFeatureClick);
		NavButtonConfig->setEventHandler(HandleNavButtonEvents);

		MCCStatus.NavEncoderStatus = true;
	}

	if (!FuncEncoder.begin(defaultFuncEncoderI2CAddress) || !FuncNeoPix.begin(defaultFuncEncoderI2CAddress))
	{
		sprintf(buf, "Func encoder not found at 0x%02X", defaultFuncEncoderI2CAddress);
		_PL(buf);
		MCCStatus.FuncEncoderStatus = false;
	}
	else
	{
		sprintf(buf, "Func encoder started at 0x%02X; ", defaultFuncEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((FuncEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		FuncNeoPix.setPixelColor(0, FuncNeoPix.Color(0x00, 0x0F, 0x00));
		FuncNeoPix.show();

		FuncEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		FuncSetting = 0 - FuncEncoder.getEncoderPosition();

		FuncButtonConfig = new SSButtonConfig(FuncEncoder);
		FuncButton = new ace_button::AceButton(FuncButtonConfig);
		//FuncButtonConfig->setFeature(ace_button::ButtonConfig::kFeatureClick);
		FuncButtonConfig->setEventHandler(HandleFuncButtonEvents);

		MCCStatus.FuncEncoderStatus = true;

	}
	
	MainMenu = new TFTMenuClass(tft);

	ESPNMenuItem = new MenuItemClass("ESPN", 36, 157, 56, 12, MenuItemClass::MenuItemTypes::OffOn);
	ESPNMenuItem->Init(tft);
	MainMenu->AddItem(ESPNMenuItem);
	ESPNMenuItem->SetOnExecuteHandler(nullptr);

	DriveModeMenuItem = new MenuItemClass("Mode", 98, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	DriveModeMenuItem->Init(tft);
	MainMenu->AddItem(DriveModeMenuItem);
	DriveModeMenuItem->SetOnExecuteHandler(nullptr);
	DriveModeMenuItem->SetMinValue(CSSMDrivePacket::DriveModes::DRV);
	DriveModeMenuItem->SetMaxValue(CSSMDrivePacket::DriveModes::DRVTw);
	DriveModeMenuItem->SetValue(CSSMDrivePacket::DriveModes::DRV);

	BRTMenuItem = new MenuItemClass("BRT", 162, 157, 56, 12, MenuItemClass::MenuItemTypes::Numeric);
	BRTMenuItem->Init(tft);
	MainMenu->AddItem(BRTMenuItem);
	BRTMenuItem->SetOnExecuteHandler(LocalDisplay.SetDisplayBrightness);
	BRTMenuItem->SetMinValue(0);
	BRTMenuItem->SetMaxValue(255);
	BRTMenuItem->SetNumericStepSize(16);
	BRTMenuItem->SetValue(LocalDisplay.GetDisplayBrightness());

	NextPageMenuItem = new MenuItemClass("Next", 226, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	NextPageMenuItem->Init(tft);
	MainMenu->AddItem(NextPageMenuItem);
	NextPageMenuItem->SetOnExecuteHandler(LocalDisplay.NextPage);
	NextPageMenuItem->SetMinValue(LocalDisplayClass::Pages::SYS);
	NextPageMenuItem->SetMaxValue(LocalDisplayClass::Pages::NONE);
	NextPageMenuItem->SetValue(LocalDisplayClass::Pages::COM);

	// Set up ADC:
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &ADC1Chars);    //Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		sprintf(buf, "eFuse Vref: %u mV", ADC1Chars.vref);
		_PL(buf);
		//Vref = adc_chars.vref;
	}
	else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		sprintf(buf, "Two Point --> coeff_a:%umV coeff_b:%umV\n", ADC1Chars.coeff_a, ADC1Chars.coeff_b);
		_PL(buf);
	}
	else {
		//Serial.println("Default Vref: 1100mV");
		sprintf(buf, "ADC1Chars.vref: %u mV", ADC1Chars.vref);
		_PL(buf);
		VRef = defaultVRef;
		sprintf(buf, "Defaul VRef: %u mV", VRef);
		_PL(buf);
	}

	pinMode(defaultVMCUPin, INPUT);
	VMCU.Init(0, 66, 40960, "V");

	if (!WSUPS3SINA219->begin())
	{
		_PL("INA219 initialization FAILED")
	}
	else
	{
		_PL("INA219 initialized")
	}

	return true;
}

void MCCControls::Update()
{
	bool NavWasSelected = NavSelected;
	bool FuncWasSelected = FuncSelected;
	MenuItemClass* currentItem = MainMenu->GetCurrentItem();

	if (MCCStatus.NavEncoderStatus)
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
	}

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
				currentItem->SetValue(currentItem->GetValue() ? 0 : 1);
				currentItem->Draw(tft, true);
				currentItem->InvokeOnExecuteHandler();
				NavSelected = false;
				break;
			case MenuItemClass::MenuItemTypes::Numeric:
				// Show the item in its Activated state:
				currentItem->Activate(true);
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

	if (MCCStatus.FuncEncoderStatus)
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
		//FuncButton->check();
	}

	if (FuncSelected) // Func encoder button was pressed...
	{
		if (currentItem != nullptr)
		{
			switch (currentItem->MenuItemType)
			{
			case MenuItemClass::MenuItemTypes::Action:
				currentItem->InvokeOnExecuteHandler();
				FuncSelected = false;
				break;
			case MenuItemClass::MenuItemTypes::Numeric:
				// Do something with the current Value of the current item:
				// The OnExecute handler (of type MenuItemOnExecuteHandler) is passed the current Value when invoked
				currentItem->InvokeOnExecuteHandler();
				// Reset selection of this item for input:
				currentItem->Activate(false);
				currentItem->Draw(tft, true);
				FuncSelected = false;
				NavSelected = false;
				break;
			default:
				FuncSelected = false;
				break;
			}
		}
	}


	uint16_t newReading = analogRead(defaultVMCUPin);
	VMCU.AddReading(newReading);

	INA219VBus = WSUPS3SINA219->getBusVoltage_V();
	INA219VShunt = WSUPS3SINA219->getShuntVoltage_mV();
	INA219Current = WSUPS3SINA219->getCurrent_mA();
	INA219Power = WSUPS3SINA219->getPower_mW();
	INA219VLoad = INA219VBus + (INA219VShunt / 1000.0f);

}

uint16_t MCCControls::GetMCURawADC()
{
	return VMCU.GetAverageRawValue();
}

float MCCControls::GetMCUVoltageReal()
{
	return VMCU.GetAverageRealValue();
}

String MCCControls::GetMCUVoltageString()
{
	return VMCU.GetRealString();
}

String MCCControls::GetMCUVoltageString(String format)
{
	return VMCU.GetRealString(format);
}

void MCCControls::CheckButtons()
{
	if (MCCStatus.NavEncoderStatus)
	{
		NavButton->check();
	}

	if (MCCStatus.FuncEncoderStatus)
	{
		FuncButton->check();
	}
}

void MCCControls::ToggleNavSelected()
{
	NavSelected = !NavSelected;
}

void MCCControls::ToggleFuncSelected()
{
	FuncSelected = !FuncSelected;
}

MCCControls mccControls;
bool MCCControls::NavSelected = false;
bool MCCControls::FuncSelected = false;
