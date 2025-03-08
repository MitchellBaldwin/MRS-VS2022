/*	BarGauge.h
*	BarGauge - Helper class to draw bar graph instrument on a TFT display showing drive settings,
*	Measured paramaters, etc. numerically and graphically
*
*
*
*
*	Mitchell Baldwin copyright 2025
*
*	v 0.0:	Initial commit
*
*/

#ifndef _BarGauge_h
#define _BarGauge_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

constexpr int defaultFrameWidth = 20;
constexpr int defaultFrameHeight = 85;
//constexpr int 

#include <TFT_eSPI.h>

class BarGauge
{
protected:
	TFT_eSPI* tft;
	
	int Xtl = 0;
	int Y0 = 0;
	int WFrame = defaultFrameWidth;
	int HFrame = defaultFrameHeight;
	int WBar = defaultFrameWidth - 4;
	int HBar = 0;

	uint16_t FrameColor = TFT_WHITE;
	uint16_t PosBarColor = TFT_GREEN;
	uint16_t NegBarColor = TFT_CYAN;

	float reading = 0.0f;				// The current value this instrument displays
	float minReading = 0.0f;
	float maxReading = 0.0f;

public:
	bool Init(TFT_eSPI* _tft, int xtl, int y0, int frameWidth = defaultFrameWidth, int frameHeight = defaultFrameHeight);
	void SetLimits(float min, float max);
	void SetFrameColor(uint16_t newFrameColor);
	void SetPosBarColor(uint16_t newPosBarColor);
	void SetNegBarColor(uint16_t newNegBarColor);

	void DrawFrame();
	void Update(float newReading);

};

//extern BarGauge ;

#endif

