/*	CSSMS3Controls.cpp
*	CSSMS3Controls - Base class for managing data from sensors on the MRS Control Stick Steering Module
*
*/

#include "CSSMS3Controls.h"
#include "CSSMS3Status.h"
#include "CSSMS3Display.h"
#include <DEBUG Macros.h>

void CSSMS3Controls::HandleDefaultButtonEvents(ace_button::AceButton* button, uint8_t eventType, uint8_t buttonState)
{
	if (button == cssmS3Controls.TS2)
	{
		if (eventType == AceButton::kEventPressed)
		{
			CSSMS3Status.TS2State = buttonState;
			//DONE: Switch display to current drive mode page:
			cssmS3Display.ShowCurrentDriveModePage();

			//_PP("TS2 on; ");
			//_PP("logical state:");
			//_PL(CSSMS3Status.TS2State);
		}
		if (eventType == AceButton::kEventReleased)
		{
			CSSMS3Status.TS2State = buttonState;
			// Switch display to last systems page:
			cssmS3Display.SetCurrentPage(cssmS3Display.GetLastSystemsPage());

			//_PP("TS2 off; ");
			//_PP("logical state:");
			//_PL(CSSMS3Status.TS2State);
		}

	}
}

void CSSMS3Controls::HandleNavButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventPressed)
	{
		NavSelected = true;
		//_PL("Navigation encoder button clicked");
	}
}

void CSSMS3Controls::HandleFuncButtonEvents(ace_button::AceButton* b, uint8_t eventType, uint8_t buttonState)
{
	if (eventType == ace_button::AceButton::kEventPressed)
	{
		FuncSelected = true;
		//_PL("Function encoder button clicked");
	}
}

void CSSMS3Controls::DriveOSBHandler(int value)
{
	if (CSSMS3Status.cssmDrivePacket.EStop)
	{
		// Take steps to ensure a safe restrat after an emergency stop:


		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::DRV;
		CSSMS3Status.cssmDrivePacket.EStop = false;
	}
	funcEncoderMode = FuncEncoderModes::MenuFuncEncMode;
	switch (CSSMS3Status.cssmDrivePacket.DriveMode)
	{
	case CSSMDrivePacket::DriveModes::DRV:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::DRVLR;
		break;
	case CSSMDrivePacket::DriveModes::DRVLR:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::DRVTw;
		funcEncoderMode = FuncEncoderModes::SteerMode;
		break;
	case CSSMDrivePacket::DriveModes::DRVTw:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::DRV;
		funcEncoderMode = FuncEncoderModes::SteerMode;
		break;
	default:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::DRV;
		funcEncoderMode = FuncEncoderModes::SteerMode;
		break;
	}
	cssmS3Display.ShowCurrentDriveModePage();
	//_PL("Drive mode handler")
}

void CSSMS3Controls::HeadingOSBHandler(int value)
{
	if (CSSMS3Status.cssmDrivePacket.EStop)
	{
		// Take steps to ensure a safe restrat after an emergency stop:


		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::HDG;
		CSSMS3Status.cssmDrivePacket.EStop = false;
	}
	funcEncoderMode = FuncEncoderModes::MenuFuncEncMode;
	switch (CSSMS3Status.cssmDrivePacket.DriveMode)
	{
	case CSSMDrivePacket::DriveModes::HDG:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::HDG;
		break;
	default:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::HDG;
		break;
	}
	cssmS3Display.ShowCurrentDriveModePage();
	//_PL("Heading mode handler")
}

void CSSMS3Controls::WaypointOSBHandler(int value)
{
	if (CSSMS3Status.cssmDrivePacket.EStop)
	{
		// Take steps to ensure a safe restrat after an emergency stop:

		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::WPT;
		CSSMS3Status.cssmDrivePacket.EStop = false;
	}
	funcEncoderMode = FuncEncoderModes::MenuFuncEncMode;
	switch (CSSMS3Status.cssmDrivePacket.DriveMode)
	{
	case CSSMDrivePacket::DriveModes::WPT:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::SEQ;
		break;
	default:
		CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::WPT;
		break;
	}
	cssmS3Display.ShowCurrentDriveModePage();
	//_PL("Waypoint mode handler")
}

