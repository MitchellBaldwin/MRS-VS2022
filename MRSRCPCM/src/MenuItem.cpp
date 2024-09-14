// 
// 
// 

#include "MenuItem.h"

MenuItemClass::MenuItemClass(String label, uint16_t xtl, uint16_t ytl, uint16_t width, uint16_t height, MenuItemOnActivateHandler onActivate)
{
	Label = label;
	Xtl = xtl;
	Ytl = ytl;
	Width = width;
	Height = height;
	onActivate = onActivate;
}

void MenuItemClass::Init()
{
}

void MenuItemClass::Draw(TFT_eSPI* tft, bool isCurrent)
{
	tft->setTextColor(TFT_YELLOW, TFT_BLACK, true);
	tft->setTextDatum(TL_DATUM);
	tft->setTextSize(1);
	tft->drawString(Label, Xtl+2, Ytl+2);

}

void MenuItemClass::SetOnActivateHandler(MenuItemOnActivateHandler onActivate)
{
	OnActivate = onActivate;
}

void MenuItemClass::InvokeOnActivateHandler()
{
	if (OnActivate != nullptr)
	{
		OnActivate();
	}
}


//MenuItemClass MenuItem;

