/*	NMControls.cpp
*	NMControlsClass - Base class for managing data from control panel of the MRS RC Navigation Module
*
*/

#include "NMControls.h"
#include "DEBUG Macros.h"

void NMControlsClass::Init()
{
	RightRockerSwitch = new ezButton(RightRockerSwitchPin);

	if (!HDGEncoder.begin(HDGEncoderI2CAddress) || !HDGNeoPix.begin(HDGEncoderI2CAddress))
	{
		sprintf(buf, "HDG encoder not found at %02X", HDGEncoderI2CAddress);
		_PL(buf);
		//tft.setCursor(2, tft.height() / 2 + 56);
		//tft.printf(buf);
	}
	else
	{
		sprintf(buf, "HDG encoder started at 0x%02X; ", HDGEncoderI2CAddress);
		_PP(buf);
		//tft.setCursor(2, tft.height() / 2 + 56);
		//tft.printf(buf);

		uint32_t afssVersion = ((HDGEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);
		//tft.setCursor(3 * tft.width() / 4, tft.height() / 2 + 56);
		//tft.printf(buf);

		HDGNeoPix.setPixelColor(0, HDGNeoPix.Color(0x00, 0x00, 0x08));
		//HDGNeoPix.setBrightness(0);
		HDGNeoPix.show();

		HDGEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		HDGSetting = 0 - HDGEncoder.getEncoderPosition();

		//_PL("Enabling HDG encoder interrupts");
		//delay(10);
		//AFSS.setGPIOInterrupts((uint32_t)1 << SS_BUTTON, 1);
		//AFSS.enableEncoderInterrupt();
	}

	if (!CRSEncoder.begin(CRSEncoderI2CAddress) || !CRSNeoPix.begin(CRSEncoderI2CAddress))
	{
		sprintf(buf, "CRS encoder not found at %02X", CRSEncoderI2CAddress);
		_PL(buf);
	}
	else
	{
		sprintf(buf, "CRS encoder started at 0x%02X; ", CRSEncoderI2CAddress);
		_PP(buf);

		uint32_t afssVersion = ((CRSEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);

		CRSNeoPix.setPixelColor(0, CRSNeoPix.Color(0x00, 0x08, 0x00));
		CRSNeoPix.show();

		CRSEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		CRSSetting = 0 - CRSEncoder.getEncoderPosition();
	}


}

void NMControlsClass::Init(byte rightRockerSwitchPin, byte hdgEncoderI2CAddress, byte crsEncoderI2CAddress)
{
	RightRockerSwitchPin = rightRockerSwitchPin;
	HDGEncoderI2CAddress = hdgEncoderI2CAddress;
	CRSEncoderI2CAddress = crsEncoderI2CAddress;

	Init();
}

void NMControlsClass::Update()
{
	RightRockerSwitch->loop();
	int newState = RightRockerSwitch->getState();
	newState = newState ? 0 : 1;	// Need to reverse sense of state for rocker switch
	if (RightRockerSwitchState != newState)
	{
		RightRockerSwitchState = newState;
		_PL(RightRockerSwitchState);
	}

	//TODO: Add debounce
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
		sprintf(buf, "%03D", HDGSetting);
		_PL(buf);
	}

	//TODO: Add debounce
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
		sprintf(buf, "%03D", CRSSetting);
		_PL(buf);
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
