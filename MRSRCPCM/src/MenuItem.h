/*	MenuItem.h
*	MenuItemClass - Base class for items comprising a TFTMenu
*
*	Mitchell Baldwin copyright 2024
*	v 0.00:	Initial data structure
*	v
*
*/
#ifndef _MENUITEM_h
#define _MENUITEM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <TFT_eSPI.h>

class MenuItemClass
{
public:
	enum MenuItemTypes
	{
		Action,
		OffOn,
		Numeric,
		OptionList,

		NoType
	};
	MenuItemTypes MenuItemType = Action;

	typedef void (*MenuItemOnExecuteHandler)(byte);	// Function pointer for OnPress event handler

protected:
	TFT_eSprite* canvas;
	
	uint16_t Xtl;
	uint16_t Ytl;
	uint16_t Width;
	uint16_t Height;
	
	String Label;
	bool Activated = false;
	
	int MinValue = 0;
	int Value = 0;
	int MaxValue = 1;
	
	int NumericStepSize = 1;

	MenuItemOnExecuteHandler OnExecute = nullptr;

public:
	MenuItemClass(String label, uint16_t xtl, uint16_t ytl, uint16_t width, uint16_t height, MenuItemTypes menuItemType = OffOn, MenuItemOnExecuteHandler onActivate = nullptr);

	void Init(TFT_eSPI* tft);
	void Draw(TFT_eSPI* tft, bool isCurrent);

	void Activate(bool isActivated);

	void SetMinValue(int minValue);
	void SetValue(int value);
	void SetMaxValue(int maxValue);
	int GetValue();

	void SetNumericStepSize(int numericStepSize);
	int GetNumericStepSize();

	void SetOnExecuteHandler(MenuItemOnExecuteHandler onActivate);
	void InvokeOnExecuteHandler();

};

//extern MenuItemClass MenuItem;

#endif

