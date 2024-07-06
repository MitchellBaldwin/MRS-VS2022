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

#include "NMStatus.h"
#include <TFT_eSPI.h>

constexpr byte MFCDLEDPin = 27;
constexpr byte InitialMFCDBrightness = 127;

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
	TFT_eSPI* tft = new TFT_eSPI();
	MFCDPageClass* Pages[PageIDs::NONE];
	MFCDPageClass* lastPage = nullptr;
	MFCDPageClass* currentPage = nullptr;

public:
	bool Init();
	void Update();
	void Control();
	void ActivatePage(PageIDs page);
	
};

extern MFCDClass MFCD;

#endif

