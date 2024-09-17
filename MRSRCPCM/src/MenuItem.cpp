// 
// 
// 

#include "MenuItem.h"

MenuItemClass::MenuItemClass(String label, uint16_t xtl, uint16_t ytl, uint16_t width, uint16_t height, MenuItemTypes menuItemType, MenuItemOnExecuteHandler onActivate)
{
	MenuItemType = menuItemType;
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
	char buf[8];
	// Erase background:
	tft->fillRect(Xtl, Ytl, Width, Height, TFT_BLACK);

	// Enclose in a box if this item is the current item:
	if (isCurrent)
	{
		tft->drawRect(Xtl, Ytl, Width, Height, TFT_LIGHTGREY);
	}
	else
	{
		tft->drawRect(Xtl, Ytl, Width, Height, TFT_BLACK);
	}
	
	// Draw label:
	tft->setTextColor(TFT_YELLOW, TFT_BLACK, true);
	tft->setTextDatum(TL_DATUM);
	tft->setTextSize(1);
	tft->drawString(Label, Xtl+2, Ytl+2);

	// Draw value, depending on type:
	switch (MenuItemType)
	{
	case Action:
		tft->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		tft->setTextDatum(TR_DATUM);
		tft->setTextSize(1);
		tft->drawString("->", Xtl + Width - 2, Ytl + 2);
		break;
	case OffOn:
		tft->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		tft->setTextDatum(TR_DATUM);
		tft->setTextSize(1);
		tft->drawString(Value ? "ON" : "OFF", Xtl + Width - 2, Ytl + 2);
		break;
	case Numeric:
		tft->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		tft->setTextDatum(TR_DATUM);
		tft->setTextSize(1);
		sprintf(buf, "%03D", Value);
		tft->drawString(buf, Xtl + Width - 2, Ytl + 2);
		break;
	case OptionList:

		break;
	default:
		break;
	}
}

void MenuItemClass::SetValue(int value)
{
	Value = value;
}

int MenuItemClass::GetValue()
{
	return Value;
}

void MenuItemClass::SetOnExecuteHandler(MenuItemOnExecuteHandler onActivate)
{
	OnExecute = onActivate;
}

void MenuItemClass::InvokeOnExecuteHandler()
{
	if (OnExecute != nullptr)
	{
		OnExecute();
	}
}


//MenuItemClass MenuItem;

