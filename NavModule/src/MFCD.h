/*	MFCDClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*	Pages supported:
*		Navigation	(NAV)	Primary navigation display
*		Comms		(COM)	Communications with MRS RC MCC
*		System		(SYS)	Default summary of navigation systems, power and I/O status
*		Debug		(DBG)	Development and debugging
*
	Display: HiLetgo ILI9341 2.8" SPI TFT LCD Display Touch Panel 240X320
*		TFT_MISO 19  // (leave TFT SDO disconnected if other SPI devices share MISO)
*		TFT_MOSI 23
*		TFT_SCLK 18
*		TFT_CS   15  // Chip select control pin
*		TFT_DC    2  // Data Command control pin
*		TFT_RST   4  // Reset pin (could connect to RST pin)
*
*	Mitchell Baldwin copyright 2023-2024
*
*	v 0.00:	Initial command set
*	v
*
*/
#ifndef _MFCD_h
#define _MFCD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#include "NMStatus.h"
#include <TFT_eSPI.h>

constexpr byte MFCDLEDPin = 27;
constexpr byte InitialMFCDBrightness = 0x7F;

//constexpr byte DefaultLOSBSensePin = 0x22;			// GPIO34	(ADC1 CH6)
//constexpr byte DefaultROSBSensePin = 0x23;			// GPIO35	(ADC1 CH7)
//
//constexpr uint8_t LOSB_NUM_BUTTONS = 4;
//constexpr uint8_t LOSB_NUM_LEVELS = LOSB_NUM_BUTTONS + 1;
//constexpr uint16_t LOSB_LEVELS[LOSB_NUM_LEVELS] =
//{
//  0,
//  1090,
//  1680,
//  2330,
//  3100
//};
//
//constexpr uint8_t ROSB_NUM_BUTTONS = 4;
//constexpr uint8_t ROSB_NUM_LEVELS = ROSB_NUM_BUTTONS + 1;
//constexpr uint16_t ROSB_LEVELS[ROSB_NUM_LEVELS] =
//{
//  0,
//  1250,
//  1920,
//  2650,
//  3440
//};

#include "MFCDPage.h"

class MFCDClass
{
public:
	enum PageIDs
	{
		NAV,
		COM,
		SYS,
		DBG,

		NONE
	};

protected:
	TFT_eSPI tft = TFT_eSPI();
	byte Brightness = InitialMFCDBrightness;

	MFCDPageClass* Pages[PageIDs::NONE];
	MFCDPageClass* lastPage = nullptr;
	MFCDPageClass* currentPage = nullptr;

	//OSBSet LOSBs;
	//OSBSet ROSBs;

public:
	bool Init();
	void Update();
	void Control(NMCommands::Commands command);
	void ActivatePage(PageIDs page);
	void SetBrightness(byte brightness);

};

extern MFCDClass MFCD;

#endif

