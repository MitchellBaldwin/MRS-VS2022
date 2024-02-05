/*
 Name:		NavModule.ino
 Created:	2/1/2024 8:50:07 AM
 Author:	Mitchell Baldwin
  
 Main executable for the Mobile Robot System Remote Controller Navigation Module (MRS RC NM)

 v1.0	Initial release
 v1.1	
 v1.2	
 v1.3	

*/

#include <seesaw_neopixel.h>
#include <Adafruit_seesaw.h>
constexpr byte SS_BUTTON = 0x18;
constexpr byte SS_NEOPIX = 0x06;
constexpr byte HDGEncoderI2CAddress = 0x36;
Adafruit_seesaw HDGEncoder;
seesaw_NeoPixel HDGNeoPix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
int32_t HDGSetting = 0;

#include "src/NMControls.h"
constexpr byte RightRockerSwitchPin = 0x04;		// GPIO04

#include "src/OSBArray.h"
constexpr byte LOSBAnalogPin = 34;
constexpr byte ROSBAnalogPin = 35;


#include "src/I2CBus.h"

#include <WiFi.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "src\DEBUG Macros.h"

#include <TaskSchedulerSleepMethods.h>
#include <TaskSchedulerDeclarations.h>
// #define _TASK_TIMECRITICAL		// Enable monitoring scheduling overruns
// #define _TASK_SLEEP_ON_IDLE_RUN	// Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass
// #define _TASK_STATUS_REQUEST		// Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
// #define _TASK_WDT_IDS			// Compile with support for wdt control points and task ids
// #define _TASK_LTS_POINTER		// Compile with support for local task storage pointer
// #define _TASK_PRIORITY			// Support for layered scheduling priority
// #define _TASK_MICRO_RES			// Support for microsecond resolution
// #define _TASK_STD_FUNCTION		// Support for std::function (ESP8266 and ESP32 ONLY)
// #define _TASK_DEBUG				// Make all methods and variables public for debug purposes
// #define _TASK_INLINE				// Make all methods "inline" - needed to support some multi-tab, multi-file implementations
// #define _TASK_TIMEOUT			// Support for overall task timeout
// #define _TASK_OO_CALLBACKS		// Support for dynamic callback method binding
#include <TaskScheduler.h>

Scheduler MainScheduler;			// Main loop task scheduler

//constexpr byte LED_BUILTIN = 0x02;
//constexpr auto HeartbeatLEDToggleInterval = 1000;
//void ToggleBuiltinLEDCallback();
//Task HeartbeatLEDTask(HeartbeatLEDToggleInterval* TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr uint64_t ReadControleInterval = 100;	// ms
void ReadControlsCallback();
Task ReadControlsTask(ReadControleInterval* TASK_MILLISECOND, TASK_FOREVER, &ReadControlsCallback, &MainScheduler, false);

