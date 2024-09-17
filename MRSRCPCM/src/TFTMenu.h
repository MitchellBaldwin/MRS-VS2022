// TFTMenu.h

#ifndef _TFTMENU_h
#define _TFTMENU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MenuItem.h"
constexpr byte MAX_MENU_ITEMS = 8;

class TFTMenuClass
{
protected:
	MenuItemClass* Items[MAX_MENU_ITEMS];
	byte CurrentItemIndex = 0;
	byte HighestItemIndex = 255;
	TFT_eSPI* tft;

public:
	TFTMenuClass(TFT_eSPI* parentTFT);
	
	void Init(TFT_eSPI* parentTFT);
	void Draw();

	bool AddItem(MenuItemClass* item);
	bool AddItemAt(MenuItemClass* item, byte index);
	MenuItemClass* GetCurrentItem();
	MenuItemClass* PrevItem();
	MenuItemClass* NextItem();
	void ActivateCurrentItem();
	void ExecuteCurrentItem();

};

#endif

