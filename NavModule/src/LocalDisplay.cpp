/*	LocalDisplayClass - Class for implementing paged graphical display interface for
*	ESP32 based MRS RC Navigation Module
*
*	Mitchell Baldwin copyright 2023-2024
*/

#include "LocalDisplay.h"
#include "I2CBus.h"
#include "NMStatus.h"
#include "NMControls.h"

#include <WiFi.h>

void LocalDisplayClass::DrawPageHeaderAndFooter()
{

	tft.fillScreen(TFT_BLACK);

	tft.setTextSize(1);
	tft.setTextColor(TFT_BLUE, TFT_BLACK, false);
	tft.setTextDatum(TL_DATUM);
	sprintf(buf, "MRS RC NM v%d.%d", NMStatus.MajorVersion, NMStatus.MinorVersion);
	tft.drawString(buf, 2, 2, 1);

	tft.setTextDatum(TR_DATUM);
	sprintf(buf, "%s", PageTitles[currentPage]);
	tft.drawString(buf, tft.width() - 2, 2);

	// Draw LOSB labels:
	tft.setTextDatum(CL_DATUM);
	tft.setTextColor(TFT_GREENYELLOW);
	tft.drawString("NAV", 0, 50, 2);
	tft.drawString("COM", 0, 120, 2);
	tft.drawString("SYS", 0, 190, 2);
	tft.drawString("DBG", 0, 260, 2);

	// Draw footer:
	tft.setTextDatum(BC_DATUM);
	tft.setTextColor(TFT_MAGENTA, TFT_BLACK, false);
	sprintf(buf, "%s", PageMenus[currentPage]);
	tft.drawString(buf, tft.width() / 2, tft.height() - 2);

	////Test code:
	//TestFonts();
	
	//// Draw rectangle at screen bounds to aid framing physical display to panel:
	//tft.drawRect(0, 0, tft.getViewportWidth(), tft.getViewportHeight(), TFT_DARKCYAN);
}

void LocalDisplayClass::DrawSYSPage()
{
	currentPage = SYS;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		tft.setTextSize(1);

		tft.setTextColor(TFT_PINK);
		tft.setTextDatum(TL_DATUM);
		tft.drawString(ComModeHeadings[NMStatus.ComMode], 40, 50);

		tft.setTextColor(TFT_LIGHTGREY);
		sprintf(buf, "UART0 %s", NMStatus.UART0Status ? "OK" : "NO");
		tft.drawString(buf, 40, 60);

		tft.setTextDatum(TR_DATUM);
		sprintf(buf, "UART2 %s", NMStatus.UART2Status ? "OK" : "NO");
		tft.drawString(buf, tft.width() - 40, 60);

		tft.setTextDatum(TL_DATUM);
		tft.setTextColor(TFT_ORANGE);
		sprintf(buf, "WiFi %s", NMStatus.WiFiStatus ? "OK" : "NO");
		tft.drawString(buf, 40, 70);

		tft.setTextColor(TFT_GREENYELLOW);
		uint8_t mac[6];
		WiFi.macAddress(mac);
		sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		tft.drawString(buf, 40, 80);

		tft.setTextColor(TFT_GREEN);
		sprintf(buf, "IP: %s", WiFi.localIP().toString());
		tft.drawString(buf, 40, 90);

		tft.setTextColor(TFT_CYAN);
		tft.drawString(I2CBus.Get1st6ActiveI2CAddressesString(), 40, 100);

		// Draw ROSB (page-specific) labels:
		tft.setTextDatum(CR_DATUM);
		tft.setTextColor(TFT_GREENYELLOW);
		tft.drawString("POW", tft.width() - 1, 50, 2);	// Power
		tft.drawString("CSS", tft.width() - 1, 120, 2);	// Control Stick Steering
		tft.drawString("MOD", tft.width() - 1, 190, 2);	// Drive Modes
		tft.drawString("ORD", tft.width() - 1, 260, 2);	// Drive Override

		lastPage = currentPage;
	}

	// Update dynamic displays:

	tft.setCursor(100, tft.height() / 2);
	tft.setTextSize(2);
	tft.setTextColor(TFT_YELLOW);
	tft.setTextDatum(CL_DATUM);
	tft.print("5.00 V");

	sprintf(buf, "HDG: %03D", NMControls.HDGSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
	tft.setTextDatum(BR_DATUM);
	tft.drawString(buf, tft.width() - 1, tft.height() - 1);
	if (NMControls.HDGSelected)
	{
		tft.drawRect(tft.width() - tft.textWidth(buf) - 2, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_GOLD);
	}
	else
	{
		tft.drawRect(tft.width() - tft.textWidth(buf) - 2, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_BLACK);
	}

	sprintf(buf, "CRS: %03D", NMControls.CRSSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft.setTextDatum(BL_DATUM);
	tft.drawString(buf, 1, tft.height() - 1);
	if (NMControls.CRSSelected)
	{
		tft.drawRect(0, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_GOLD);
	}
	else
	{
		tft.drawRect(0, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_BLACK);
	}

	sprintf(buf, "BRT: %03D", NMControls.BRTSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft.setTextDatum(TL_DATUM);
	tft.drawString(buf, 40, 16);

	sprintf(buf, "TRR: %03D", NMControls.TRRSetting);
	tft.setTextSize(1);
	tft.setTextColor(TFT_SILVER, TFT_BLACK, true);
	tft.setTextDatum(TR_DATUM);
	tft.drawString(buf, tft.width() - 40, 16);

	// Test OSB arrays:
	//DONE: Sset up OSB arrays either in the main NavModule code block or in NMControls
	tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	tft.setTextDatum(CL_DATUM);
	sprintf(buf, "LOSB: %04D", NMControls.LeftOSBADCReading);
	tft.drawString(buf, 2, tft.height() - 24);
	sprintf(buf, "ROSB: %04D", NMControls.RightOSBADCReading);
	tft.drawString(buf, tft.width() - tft.textWidth(buf), tft.height() - 24);

}

void LocalDisplayClass::DrawNAVPage()
{
	currentPage = NAV;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();


		lastPage = currentPage;
	}

	// Update dynamic displays:

}

void LocalDisplayClass::DrawCOMPage()
{
	currentPage = COM;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();


		lastPage = currentPage;
	}

	// Update dynamic displays:

}

