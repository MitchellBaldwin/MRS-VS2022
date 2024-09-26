/*	MenuItemClass - Base class for items comprising a TFTMenu
*
*	Mitchell Baldwin copyright 2024
*
*/

#include "MenuItem.h"
#include "DEBUG Macros.h"

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

void MenuItemClass::Init(TFT_eSPI* tft)
{
	this->canvas = new TFT_eSprite(tft);
}

void MenuItemClass::Draw(TFT_eSPI* tft, bool isCurrent)
{
	char buf[8];

	canvas->createSprite(Width, Height);

	// Erase background:
	//tft->fillRect(Xtl, Ytl, Width, Height, TFT_BLACK);
	canvas->fillSprite(TFT_BLACK);

	// Enclose in a box if this item is the current item:
	uint32_t boxColor = TFT_DARKGREY;
	if (isCurrent)
	{
		boxColor = TFT_LIGHTGREY;
	}
	if (Activated)
	{
		boxColor = TFT_RED;
	}
	//tft->drawRect(Xtl, Ytl, Width, Height, boxColor);
	canvas->drawRect(0, 0, Width, Height, boxColor);

	// Draw label:

	//tft->setTextColor(TFT_YELLOW, TFT_BLACK, true);
	//tft->setTextDatum(TL_DATUM);
	//tft->setTextSize(1);
	//tft->drawString(Label, Xtl+2, Ytl+2);
	canvas->setTextColor(TFT_YELLOW, TFT_BLACK, true);
	canvas->setTextDatum(TL_DATUM);
	canvas->setTextSize(1);
	canvas->drawString(Label, 2, 2, 1);
	//canvas->drawLine(0, 0, Width, Height, TFT_WHITE);
	//_PP(Label)
	//_PP(" ")
	//_PL(canvas->textcolor)

	// Draw value, depending on type:
	switch (MenuItemType)
	{
	case Action:
		//tft->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		//tft->setTextDatum(TR_DATUM);
		//tft->setTextSize(1);
		//tft->drawString("->", Xtl + Width - 2, Ytl + 2);
		canvas->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		canvas->setTextDatum(TR_DATUM);
		canvas->setTextSize(1);
		canvas->drawString("->", Width - 2, 2);
		break;
	case OffOn:
		//tft->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		//tft->setTextDatum(TR_DATUM);
		//tft->setTextSize(1);
		//tft->drawString(Value ? "ON" : "OFF", Xtl + Width - 2, Ytl + 2);
		canvas->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		canvas->setTextDatum(TR_DATUM);
		canvas->setTextSize(1);
		canvas->drawString(Value ? "ON" : "OFF", Width - 2, 2);
		break;
	case Numeric:
		//tft->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		//tft->setTextDatum(TR_DATUM);
		//tft->setTextSize(1);
		//sprintf(buf, "%03D", Value);
		//tft->drawString(buf, Xtl + Width - 2, Ytl + 2);
		canvas->setTextColor(TFT_GREENYELLOW, TFT_BLACK, true);
		canvas->setTextDatum(TR_DATUM);
		canvas->setTextSize(1);
		sprintf(buf, "%03D", Value);
		canvas->drawString(buf, Width - 2, 2);
		break;
	case OptionList:

		break;
	default:
		break;
	}
	canvas->pushSprite(Xtl, Ytl);
	canvas->deleteSprite();

}

void MenuItemClass::Activate(bool isActivated)
{
	Activated = isActivated;
}

void MenuItemClass::SetMinValue(int minValue)
{
	MinValue = minValue;
	if (Value < MinValue)
	{
		Value = MinValue;
	}
}

void MenuItemClass::SetValue(int value)
{
	if (value < MinValue)
	{
		value = MinValue;
	}
	if (value > MaxValue)
	{
		value = MaxValue;
	}
	Value = value;
}

void MenuItemClass::SetMaxValue(int maxValue)
{
	MaxValue = maxValue;
	if (Value > MaxValue)
	{
		Value = MaxValue;
	}
}

int MenuItemClass::GetValue()
{
	return Value;
}

void MenuItemClass::SetNumericStepSize(int numericStepSize)
{
	NumericStepSize = numericStepSize;
}

int MenuItemClass::GetNumericStepSize()
{
	return NumericStepSize;
}

void MenuItemClass::SetOnExecuteHandler(MenuItemOnExecuteHandler onActivate)
{
	OnExecute = onActivate;
}

void MenuItemClass::InvokeOnExecuteHandler()
{
	if (OnExecute != nullptr)
	{
		OnExecute(Value);
	}
}

//MenuItemClass MenuItem;

