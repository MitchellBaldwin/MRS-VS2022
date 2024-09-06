/*	NMControls.cpp
*	NMControlsClass - Base class for managing data from control panel of the MRS RC Navigation Module
*
*/

#include "NMControls.h"
#include "DEBUG Macros.h"

/// <summary>
/// Init() - Initialize Navigation Module controls class using default pin assignments and device addresses
/// </summary>
void NMControlsClass::Init()
{
	Init(DefaultRightRockerSwitchPin, DefaultHDGEncoderI2CAddress, DefaultCRSEncoderI2CAddress, DefaultBRTEncoderI2CAddress, DefaultTRREncoderI2CAddress);
}

void NMControlsClass::Init(
	byte rightRockerSwitchPin,
	byte hdgEncoderI2CAddress,
	byte crsEncoderI2CAddress,
	byte brtEncoderI2CAddress,
	byte trrEncoderI2CAddress)
{
	//// Set up OSB arrays:
	//LOSBSensePin = losbSensePin;
	//ROSBSensePin = rosbSensePin;
	//LOSBArray.Init(losbSensePin, LOSB_NUM_LEVELS, LOSB_LEVELS);
	//ROSBArray.Init(rosbSensePin, ROSB_NUM_LEVELS, ROSB_LEVELS);

	LOSBs.Init(DefaultLOSBSensePin, LOSB_NUM_LEVELS, LOSB_LEVELS);
	ROSBs.Init(DefaultROSBSensePin, ROSB_NUM_LEVELS, ROSB_LEVELS);

	RightRockerSwitch = new ezButton(rightRockerSwitchPin, INPUT_PULLUP);

	if (!HDGEncoder.begin(hdgEncoderI2CAddress) || !HDGNeoPix.begin(hdgEncoderI2CAddress))
	{
		sprintf(buf, "HDG encoder not found at %02X", hdgEncoderI2CAddress);
		_PL(buf);
	}
	else
	{
		sprintf(buf, "HDG encoder started at 0x%02X; ", hdgEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((HDGEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		HDGNeoPix.setPixelColor(0, HDGNeoPix.Color(0x00, 0x00, 0x08));
		HDGNeoPix.show();

		HDGEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		HDGSetting = 0 - HDGEncoder.getEncoderPosition();

		//_PL("Enabling HDG encoder interrupts");
		//delay(10);
		//AFSS.setGPIOInterrupts((uint32_t)1 << SS_BUTTON, 1);
		//AFSS.enableEncoderInterrupt();
	}

	if (!CRSEncoder.begin(crsEncoderI2CAddress) || !CRSNeoPix.begin(crsEncoderI2CAddress))
	{
		sprintf(buf, "CRS encoder not found at %02X", crsEncoderI2CAddress);
		_PL(buf);
	}
	else
	{
		sprintf(buf, "CRS encoder started at 0x%02X; ", crsEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((CRSEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		CRSNeoPix.setPixelColor(0, CRSNeoPix.Color(0x00, 0x08, 0x00));
		CRSNeoPix.show();

		CRSEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		CRSSetting = 0 - CRSEncoder.getEncoderPosition();
	}

	if (!BRTEncoder.begin(brtEncoderI2CAddress) || !BRTNeoPix.begin(brtEncoderI2CAddress))
	{
		sprintf(buf, "BRT encoder not found at %02X", brtEncoderI2CAddress);
		_PL(buf);
	}
	else
	{
		sprintf(buf, "BRT encoder started at 0x%02X; ", brtEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((BRTEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		BRTNeoPix.setPixelColor(0, BRTNeoPix.Color(0x00, 0x08, 0x00));
		BRTNeoPix.show();

		BRTEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		BRTEncoder.setEncoderPosition(BRTSetting);
	}

	if (!TRREncoder.begin(trrEncoderI2CAddress) || !TRRNeoPix.begin(trrEncoderI2CAddress))
	{
		sprintf(buf, "TRR encoder not found at %02X", trrEncoderI2CAddress);
		_PL(buf);
	}
	else
	{
		sprintf(buf, "TRR encoder started at 0x%02X; ", trrEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((TRREncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		TRRNeoPix.setPixelColor(0, TRRNeoPix.Color(0x00, 0x08, 0x00));
		TRRNeoPix.show();

		TRREncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		TRRSetting = 0 - TRREncoder.getEncoderPosition();
	}

}

void NMControlsClass::Update()
{
	LOSBs.Update();
	ROSBs.Update();

	// Read right rocker switch state:
	//DONE: Turning on the right rocker switch, which directly connects the port pin to GND, dims the display
	//Check: change in current draw when right rocker switch is turned on and off -> current draw increases 30~35 mA
	//Check: how ezButton handles internal pullup resistor settings - Defaults to INPUT_PULLUP
	//Try: reversing the sense such that turning the right rocker switch on connects the port pin to 3.3 V
	//instead of GND
	// Solution: Changed to GPIO05 (see NOTE in NMControls.h)
	RightRockerSwitch->loop();
	int newState = RightRockerSwitch->getState();
	newState = newState ? 0 : 1;					// Need to reverse sense of state for rocker switch
	if (RightRockerSwitchState != newState)
	{
		RightRockerSwitchState = newState;
		_PL(RightRockerSwitchState);
	}

	// Read encoders and check for encoder button presses:
	//TODO: Test debounce; wrap encoder button function into ACEButton, which should take care of debouncing and other
	//button conditioning and event handling needs
	if (!HDGEncoder.digitalRead(SS_BUTTON))
	{
		HDGButtonState = true;
	}
	int32_t newHDGSetting = HDGSetting - HDGEncoder.getEncoderDelta();
	if (newHDGSetting != HDGSetting)
	{
		if (newHDGSetting > 359)
		{
			newHDGSetting = 0;
		}
		else if (newHDGSetting < 0)
		{
			newHDGSetting = 359;
		}
		
		HDGSetting = newHDGSetting;
		//sprintf(buf, "%03D", HDGSetting);
		//_PL(buf);
	}

	if (HDGButtonWasPressed())
	{
		ToggleHDGSelected();
	}

	if (!CRSEncoder.digitalRead(SS_BUTTON))
	{
		CRSButtonState = true;
	}
	int32_t newCRSSetting = CRSSetting - CRSEncoder.getEncoderDelta();
	if (newCRSSetting != CRSSetting)
	{
		if (newCRSSetting > 359)
		{
			newCRSSetting = 0;
		}
		else if (newCRSSetting < 0)
		{
			newCRSSetting = 359;
		}
		
		CRSSetting = newCRSSetting;
		//sprintf(buf, "%03D", CRSSetting);
		//_PL(buf);
	}

	if (CRSButtonWasPressed())
	{
		ToggleCRSSelected();
	}

	if (!BRTEncoder.digitalRead(SS_BUTTON))
	{
		BRTButtonState = true;
	}
	int32_t newBRTSetting = BRTSetting - BRTEncoder.getEncoderDelta();
	if (newBRTSetting != BRTSetting)
	{
		if (newBRTSetting > 255)
		{
			newBRTSetting = 255;
		}
		else if (newBRTSetting < 0)
		{
			newBRTSetting = 0;
		}

		BRTSetting = newBRTSetting;
	}

	if (BRTButtonWasPressed())
	{
		ToggleBRTSelected();
	}

	if (!TRREncoder.digitalRead(SS_BUTTON))
	{
		TRRButtonState = true;
	}
	int32_t newTRRSetting = TRRSetting - TRREncoder.getEncoderDelta();
	if (newTRRSetting != TRRSetting)
	{
		if (newTRRSetting > 255)
		{
			newTRRSetting = 255;
		}
		else if (newTRRSetting < 0)
		{
			newTRRSetting = 0;
		}

		TRRSetting = newTRRSetting;
	}

}

bool NMControlsClass::HDGButtonWasPressed()
{
	if (HDGButtonState)
	{
		HDGButtonState = false;
		return true;
	}
	else
	{
		return false;
	}
}

void NMControlsClass::ToggleHDGSelected()
{
	HDGSelected = !HDGSelected;
}

bool NMControlsClass::CRSButtonWasPressed()
{
	if (CRSButtonState)
	{
		CRSButtonState = false;
		return true;
	}
	else
	{
		return false;
	}
}

void NMControlsClass::ToggleCRSSelected()
{
	CRSSelected = !CRSSelected;
}

bool NMControlsClass::BRTButtonWasPressed()
{
	if (BRTButtonState)
	{
		BRTButtonState = false;
		return true;
	}
	else
	{
		return false;
	}
}

void NMControlsClass::ToggleBRTSelected()
{
	BRTSelected = !BRTSelected;

}

bool NMControlsClass::TRRButtonWasPressed()
{
	if (TRRButtonState)
	{
		TRRButtonState = false;
		return true;
	}
	else
	{
		return false;
	}
}

void NMControlsClass::ToggleTRRSelected()
{
	TRRSelected = !TRRSelected;
}

void NMControlsClass::SetLocalDisplayBrightness(byte brightness)
{
	BRTSetting = brightness;
	BRTEncoder.setEncoderPosition(BRTSetting);
}

uint32_t NMControlsClass::ColorWheel(byte WheelPos)
{
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return HDGNeoPix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return HDGNeoPix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return HDGNeoPix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

NMControlsClass NMControls;
