/*
 Name:		ESP32PowMgmtBoard.ino
 Created:	1/15/2024 4:26:28 PM
 Author:	Mitchell Baldwin copyright 2024

 Main executable for the Mobile Robot System Remote Controller Power Control Module (MRS RC PCM)

 Controller: ESP32 TTGO T-Display

 Configuration notes: 
	Board: ESP32 Dev Module
	TFT_eSPI setup: #include <User_Setups/Setup25_TTGO_T_Display.h>

 v1.0	Initial release
 v1.1	Updated I2C pin mapping
 v1.2	
 v1.3	

*/

#include <WiFi.h>

#include "src\LocalDisplay.h"

#include "src\DEBUG Macros.h"
#include "src\PCMStatus.h"


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
Scheduler MainScheduler;	// Main task scheduler

constexpr byte LED_BUILTIN = 0x02;
constexpr auto HeartbeatLEDToggleInterval = 1000;

void ToggleBuiltinLEDCallback();
Task HeartbeatLEDTask((HeartbeatLEDToggleInterval * TASK_MILLISECOND), TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

#include <HardwareSerial.h>
HardwareSerial USBSerial(0);				// Uses the same UART device as "Serial" but does not allow use of "Serial"?
HardwareSerial IDCSerial(2);				// UART for inter-device communication & control

constexpr byte UART2RX = 0x19;				// GPIO25; IDCSerial RX pin
constexpr byte UART2TX = 0x1A;				// GPIO26; IDCSerial TX pin

constexpr auto SensorUpdateInterval = 1000;
void UpdateSensorsCallback();
Task UpdateSensorsTask((SensorUpdateInterval * TASK_MILLISECOND), TASK_FOREVER, &UpdateSensorsCallback, &MainScheduler, false);

constexpr auto LocalDisplayUpdateInterval = 1000;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((LocalDisplayUpdateInterval * TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

#include "src\I2CBus.h"
#include "PCMSensorData.h"

void setup()
{
	char buf[32];
	
	USBSerial.begin(115200);
	if (!USBSerial)
	{
		PCMStatus.UART0Status = false;
		//HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		PCMStatus.UART0Status = true;
		//HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
	}

	IDCSerial.begin(115200, SERIAL_8N1, UART2RX, UART2TX);
	if (!IDCSerial)
	{
		PCMStatus.UART2Status = false;
		//HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		PCMStatus.UART2Status = true;
		//HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
	}

	//Test code:
	IDCSerial.println("TTGO T Display UART2 test");

	_PL();

	pinMode(LED_BUILTIN, OUTPUT);
	sprintf(buf, "Heartbeat LED on GPIO%02X", LED_BUILTIN);
	_PL(buf);
	HeartbeatLEDTask.enable();

	// Default SCL definition is: static const uint8_t SCL = 22
	// Using the TTGO T-Display pin 22 is used for the built-in LED, so we must re-map SCL to another pin
	// GPIO26 is the default UART2TX pin, though, so consider using GPIO27 instead once the hardwars can
	//be updated
	if (I2CBus.Init(GPIO_NUM_21, GPIO_NUM_26))
	{
		I2CBus.Scan();
		_PL(I2CBus.GetActiveI2CAddressesString().c_str());
	}
	else
	{
		_PL("Error initializing I2C bus...");
	}

	SensorData.Init();
	UpdateSensorsTask.enable();

	if (LocalDisplay.Init())
	{
		PCMStatus.LocalDisplayStatus = true;
		_PP("Local Display initialized successfully");
	}
	else
	{
		PCMStatus.LocalDisplayStatus = false;
		_PP("Local Display initialization FAIL");
	}
	UpdateLocalDisplayTask.enable();

}

void loop() 
{
	MainScheduler.execute();
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void UpdateSensorsCallback()
{
	SensorData.Update();
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}