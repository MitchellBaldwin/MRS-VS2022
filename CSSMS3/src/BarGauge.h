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

constexpr int defaultFrameWidth = 15;
constexpr int defaultFrameHeight = 85;
//constexpr int 

#include <TFT_eSPI.h>

class BarGauge
{
public:
	enum BarGaugeLayoutTypes
	{
		NoPowerBar,
		PowerBarLeft,
		PowerBarRight
	};

protected:
	TFT_eSPI* tft;
	
	BarGaugeLayoutTypes LayoutType = BarGaugeLayoutTypes::NoPowerBar;

	int X0 = 0;							// Horizontal center of the gauge
	int Xtl = 0;						// Top left corner of the frame
	int Y0 = 0;							// Vertical center of the gauge
	int Ytl = 0;						// Top left corner of the frame
	int WFrame = defaultFrameWidth;
	int HFrame = defaultFrameHeight;
	int WBar = defaultFrameWidth - 4;
	int HBar = 0;

	uint16_t FrameColor = TFT_WHITE;
	uint16_t LabelColor = TFT_YELLOW;
	uint16_t PosBarColor = TFT_GREEN;
	uint16_t NegBarColor = TFT_CYAN;
	uint16_t PowBarColor = TFT_ORANGE;

	String Label;
	float reading = 0.0f;				// The current value this instrument displays
	float minReading = 0.0f;
	float maxReading = 0.0f;

	float powerReading = 0.0f;
	float maxPowerReading = 0.0f;


public:
	bool Init(TFT_eSPI* _tft, int x0, int y0, BarGaugeLayoutTypes layoutType = BarGaugeLayoutTypes::NoPowerBar, int frameWidth = defaultFrameWidth, int frameHeight = defaultFrameHeight);
	void SetLabel(char* label);
	void SetLimits(float min, float max);
	void SetPowerLimit(float max);
	void SetFrameColor(uint16_t newFrameColor);
	void SetPosBarColor(uint16_t newPosBarColor);
	void SetNegBarColor(uint16_t newNegBarColor);
	void SetPowBarColor(uint16_t newPowBarColor);

	void DrawFrame();
	void Update(float newReading, float newPowerReading = 0.0f);

};

//extern BarGauge ;

#endif

