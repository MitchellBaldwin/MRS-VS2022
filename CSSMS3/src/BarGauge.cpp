/*	BarGauge.cpp
*	BarGauge - Helper class to draw bar graph instrument on a TFT display showing drive settings,
*	Measured paramaters, etc. numerically and graphically
*
*/

#include "BarGauge.h"
#include <math.h>

//bool BarGauge::Init()
//{
//
//	return true;
//}

bool BarGauge::Init(TFT_eSPI* _tft, int x0, int y0, BarGaugeLayoutTypes layoutType, int frameWidth, int frameHeight)
{
	tft = _tft;
	
	WFrame = frameWidth;
	HFrame = frameHeight;

	X0 = x0;					// Horizontal center of the gauge
	Xtl = x0 - WFrame / 2;		// Top left corner of the frame
	Y0 = y0;					// Vertical center of the gauge
	Ytl = y0 - HFrame / 2;		// Top left corner of the frame

	WBar = WFrame - 4;

	LayoutType = layoutType;

	return true;
}

void BarGauge::SetLabel(char* label)
{
	Label = label;
}

void BarGauge::SetLimits(float min, float max)
{
	minReading = min;
	maxReading = max;

	//Update(reading, powerReading);
}

void BarGauge::SetPowerLimit(float max)
{
	maxPowerReading = max;

	//Update(reading, powerReading);
}

void BarGauge::SetFrameColor(uint16_t newFrameColor)
{
	FrameColor = newFrameColor;
}

void BarGauge::SetPosBarColor(uint16_t newPosBarColor)
{
	PosBarColor = newPosBarColor;
}

void BarGauge::SetNegBarColor(uint16_t newNegBarColor)
{
	NegBarColor = newNegBarColor;
}

void BarGauge::SetPowBarColor(uint16_t newPowBarColor)
{
	PowBarColor = newPowBarColor;
}

void BarGauge::DrawFrame()
{
	tft->fillRect(Xtl, Ytl, WFrame, HFrame, TFT_BLACK);
	tft->drawRect(Xtl, Ytl, WFrame, HFrame, FrameColor);
	tft->drawLine(Xtl, Y0, X0 + WFrame / 2, Y0, FrameColor);
	tft->setTextColor(LabelColor, TFT_BLACK, true);
	tft->setTextDatum(TC_DATUM);
	tft->drawString(Label, X0 + 1, Ytl + HFrame + 2, 1);
	//switch (LayoutType)
	//{
	//case NoPowerBar:
	//	tft->setTextDatum(TC_DATUM);
	//	tft->drawString(Label, X0 + 1, Ytl + HFrame + 2, 1);
	//	break;
	//case PowerBarLeft:
	//	tft->setTextDatum(TL_DATUM);
	//	tft->drawString(Label, Xtl - 4, Ytl + HFrame + 2, 1);
	//	break;
	//case PowerBarRight:
	//	tft->setTextDatum(TR_DATUM);
	//	tft->drawString(Label, Xtl + WFrame + 4, Ytl + HFrame + 2, 1);
	//	break;
	//default:
	//	break;
	//}
}

void BarGauge::Update(float newReading, float newPowerReading)
{
	char buf[16];
	
	reading = newReading;
	int16_t HBar = 0;
	if (reading < 0.0f)
	{
		tft->fillRect(Xtl + 2, Y0 + 1, WBar, HFrame / 2 - 1, TFT_BLACK);
		HBar = reading / minReading * (HFrame / 2 - 1);
		tft->fillRect(Xtl + 2, Y0 + 1, WBar, HBar, NegBarColor);
		tft->setTextColor(NegBarColor, TFT_BLACK, true);
	}
	else
	{
		tft->fillRect(Xtl + 2, Y0 - HFrame / 2 + 1, WBar, HFrame / 2 - 1, TFT_BLACK);
		HBar = reading / maxReading * (HFrame / 2 - 1);
		tft->fillRect(Xtl + 2, Y0 - HBar, WBar, HBar, PosBarColor);
		tft->setTextColor(PosBarColor, TFT_BLACK, true);
	}

	powerReading = newPowerReading;
	// Constrain height of power bar to 0..maxPowerReading:
	HBar = min((int)(powerReading / maxPowerReading * HFrame), HFrame);

	// Display numeric reading as pecent of full scale:
	float pctReading = 0.0f;
	if (reading < 0.0f)
	{
		pctReading = 0.0f - reading / minReading * 100.0f;
	}
	else
	{
		pctReading = reading / maxReading * 100.0f;
	}
	sprintf(buf, "%+04.0f", pctReading);

	tft->setTextDatum(TC_DATUM);
	tft->drawString(buf, X0 + 1, Ytl + HFrame + 12, 1);
	switch (LayoutType)
	{
	case NoPowerBar:
		break;
	case PowerBarLeft:
		// Erase old power bar:
		tft->fillRect(Xtl - 4, Ytl, 3, HFrame, TFT_BLACK);
		// Draw power bar:
		tft->fillRect(Xtl - 4, Ytl + HFrame - HBar, 3, HBar, PowBarColor);
		break;
	case PowerBarRight:
		// Erase old power bar:
		tft->fillRect(Xtl + WFrame + 2, Ytl, 3, HFrame, TFT_BLACK);
		// Draw power bar:
		tft->fillRect(Xtl + WFrame + 2, Ytl + HFrame - HBar, 3, HBar, PowBarColor);
		break;
	default:
		break;
	}
}
