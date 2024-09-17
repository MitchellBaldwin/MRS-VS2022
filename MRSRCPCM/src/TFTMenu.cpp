// 
// 
// 

#include "TFTMenu.h"
#include "DEBUG Macros.h"

TFTMenuClass::TFTMenuClass(TFT_eSPI* parentTFT)
{
	Init(parentTFT);
}

void TFTMenuClass::Init(TFT_eSPI* parentTFT)
{
	for (byte i = 0; i < MAX_MENU_ITEMS; ++i)
	{
		Items[i] = nullptr;
	}
	tft = parentTFT;
}

void TFTMenuClass::Draw()
{
	//_PL(HighestItemIndex)
	for (byte i = 0; i <= HighestItemIndex; ++i)
	{
		//_PL(i)
		Items[i]->Draw(tft, (i == CurrentItemIndex) ? true : false);
	}
}

bool TFTMenuClass::AddItem(MenuItemClass* item)
{
	if (HighestItemIndex == 255)	// This is the first item added
	{
		HighestItemIndex = 0;
		Items[HighestItemIndex] = item;
		return true;
	}
	if (HighestItemIndex < MAX_MENU_ITEMS - 1)
	{
		// OK to add a new menu item:
		Items[++HighestItemIndex] = item;
		return true;
	}
	else
	{
		return false;
	}
}

bool TFTMenuClass::AddItemAt(MenuItemClass* item, byte index)
{
	if (index >= MAX_MENU_ITEMS)
	{
		return false;
	}
	else
	{
		Items[index] = item;
		return true;
	}
}

MenuItemClass* TFTMenuClass::GetCurrentItem()
{
	if (CurrentItemIndex == 255)
	{
		return nullptr;
	}
	return Items[CurrentItemIndex];
}

MenuItemClass* TFTMenuClass::PrevItem()
{
	MenuItemClass* item = Items[CurrentItemIndex];
	item->Draw(tft, false);
	//_PL(CurrentItemIndex)
	if (CurrentItemIndex == 0)
	{
		CurrentItemIndex = HighestItemIndex;
	}
	else
	{
		--CurrentItemIndex;
	}
	//_PL(CurrentItemIndex)
	item = Items[CurrentItemIndex];
	item->Draw(tft, true);
	return item;
}

MenuItemClass* TFTMenuClass::NextItem()
{
	MenuItemClass* item = Items[CurrentItemIndex];
	item->Draw(tft, false);
	if (++CurrentItemIndex > HighestItemIndex)
	{
		CurrentItemIndex = 0;
	}
	item = Items[CurrentItemIndex];
	item->Draw(tft, true);
	return item;
}

void TFTMenuClass::ActivateCurrentItem()
{
}

void TFTMenuClass::ExecuteCurrentItem()
{
	Items[CurrentItemIndex]->InvokeOnExecuteHandler();
}