void CSSMS3Controls::StopOSBHandler(int value)
{
	CSSMS3Status.cssmDrivePacket.EStop = true;
	CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::ESTOP;

	CSSMS3Status.cssmDrivePacket.LThrottle = 0.0f;
	CSSMS3Status.cssmDrivePacket.RThrottle = 0.0f;
	CSSMS3Status.cssmDrivePacket.SpeedSetting = 0.0f;
	CSSMS3Status.cssmDrivePacket.OmegaXYSetting = 0.0f;

	funcEncoderMode = FuncEncoderModes::MenuFuncEncMode;

	_PL("Stop mode handler")
}

uint32_t CSSMS3Controls::ReadCalibratedADC1(int rawADC1)
{
	return esp_adc_cal_raw_to_voltage(rawADC1, &ADC1Chars);
}

/// <summary>
/// Get the latest smoothed (averaged) measurement from the left throttle
/// </summary>
/// <returns>Averaged left throttle measurement</returns>
float CSSMS3Controls::GetLThrottleActual()
{
	return LThrottleSetting.GetAverageRealValue();
}

/// <summary>
/// Get left throttle setting in the range [-100%:+100%] with center dead zone
/// </summary>
/// <returns>Throttle setting adjusted for dead zone</returns>
float CSSMS3Controls::GetLThrottle()
{
	float actual = LThrottleSetting.GetAverageRealValue();

	if (abs(actual) < ThrottleDeadZone)
	{
		return 0.0f;
	}
	if (actual >= ThrottleDeadZone)
	{
		actual = (actual - ThrottleDeadZone) * 100.0f / (100.0f - ThrottleDeadZone);
	}
	else if (actual <= -ThrottleDeadZone)
	{
		actual = (actual + ThrottleDeadZone) * 100.0f / (100.0f - ThrottleDeadZone);
	}

	return actual;
}

/// <summary>
/// Get the latest smoothed (averaged) measurement from the right throttle
/// </summary>
/// <returns>Averaged left throttle measurement</returns>
float CSSMS3Controls::GetRThrottleActual()
{
	return RThrottleSetting.GetAverageRealValue();
}

/// <summary>
/// Get right throttle setting in the range [-100%:+100%] with center dead zone
/// </summary>
/// <returns>Throttle setting adjusted for dead zone</returns>
float CSSMS3Controls::GetRThrottle()
{
	float actual = RThrottleSetting.GetAverageRealValue();

	if (abs(actual) < ThrottleDeadZone)
	{
		return 0.0f;
	}
	if (actual >= ThrottleDeadZone)
	{
		actual = (actual - ThrottleDeadZone) * 100.0f / (100.0f - ThrottleDeadZone);
	}
	else if (actual <= -ThrottleDeadZone)
	{
		actual = (actual + ThrottleDeadZone) * 100.0f / (100.0f - ThrottleDeadZone);
	}

	return actual;
}

void CSSMS3Controls::NextDriveMode(int value)
{
	CSSMS3Status.cssmDrivePacket.NextDriveMode();
	if (CSSMS3Status.cssmDrivePacket.DriveMode == CSSMDrivePacket::DriveModes::DRVTw)
	{
		funcEncoderMode = FuncEncoderModes::SteerMode;
	}
	else
	{
		funcEncoderMode = FuncEncoderModes::MenuFuncEncMode;
	}
}

void CSSMS3Controls::SetHDG(int value)
{
	CSSMS3Status.cssmDrivePacket.HeadingSetting = value;
}

void CSSMS3Controls::CaptureHDG(int value)
{
	CSSMS3Status.cssmDrivePacket.HeadingSetting = CSSMS3Status.mcStatus.Heading;
	CSSMS3Status.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::HDG;
}

