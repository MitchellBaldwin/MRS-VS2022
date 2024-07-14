/*
 Name:		NavModule.ino
 Created:	2/1/2024 8:50:07 AM
 Author:	Mitchell Baldwin
  
 Main executable for the Mobile Robot System Remote Controller Navigation Module (MRS RC NM)

 Controller: DOIT ESP32 DEVKIT V1

 Configuration notes:
	Board:			DOIT ESP32 DEVKIT V1
	Display:		HiLetgo ILI9341 2.8" SPI TFT LCD Display Touch Panel 240X320
					http://www.lcdwiki.com/2.8inch_SPI_Module_ILI9341_SKU:MSP2807
	TFT_eSPI setup: #include <User_Setups/Setup42_ILI9341_ESP32.h>

 v1.0	Initial release
 v1.1	
 v1.2	
 v1.3	

*/
#include "src\DEBUG Macros.h"

#include <HardwareSerial.h>
HardwareSerial USBSerial(0);	// Uses the same UART device as "Serial" but does not allow use of "Serial"?
HardwareSerial IDCSerial(2);	// UART for inter-device communication & control

#include <WiFi.h>

#include "src/NMStatus.h"
#include "src/NMControls.h"

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

//#include "src\LocalDisplay.h"
#include "src\MFCD.h"

void setup()
{
	char buf[64];


//#ifdef LED_BUILTIN
//	sprintf(buf, "Heartbeat LED: GPIO 0x%02X", LED_BUILTIN);
//	_PL(buf);
//#endif // LED_BUILTIN

	//pinMode(LED_BUILTIN, OUTPUT);
	//HeartbeatLEDTask.enable();

	USBSerial.begin(115200);
	if (!USBSerial)
	{
		NMStatus.UART0Status = false;
	}
	else
	{
		NMStatus.UART0Status = true;
	}

	_PL();

	IDCSerial.begin(115200);
	if (!IDCSerial)
	{
		NMStatus.UART2Status = false;
	}
	else
	{
		NMStatus.UART2Status = true;
	}

	I2CBus.Init();
	I2CBus.Scan();
	_PL(I2CBus.GetActiveI2CAddressesString().c_str());

	NMControls.Init();
	
	//NMStatus.LocalDisplayStatus = LocalDisplay.Init();
	NMStatus.LocalDisplayStatus = MFCD.Init();
	
	ReadControlsTask.enable();
	UpdateDisplayTask.enable();

}

void loop()
{
	MainScheduler.execute();
}

// GPIO02 is in use for the TFT display board
//void ToggleBuiltinLEDCallback()
//{
//	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//}
//

void ReadControlsCallback()
{
	NMControls.Update();
	
}

void UpdateDisplayCallback()
{
	//LocalDisplay.Update();
	//LocalDisplay.Control(LocalDisplayClass::Commands::NoCommand);
	//MFCD.Update();
	MFCD.Control(NMCommands::NoCommand);
}
