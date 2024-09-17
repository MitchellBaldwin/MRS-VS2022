// MenuItem.h

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

	typedef void (*MenuItemOnExecuteHandler)(void);	// Function pointer for OnPress event handler

protected:
	uint16_t Xtl;
	uint16_t Ytl;
	uint16_t Width;
	uint16_t Height;
	String Label;
	int Value = 0;

	MenuItemOnExecuteHandler OnExecute = nullptr;

public:
	MenuItemClass(String label, uint16_t xtl, uint16_t ytl, uint16_t width, uint16_t height, MenuItemTypes menuItemType = OffOn, MenuItemOnExecuteHandler onActivate = nullptr);

	void Init();
	void Draw(TFT_eSPI* tft, bool isCurrent);

	void SetValue(int value);
	int GetValue();
	void SetOnExecuteHandler(MenuItemOnExecuteHandler onActivate);
	void InvokeOnExecuteHandler();

};

//extern MenuItemClass MenuItem;

#endif

