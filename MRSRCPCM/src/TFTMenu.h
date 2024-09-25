/*	TFTMenu.h
*	TFTMenuClass - Base class for encoder or button driven on-screen menus
*	Container class for MenuItemClass objects
*
*	Mitchell Baldwin copyright 2024
*	v 0.00:	Initial data structure
*	v
*
*/
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
	byte CurrentItemIndex = 1;
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

