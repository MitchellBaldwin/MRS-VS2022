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

//constexpr byte LED_BUILTIN = 0x02;
//constexpr auto HeartbeatLEDToggleInterval = 1000;

// Scheduler
Scheduler MainScheduler;

//void ToggleBuiltinLEDCallback();
//Task HeartbeatLEDTask(HeartbeatLEDToggleInterval* TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

void setup()
{
	//pinMode(LED_BUILTIN, OUTPUT);
	_PL();
	_PLH(LED_BUILTIN);

	//HeartbeatLEDTask.enable();

	tft.init();
	tft.setRotation(2);
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

	tft.drawRect(0, 0, tft.getViewportWidth(), tft.getViewportHeight(), TFT_DARKCYAN);
}

void loop()
{
	MainScheduler.execute();
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