void setup()
{
	//pinMode(LED_BUILTIN, OUTPUT);
	_PL();
	_PLH(LED_BUILTIN);

	//HeartbeatLEDTask.enable();

	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	//tft.setCursor();	// Used with 'printxx' statements; not needed when using drawString()
	tft.setTextSize(1);
	tft.setTextColor(TFT_RED, TFT_BLACK, false);
	tft.setTextDatum(TC_DATUM);
	tft.drawString("MRSRC PCM", tft.width() / 2, 2);

	tft.setCursor(10, tft.height() / 2);
	tft.setTextSize(2);
	tft.setTextColor(TFT_YELLOW);
	tft.setTextDatum(CL_DATUM);
	tft.print("5.00 V");

	tft.setCursor(2, tft.height() / 2 + 16);
	tft.setTextSize(1);
	tft.setTextColor(TFT_GREEN);
	tft.setTextDatum(CL_DATUM);
	uint8_t mac[6];
	WiFi.macAddress(mac);
	tft.printf("MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	tft.setCursor(2, tft.height() / 2 + 24);
	tft.printf("IP: %s", WiFi.localIP().toString());

	//tft.drawRect(0, 0, tft.getViewportWidth(), tft.getViewportHeight(), TFT_DARKCYAN);

	char buf[64];

	I2CBus.Init();
	I2CBus.Scan();
	snprintf(buf, 31, "I2C %02X %02X %02X %02X %02X %02X %02X %02X",
		I2CBus.ActiveI2CDeviceAddresses[0],
		I2CBus.ActiveI2CDeviceAddresses[1],
		I2CBus.ActiveI2CDeviceAddresses[2],
		I2CBus.ActiveI2CDeviceAddresses[3],
		I2CBus.ActiveI2CDeviceAddresses[4],
		I2CBus.ActiveI2CDeviceAddresses[5],
		I2CBus.ActiveI2CDeviceAddresses[6],
		I2CBus.ActiveI2CDeviceAddresses[7]);
	_PL(buf);
	tft.setCursor(2, tft.height() / 2 + 48);
	tft.printf("I2C: %s", buf);

	if (!HDGEncoder.begin(HDGEncoderI2CAddress) || !HDGNeoPix.begin(HDGEncoderI2CAddress))
	{
		sprintf(buf, "HDG encoder not found at %02X", HDGEncoderI2CAddress);
		_PL(buf);
		tft.setCursor(2, tft.height() / 2 + 56);
		tft.printf(buf);
	}
	else
	{
		sprintf(buf, "HDG encoder started at 0x%02X;", HDGEncoderI2CAddress);
		_PL(buf);
		tft.setCursor(2, tft.height() / 2 + 56);
		tft.printf(buf);
	
		uint32_t afssVersion = ((HDGEncoder.getVersion() >> 0x10) & 0xFFFF);
		sprintf(buf, "ver: %d", afssVersion);
		_PL(buf);
		tft.setCursor(3 * tft.width() / 4, tft.height() / 2 + 56);
		tft.printf(buf);

		HDGNeoPix.setPixelColor(0, HDGNeoPix.Color(0x00, 0x00, 0x08));
		//HDGNeoPix.setBrightness(0);
		HDGNeoPix.show();

		HDGEncoder.pinMode(SS_BUTTON, INPUT_PULLUP);
		HDGSetting = HDGEncoder.getEncoderPosition();

		//_PL("Enabling HDG encoder interrupts");
		//delay(10);
		//AFSS.setGPIOInterrupts((uint32_t)1 << SS_BUTTON, 1);
		//AFSS.enableEncoderInterrupt();
	}

	NMControls.Init(RightRockerSwitchPin);
	
	ReadControlsTask.enable();

}

void loop()
{
	MainScheduler.execute();
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void ReadControlsCallback()
{
	NMControls.Update();
	
	char buf[64];
	
	// Test OSB arrays:
	tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	uint16_t OSBADC = analogRead(LOSBAnalogPin);
	sprintf(buf, "LOSB: %04D", OSBADC);
	tft.drawString(buf, 2, tft.height() - 24);
	OSBADC = analogRead(ROSBAnalogPin);
	sprintf(buf, "ROSB: %04D", OSBADC);
	tft.drawString(buf, tft.width() - tft.textWidth(buf), tft.height() - 24);

	// Test AF rotary encoder & Neopixel boards:
	if (!HDGEncoder.digitalRead(SS_BUTTON))
	{
		_PL("HDG encoder button pressed");
	}
	int32_t newHDGSetting = HDGEncoder.getEncoderPosition();
	if (newHDGSetting != HDGSetting)
	{
		_PL(newHDGSetting);
		HDGSetting = newHDGSetting;
		//HDGNeoPix.setPixelColor(0, Wheel(newHDGSetting & 0xFF));
		//HDGNeoPix.show();
		sprintf(buf, "%+04D", HDGSetting);
		tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
		tft.drawString(buf, tft.width() - tft.textWidth(buf), tft.height() - 8);
	}

}

uint32_t Wheel(byte WheelPos) 
{
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return HDGNeoPix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return HDGNeoPix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return HDGNeoPix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}