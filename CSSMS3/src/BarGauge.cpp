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

bool BarGauge::Init(TFT_eSPI* _tft, int xtl, int y0, int frameWidth, int frameHeight)
{
	tft = _tft;
	
	Xtl = xtl;
	Y0 = y0;

	WFrame = frameWidth;
	HFrame = frameHeight;

	return true;
}

void BarGauge::SetLimits(float min, float max)
{
	minReading = min;
	maxReading = max;

	Update(reading);
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

void BarGauge::DrawFrame()
{
	tft->fillRect(Xtl, Y0 - HFrame / 2, WFrame, HFrame, TFT_BLACK);
	tft->drawRect(Xtl, Y0 - HFrame / 2, WFrame, HFrame, FrameColor);
	tft->drawLine(Xtl, Y0, Xtl + WFrame - 1, Y0, FrameColor);
}

void BarGauge::Update(float newReading)
{
	//DrawFrame();
	reading = newReading;
	int16_t HBar = 0;
	if (reading < 0.0f)
	{
		tft->fillRect(Xtl + 2, Y0 + 1, WFrame - 4, HFrame / 2 - 1, TFT_BLACK);
		HBar = reading / minReading * (HFrame / 2 - 1);
		tft->fillRect(Xtl + 2, Y0 + 1, WFrame - 4, HBar, NegBarColor);
	}
	else
	{
		tft->fillRect(Xtl + 2, Y0 - HFrame / 2 + 1, WFrame - 4, HFrame / 2 - 1, TFT_BLACK);
		HBar = reading / maxReading * (HFrame / 2 - 1);
		tft->fillRect(Xtl + 2, Y0 - HBar, WFrame - 4, HBar, PosBarColor);
	}
}
