/*
 Name:		MRSSENXIAOS3.ino
 Created:	3/17/2025 3:32:36 PM
 Author:	Mitchell Baldwin
*/

//#include <gfxfont.h>
//#include <Adafruit_SPITFT_Macros.h>
//#include <Adafruit_SPITFT.h>
//#include <Adafruit_GrayOLED.h>
#include <Adafruit_GFX.h>
//#include <splash.h>
#include <Adafruit_SSD1306.h>
constexpr uint8_t DefaultOLEDAddress = 0x3C;
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET = -1;		// Reset pin # (or -1 if sharing Arduino reset pin)

#include "src/DEBUG Macros.h"
#include <I2CBus.h>

#include <SparkFun_Qwiic_OTOS_Arduino_Library.h>
QwiicOTOS* OTOS;

void setup()
{
	char buf[64];
	
	Serial.begin(115200);
	_PL("")
	_PL("Initializing MRS Sensor module")
	
	//pinMode(BUILTIN_LED, OUTPUT);
	//digitalWrite(BUILTIN_LED, HIGH);

	// Initialize I2C bus:
	if (I2CBus.Init(DefaultSDA, DefaultSCL))
	{
		I2CBus.Scan();
		_PL(I2CBus.GetActiveI2CAddressesString());
	}
	else
	{
		_PL("Error initializing I2C bus...");
	}

	// Initialize local display:
	Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, DefaultOLEDAddress))
	{
#ifdef _DEBUG_
		_PP(F("\nSSD1306 initialization failed; "));
		snprintf(buf, 31, "Firmware version: %02d.%02d", 1, 0);
		_PL(buf);
#endif
	}
	display.clearDisplay();
	display.setCursor(0, 0);
	display.cp437();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	
	//// If all the characters will not fit on the display then the
	//// library will draw what it can and the rest will be clipped.
	//int16_t charCode = 0x30;				// ASCII '0'
	//for (int16_t i = 0x00; i < 0x10; i++)
	//	for (int16_t j = 0x00; j < 0x0A; j++)
	//	{
	//		charCode = j + 0x30;
	//		if (charCode == '\n')	display.write(' ');
	//		else					display.write(charCode);
	//	}
	
	display.write("MRS Sensors");

	display.setCursor(0, 10);
	OTOS = new QwiicOTOS();
	OTOS->begin();
	sfe_otos_version_t hwVersion;
	sfe_otos_version_t fwVersion;
	OTOS->getVersionInfo(hwVersion, fwVersion);
	sprintf(buf, "OTOS HWv%X.%X FWv%X.%X", hwVersion.major, hwVersion.minor, fwVersion.major, fwVersion.minor);
	display.write(buf);

	display.display();

}

void loop()
{
	
}