void LocalDisplayClass::DrawDBGPage()
{
	currentPage = DBG;

	if (lastPage != currentPage)
	{
		// Clear display and redraw static elements of the page format:
		DrawPageHeaderAndFooter();

		lastPage = currentPage;
	}

	// Update dynamic displays:

}

void LocalDisplayClass::DrawNONEPage()
{
	currentPage = NONE;
	tft.fillScreen(TFT_BLACK);
	lastPage = NONE;
}

bool LocalDisplayClass::Init()
{
	pinMode(LEDPin, OUTPUT);
	SetBrightness(Brightness);
	
	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	Control(LocalDisplayClass::SYSPage);

	return true;
}

void LocalDisplayClass::SetBrightness(byte brightness)
{
	NMControls.SetLocalDisplayBrightness(brightness);
	//Brightness = brightness;
	
	//TODO: Try using ledc library
	analogWrite(LEDPin, Brightness);
}

bool LocalDisplayClass::TestFonts()
{
	//TODO: change to display the font character set as a matrix
	//char upArrow[1] = { 0x18 };
	//char downArrow[1] = { 0x19 };
	
	tft.setTextDatum(TL_DATUM);
	tft.setTextColor(TFT_GOLD, TFT_BLACK, true);
	for (byte i = 0; i <= MaxFonts; ++i)
	{
		tft.setTextSize(1);
		sprintf(buf, "Font %d", i);
		tft.drawString(buf, 1, 50 + 20 * i, i);

	}
	
	return true;
}

void LocalDisplayClass::Update()
{
	Brightness = NMControls.BRTSetting;
	SetBrightness(Brightness);
	
	switch (currentPage)
	{
	case SYS:
		DrawSYSPage();
		break;
	case NAV:
		DrawNAVPage();
		break;
	case COM:
		DrawCOMPage();
		break;
	case DBG:
		DrawDBGPage();
		break;

	default:
		DrawNONEPage();
	}
}

void LocalDisplayClass::Control(uint8_t command)
{
	if (NMControls.NewLOSBPress)
	{
		switch (NMControls.NewLOSBKeyWasPressed())
		{
		case OSBArrayClass::OSBs::OSB4:
			command = NAVPage;
			break;
		case OSBArrayClass::OSBs::OSB3:
			command = COMPage;
			break;
		case OSBArrayClass::OSBs::OSB2:
			command = SYSPage;
			break;
		case OSBArrayClass::OSBs::OSB1:
			command = DBGPage;
			break;

		default:
			break;
		}
	}
	else if (NMControls.NewROSBPress)
	{
		switch (NMControls.NewROSBKeyWasPressed())
		{
		case OSBArrayClass::OSBs::OSB4:
			command = NAVPage;
			break;
		case OSBArrayClass::OSBs::OSB3:
			command = COMPage;
			break;
		case OSBArrayClass::OSBs::OSB2:
			command = SYSPage;
			break;
		case OSBArrayClass::OSBs::OSB1:
			command = DBGPage;
			break;

		default:
			break;
		}
	}

	switch (command)
	{
	case NoCommand:
		Update();
		break;
	case Clear:
		DrawNONEPage();
		break;
	case SYSPage:
		DrawSYSPage();
		break;
	case NAVPage:
		DrawNAVPage();
		break;
	case COMPage:
		DrawCOMPage();
		break;
	case DBGPage:
		DrawDBGPage();
		break;

	default:
		break;
	}

	if (NMControls.BRTSelected)
	{
		NMControls.ToggleBRTSelected();

		if (Brightness >= 255)
		{
			SetBrightness(0);
		}
		else if (Brightness > 191)
		{
			SetBrightness(255);
		}
		else if (Brightness > 127)
		{
			SetBrightness(192);
		}
		else if (Brightness > 63)
		{
			SetBrightness(128);
		}
		else if (Brightness >= 0)
		{
			SetBrightness(64);
		}
	}
}

LocalDisplayClass::Pages LocalDisplayClass::GetCurrentPage()
{
	return currentPage;
}

LocalDisplayClass LocalDisplay;
