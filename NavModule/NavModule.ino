/*
 Name:		NavModule.ino
 Created:	2/1/2024 8:50:07 AM
 Author:	Mitchell Baldwin
  
 Main executable for the Mobile Robot System Remote Controller Navigation Module (MRS RC NM)

 Controller: DOIT ESP32 DEVKIT V1

 Configuration notes:
	Board: DOIT ESP32 DEVKIT V1
	TFT_eSPI setup: #include <User_Setups/Setup42_ILI9341_ESP32.h>

 v1.0	Initial release
 v1.1	
 v1.2	
 v1.3	

*/
#include "src\DEBUG Macros.h"

#include <WiFi.h>

#include <TFT_eSPI.h>
//TFT_eSPI tft = TFT_eSPI();

#include "src/NMControls.h"
constexpr byte RightRockerSwitchPin = 0x04;		// GPIO04
constexpr byte HDGEncoderI2CAddress = 0x36;
constexpr byte CRSEncoderI2CAddress = 0x37;

#include "src/OSBArray.h"
constexpr byte LOSBAnalogPin = 34;
constexpr byte ROSBAnalogPin = 35;

#include "src/I2CBus.h"


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

constexpr uint64_t ReadControlInterval = 100;	// ms
void ReadControlsCallback();
Task ReadControlsTask((ReadControlInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadControlsCallback, &MainScheduler, false);

constexpr uint64_t UpdateDisplayInterval = 100;	// ms
void UpdateDisplayCallback();
Task UpdateDisplayTask((UpdateDisplayInterval * TASK_MILLISECOND), TASK_FOREVER, &UpdateDisplayCallback, &MainScheduler, false);

#include "src\LocalDisplay.h"

void setup()
{
	char buf[64];

	_PL();

//#ifdef LED_BUILTIN
//	sprintf(buf, "Heartbeat LED: GPIO 0x%02X", LED_BUILTIN);
//	_PL(buf);
//#endif // LED_BUILTIN

	//pinMode(LED_BUILTIN, OUTPUT);
	//HeartbeatLEDTask.enable();

	//tft.init();
	//tft.setRotation(0);
	//tft.fillScreen(TFT_BLACK);

	//tft.setTextSize(1);
	//tft.setTextColor(TFT_RED, TFT_BLACK, false);
	//tft.setTextDatum(TL_DATUM);
	//tft.drawString("MRS RC NM", 2, 2);

	//tft.setCursor(10, tft.height() / 2);
	//tft.setTextSize(2);
	//tft.setTextColor(TFT_YELLOW);
	//tft.setTextDatum(CL_DATUM);
	//tft.print("5.00 V");

	//tft.setCursor(2, tft.height() / 2 + 16);
	//tft.setTextSize(1);
	//tft.setTextColor(TFT_GREEN);
	//tft.setTextDatum(CL_DATUM);
	//uint8_t mac[6];
	//WiFi.macAddress(mac);
	//tft.printf("MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	//tft.setCursor(2, tft.height() / 2 + 24);
	//tft.printf("IP: %s", WiFi.localIP().toString());

	//tft.drawRect(0, 0, tft.getViewportWidth(), tft.getViewportHeight(), TFT_DARKCYAN);

	I2CBus.Init();
	I2CBus.Scan();
	_PL(I2CBus.GetActiveI2CAddressesString().c_str());
	//tft.setCursor(2, tft.height() / 2 + 48);
	//tft.printf(I2CBus.GetActiveI2CAddressesString().c_str());

	NMControls.Init(RightRockerSwitchPin, HDGEncoderI2CAddress, CRSEncoderI2CAddress);
	
	NMStatus.LocalDisplayStatus = LocalDisplay.Init();
	
	ReadControlsTask.enable();
	UpdateDisplayTask.enable();

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
	
	//// Test encoder buttons:
	//if (NMControls.HDGButtonWasPressed())
	//{
	//	_PL("HDG button pressed");
	//	NMControls.ToggleHDGSelected();
	//}
	//if (NMControls.CRSButtonWasPressed())
	//{
	//	_PL("CRS button pressed");
	//	NMControls.ToggleCRSSelected();
	//}
	
	//// Test OSB arrays:
	//tft.setTextColor(TFT_CYAN, TFT_BLACK, true);
	//tft.setTextDatum(CL_DATUM);
	//uint16_t OSBADC = analogRead(LOSBAnalogPin);
	//sprintf(buf, "LOSB: %04D", OSBADC);
	//tft.drawString(buf, 2, tft.height() - 24);
	//OSBADC = analogRead(ROSBAnalogPin);
	//sprintf(buf, "ROSB: %04D", OSBADC);
	//tft.drawString(buf, tft.width() - tft.textWidth(buf), tft.height() - 24);

}

void UpdateDisplayCallback()
{
	LocalDisplay.Update();

	//char buf[64];
	
	//sprintf(buf, "%03D", NMControls.HDGSetting);
	////_PL(buf);
	//tft.setTextColor(TFT_ORANGE, TFT_BLACK, true);
	//tft.setTextDatum(TL_DATUM);
	//tft.drawString(buf, tft.width() - tft.textWidth(buf) - 1, tft.height() - 9);
	//if (NMControls.HDGSelected)
	//{
	//	tft.drawRect(tft.width() - tft.textWidth(buf) - 2, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_WHITE);
	//}
	//else
	//{
	//	tft.drawRect(tft.width() - tft.textWidth(buf) - 2, tft.height() - 11, tft.textWidth(buf) + 2, 11, TFT_BLACK);
	//}
}