void CSSMS3Controls::SetSPD(int value)
{
	CSSMS3Status.cssmDrivePacket.SpeedSetting = value;
}

bool CSSMS3Controls::Init(TFT_eSPI* parentTFT)
{
	char buf[64];

	tft = parentTFT;

	// Configure TS2 for use switching between display of systems pages and drive mode pages:
	pinMode(defaultTS2Pin, INPUT);
	TS2 = new ace_button::AceButton(defaultTS2Pin, true, 0);
	ace_button::ButtonConfig* defaultACEButtonConfig = TS2->getButtonConfig();
	defaultACEButtonConfig->setEventHandler(HandleDefaultButtonEvents);

	if (!NavEncoder.begin(defaultNavEncoderI2CAddress) || !NavNeoPix.begin(defaultNavEncoderI2CAddress))
	{
		sprintf(buf, "Nav encoder not found at 0x%02X", defaultNavEncoderI2CAddress);
		_PL(buf);
		CSSMS3Status.NavEncoderStatus = false;
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

		CSSMS3Status.NavEncoderStatus = true;
	}

	if (!FuncEncoder.begin(defaultFuncEncoderI2CAddress) || !FuncNeoPix.begin(defaultFuncEncoderI2CAddress))
	{
		sprintf(buf, "Func encoder not found at 0x%02X", defaultFuncEncoderI2CAddress);
		_PL(buf);
		CSSMS3Status.FuncEncoderStatus = false;
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

		CSSMS3Status.FuncEncoderStatus = true;

	}

	OSBs = new OSBArrayClass(defaultKPSensePin);
	OSBs->Init(OSBLevelCount, OSBLevels);

	MainMenu = new TFTMenuClass();
	MainMenu->Init(tft);

	ESPNMenuItem = new MenuItemClass("ESPN", 36, 157, 56, 12, MenuItemClass::MenuItemTypes::OffOn);
	ESPNMenuItem->Init(tft);
	MainMenu->AddItem(ESPNMenuItem);
	ESPNMenuItem->SetOnExecuteHandler(cssmS3Controls.SetESPNOW);

	DriveModeMenuItem = new MenuItemClass("Mode", 98, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	DriveModeMenuItem->Init(tft);
	MainMenu->AddItem(DriveModeMenuItem);
	DriveModeMenuItem->SetOnExecuteHandler(NextDriveMode);
	DriveModeMenuItem->SetMinValue(CSSMDrivePacket::DriveModes::DRV);
	DriveModeMenuItem->SetMaxValue(CSSMDrivePacket::DriveModes::DRVTw);
	DriveModeMenuItem->SetValue(CSSMDrivePacket::DriveModes::DRV);

	BRTMenuItem = new MenuItemClass("BRT", 162, 157, 56, 12, MenuItemClass::MenuItemTypes::Numeric);
	BRTMenuItem->Init(tft);
	MainMenu->AddItem(BRTMenuItem);
	BRTMenuItem->SetOnExecuteHandler(cssmS3Display.SetDisplayBrightness);
	BRTMenuItem->SetMinValue(0);
	BRTMenuItem->SetMaxValue(255);
	BRTMenuItem->SetNumericStepSize(16);
	BRTMenuItem->SetValue(cssmS3Display.GetDisplayBrightness());

	NextPageMenuItem = new MenuItemClass("Next", 226, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	NextPageMenuItem->Init(tft);
	MainMenu->AddItem(NextPageMenuItem);
	NextPageMenuItem->SetOnExecuteHandler(cssmS3Display.NextPage);
	NextPageMenuItem->SetMinValue(CSSMS3Display::Pages::SYS);
	NextPageMenuItem->SetMaxValue(CSSMS3Display::Pages::NONE);
	NextPageMenuItem->SetValue(CSSMS3Display::Pages::SYS);

	CommsMenu = new TFTMenuClass();
	CommsMenu->Init(tft);

	CommsMenu->AddItem(NextPageMenuItem);

	CommsMenu->AddItem(ESPNMenuItem);

	WiFiMenuItem = new MenuItemClass("WiFi", 98, 157, 56, 12, MenuItemClass::MenuItemTypes::OffOn);
	WiFiMenuItem->Init(tft);
	CommsMenu->AddItem(WiFiMenuItem);
	WiFiMenuItem->SetOnExecuteHandler(SetWiFi);

	DebugMenu = new TFTMenuClass();
	DebugMenu->Init(tft);

	DebugMenu->AddItem(NextPageMenuItem);

	ReportMemoryMenuItem = new MenuItemClass("Mem", 36, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	ReportMemoryMenuItem->Init(tft);
	DebugMenu->AddItem(ReportMemoryMenuItem);
	ReportMemoryMenuItem->SetOnExecuteHandler(cssmS3Display.ReportHeapStatus);

	ShowFontMenuItem = new MenuItemClass("Font", 98, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	ShowFontMenuItem->Init(tft);
	DebugMenu->AddItem(ShowFontMenuItem);
	ShowFontMenuItem->SetOnExecuteHandler(cssmS3Display.ShowFontTableFixed);

	HDGPageMenu = new TFTMenuClass();
	HDGPageMenu->Init(tft);

	//HDGPageMenu->AddItem(NextPageMenuItem);
	
	HDGHoldMenuItem = new MenuItemClass("HOLD", 36, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	HDGHoldMenuItem->Init(tft);
	HDGPageMenu->AddItem(HDGHoldMenuItem);
	HDGHoldMenuItem->SetOnExecuteHandler(nullptr);

	HDGSetMenuItem = new MenuItemClass("HDG", 98, 157, 56, 12, MenuItemClass::MenuItemTypes::Numeric);
	HDGSetMenuItem->Init(tft);
	HDGPageMenu->AddItem(HDGSetMenuItem);
	HDGSetMenuItem->SetOnExecuteHandler(SetHDG);
	HDGSetMenuItem->SetMinValue(0);
	HDGSetMenuItem->SetMaxValue(359);
	//DONE: HDG & CRS settings should be in the range 0 - 359, but the MenuItem numeric value is of type byte; change to int32_t or something more generally useful!
	HDGSetMenuItem->SetNumericStepSize(1);
	HDGSetMenuItem->SetValue(CSSMS3Status.cssmDrivePacket.HeadingSetting);

	//DONE: Change Speed setting from % of full speed to mm/s units
	SPDSetMenuItem = new MenuItemClass("SPD", 162, 157, 56, 12, MenuItemClass::MenuItemTypes::Numeric);
	SPDSetMenuItem->Init(tft);
	HDGPageMenu->AddItem(SPDSetMenuItem);
	SPDSetMenuItem->SetOnExecuteHandler(SetSPD);
	SPDSetMenuItem->SetMinValue(-1000);
	SPDSetMenuItem->SetMaxValue(1000);
	SPDSetMenuItem->SetNumericStepSize(1);
	SPDSetMenuItem->SetValue((int)CSSMS3Status.cssmDrivePacket.SpeedSetting);

	DRVPageMenu = new TFTMenuClass();
	DRVPageMenu->Init(tft);

	CaptureHDGMenuItem = new MenuItemClass("CAP HDG", 36, 157, 56, 12, MenuItemClass::MenuItemTypes::Action);
	CaptureHDGMenuItem->Init(tft);
	DRVPageMenu->AddItem(CaptureHDGMenuItem);
	CaptureHDGMenuItem->SetOnExecuteHandler(CaptureHDG);

	DRVPageMenu->AddItem(HDGSetMenuItem);
	//DRVPageMenu->AddItem(NextPageMenuItem);

	// Set up ADC:
	SetupADC();

	pinMode(KPSensePin, INPUT);
	// Assumes use of a unity gain OpAmp buffer (3.3 V max);
	// 3.3 V / 4096 * 1000000 = 806 uV/count:
	KPVoltage.Init(0, 33, 40960, "V", 16);

	pinMode(LThrottlePin, INPUT);
	// Assumes use of a potentiometer voltage divider working from 0.0 - 3.3 V (0 - 4095)
	// Return throttle setting in the range -100.0% to +100.0%
	// +/- 100.0% / 2048 = 0.04883 %/count
	LThrottleSetting.Init(2048, 1000, 20480, "%");

	pinMode(RThrottlePin, INPUT);
	// Assumes use of a potentiometer voltage divider working from 0.0 - 3.3 V (0 - 4095)
	// Return throttle setting in the range -100.0% to +100.0%
	// +/- 100.0% / 2048 = 0.04883 %/count
	RThrottleSetting.Init(2048, 1000, 20480, "%");

	pinMode(VMCUPin, INPUT);
	VMCU.Init(0, 69, 40960, "V");

	return true;

}

void CSSMS3Controls::Update()
{
	bool NavWasSelected = NavSelected;
	bool FuncWasSelected = FuncSelected;

	TFTMenuClass* currentMenu = nullptr;

	switch (cssmS3Display.GetCurrentPage())
	{
	case CSSMS3Display::Pages::SYS:
	{
		currentMenu = MainMenu;
		break;
	}
	case CSSMS3Display::Pages::COM:
	{
		currentMenu = CommsMenu;
		break;
	}
	case CSSMS3Display::Pages::DBG:
	{
		currentMenu = DebugMenu;
		break;
	}
	case CSSMS3Display::Pages::DRV:
	{
		currentMenu = DRVPageMenu;
		break;
	}
	case CSSMS3Display::Pages::HDG:
	{
		currentMenu = HDGPageMenu;
		break;
	}
	default:
		currentMenu = MainMenu;
		break;
	}
	MenuItemClass* currentItem = currentMenu->GetCurrentItem();

	if (CSSMS3Status.NavEncoderStatus)
	{
		int delta = NavEncoder.getEncoderDelta();
		switch (navEncoderMode)
		{
		case NavEncoderModes::MenuNavEncMode:
			if (delta > 0)
			{
				currentItem = currentMenu->NextItem();
				NavSelected = false;
			}
			else if (delta < 0)
			{
				currentItem = currentMenu->PrevItem();
				NavSelected = false;
			}
			break;
		case NavEncoderModes::HDGSetMode:

			break;
		default:
			break;
		}

	}

	if (NavSelected) // Nav encoder button was pressed...
	{
		// Check display brightness setting; if very low or 0 then set to a dim but readable value:
		if (cssmS3Display.GetDisplayBrightness() <= 32)
		{
			cssmS3Display.SetDisplayBrightness(64);
			BRTMenuItem->SetValue(cssmS3Display.GetDisplayBrightness());
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

	if (CSSMS3Status.FuncEncoderStatus)
	{
		int delta = FuncEncoder.getEncoderDelta();
		switch (funcEncoderMode)
		{
		case FuncEncoderModes::MenuFuncEncMode:
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
			break;
		case FuncEncoderModes::SteerMode:
			if (!CSSMS3Status.cssmDrivePacket.EStop)
			{
				if (delta > 0)
				{
					CSSMS3Status.cssmDrivePacket.OmegaXYSettingPct += 5.0f;
				}
				else if (delta < 0)
				{
					CSSMS3Status.cssmDrivePacket.OmegaXYSettingPct -= 5.0f;
				}
			}
			break;
		default:
			break;
		}
		
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
	
	byte OSBPressed = OSBs->GetOSBPress();
	switch (OSBPressed)
	{
	case OSBArrayClass::OSBs::OSB1:
		DriveOSBHandler(0);
		break;
	case OSBArrayClass::OSBs::OSB2:
		HeadingOSBHandler(0);
		break;
	case OSBArrayClass::OSBs::OSB3:
		WaypointOSBHandler(0);
		break;
	case OSBArrayClass::OSBs::OSB4:
		StopOSBHandler(0);
		break;
	default:
		break;
	}
	
	uint16_t newReading = analogRead(KPSensePin);
	KPVoltage.AddReading(newReading);
	
	newReading = analogRead(LThrottlePin);
	LThrottleSetting.AddReading(newReading);
	CSSMS3Status.cssmDrivePacket.LThrottle = GetLThrottle();

	newReading = analogRead(RThrottlePin);
	RThrottleSetting.AddReading(newReading);
	CSSMS3Status.cssmDrivePacket.RThrottle = GetRThrottle();

	// Update speed setting based on throttle settings (when in DRVTw dirve mode):
	if (CSSMS3Status.cssmDrivePacket.DriveMode == CSSMDrivePacket::DriveModes::DRVTw)
	{
		CSSMS3Status.cssmDrivePacket.SpeedSettingPct = (CSSMS3Status.cssmDrivePacket.LThrottle + CSSMS3Status.cssmDrivePacket.RThrottle) / 2.0f;

	}
	newReading = analogRead(VMCUPin);
	VMCU.AddReading(newReading);

}

bool CSSMS3Controls::SetupADC()
{
	char buf[64];
	
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
	return true;
}

uint16_t CSSMS3Controls::GetKPRawADC()
{
	return KPVoltage.GetAverageRawValue();
}

float CSSMS3Controls::GetKPVoltageReal()
{
	return KPVoltage.GetAverageRealValue();
}

String CSSMS3Controls::GetKPVoltageString()
{
	return KPVoltage.GetRealString();
}

String CSSMS3Controls::GetKPVoltageString(String format)
{
	return KPVoltage.GetRealString(format);
}

uint16_t CSSMS3Controls::GetMCURawADC()
{
	return VMCU.GetAverageRawValue();
}

float CSSMS3Controls::GetMCUVoltageReal()
{
	return VMCU.GetAverageRealValue();
}

String CSSMS3Controls::GetMCUVoltageString()
{
	return VMCU.GetRealString();
}

String CSSMS3Controls::GetMCUVoltageString(String format)
{
	return VMCU.GetRealString(format);
}

void CSSMS3Controls::CheckButtons()
{
	TS2->check();

	if (CSSMS3Status.NavEncoderStatus)
	{
		NavButton->check();
	}

	if (CSSMS3Status.FuncEncoderStatus)
	{
		FuncButton->check();
	}

}

bool CSSMS3Controls::GetTS2State()
{
	bool ts2State = digitalRead(defaultTS2Pin);
	return ts2State;
}

void CSSMS3Controls::ToggleNavSelected()
{
	NavSelected = !NavSelected;
}

void CSSMS3Controls::ToggleFuncSelected()
{
	FuncSelected = !FuncSelected;
}

void CSSMS3Controls::SetESPNOW(int value)
{
	if (cssmS3Controls.ESPNMenuItem->GetValue())
	{
		CSSMS3Status.ESPNOWStatus = true;
	}
	else
	{
		CSSMS3Status.ESPNOWStatus = false;
	}
}

bool CSSMS3Controls::GetESPNowStatus()
{
	return (bool)ESPNMenuItem->GetValue();
}

void CSSMS3Controls::SetESPNOWStatus(bool newStatus)
{
	ESPNMenuItem->SetValue((int)newStatus);
	SetESPNOW(ESPNMenuItem->GetValue());
}

void CSSMS3Controls::SetWiFi(int value)
{
	if (cssmS3Controls.WiFiMenuItem->GetValue())
	{
		CSSMS3Status.WiFiStatus = true;
	}
	else
	{
		CSSMS3Status.WiFiStatus = false;
	}
}


// Global and static variable declarations:
CSSMS3Controls cssmS3Controls;
CSSMS3Controls::NavEncoderModes CSSMS3Controls::navEncoderMode = CSSMS3Controls::NavEncoderModes::MenuNavEncMode;
CSSMS3Controls::FuncEncoderModes CSSMS3Controls::funcEncoderMode = CSSMS3Controls::FuncEncoderModes::MenuFuncEncMode;
bool CSSMS3Controls::NavSelected = false;
bool CSSMS3Controls::FuncSelected = false;
