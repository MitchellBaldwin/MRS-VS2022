/*
 Name:		CSSModuleESP32.ino
 Created:	6/24/2023 3:51:52 PM
 Author:	Mitchell Baldwin copyright 2023

 v1.0	Initial release
 v1.1	Added support for ladder button arrays
 v1.2	
 v1.3	

*/


#include "src/SFQJS.h"
#include "src/BME280Data.h"
#include "src/CSSMStatus.h"
#include "src/DEBUG Macros.h"

constexpr byte DebugLED = 0x04;

#include <TaskScheduler.h>
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
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

#include <HardwareSerial.h>
HardwareSerial USBSerial(0);	// Uses the same UART device as "Serial" but does not allow use of "Serial"?
HardwareSerial IDCSerial(2);	// UART for inter-device communication & control

#include "src/I2CBus.h"

#include "src/LocalDisplay.h"
constexpr byte LocalDisplayI2CAddress = 0x3C;
#include "src/DebugDisplay.h"
constexpr byte DebugDisplayI2CAddress = 0x3D;

#include "src/CSSMSensorData.h"
// TODO: If OSB arrays are powered from ESP 32 3.3 V supply then consider running signals directly to the ADC ports
// Scale and buffer VIN signal and use the VP pin (pin 36) for measurement
constexpr byte LOSBAnalogPin = 34;
constexpr byte ROSBAnalogPin = 35;
constexpr byte ESP32VINAnalogPin = 36;

#include "src/OSBArray.h"

#define _ROBOX
#ifdef _ROBOX
OSBArrayClass OSBArray(LOSBAnalogPin);
static const uint8_t OBS_NUM_BUTTONS = 8;
static const uint8_t OSB_NUM_LEVELS = OBS_NUM_BUTTONS + 1;
static const uint16_t OSB_LEVELS[OSB_NUM_LEVELS] =
{
  160,
  830,
  1100,
  1350,
  1640,
  1920,
  2240,
  2640,
  3100,
};
#elif
OSBArrayClass LOSBArray(LOSBAnalogPin);
static const uint8_t LOBS_NUM_BUTTONS = 4;
static const uint8_t LOSB_NUM_LEVELS = LOBS_NUM_BUTTONS + 1;
static const uint16_t LOSB_LEVELS[LOSB_NUM_LEVELS] = 
{
  0,		/* 0%, short to ground */
  1165,
  1770,
  2400,
  3160
};
OSBArrayClass ROSBArray(LOSBAnalogPin);
static const uint8_t ROBS_NUM_BUTTONS = 4;
static const uint8_t ROSB_NUM_LEVELS = ROBS_NUM_BUTTONS + 1;
static const uint16_tRLOSB_LEVELS[ROSB_NUM_LEVELS] = 
{
  0,		/* 0%, short to ground */
  1165,
  1770,
  2400,
  3160
};
#endif

// Scheduler
Scheduler MainScheduler;

constexpr long NormalHeartbeatLEDToggleInterval = 1000;
constexpr long NoSerialHeartbeatLEDToggleInterval = 250;
constexpr long NoLocalDisplayHeartbeatLEDToggleInterval = 100;

long HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
void ToggleBuiltinLEDCallback();
Task HeartbeatLEDTask(HeartbeatLEDTogglePeriod* TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr long ReadSensorsInterval = 50;
void ReadSensorDataCallback();
Task ReadSensorsTask(ReadSensorsInterval* TASK_MILLISECOND, TASK_FOREVER, &ReadSensorDataCallback, &MainScheduler, false);

constexpr long ReadENVDataInterval = 5000;
void ReadENVDataCallback();
Task ReadENVDataTask(ReadENVDataInterval* TASK_MILLISECOND, TASK_FOREVER, &ReadENVDataCallback, &MainScheduler, false);

constexpr long ReadControlsInterval = 50;
void ReadControlsCallback();
Task ReadControlsTask(ReadControlsInterval* TASK_MILLISECOND, TASK_FOREVER, &ReadControlsCallback, &MainScheduler, false);

constexpr long UpdateLocalDisplayInterval = 100;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask(UpdateLocalDisplayInterval* TASK_MILLISECOND, TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

void setup()
{
	USBSerial.begin(115200);
	if (!USBSerial)
	{
		CSSMStatus.UART0Status = false;
		HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		CSSMStatus.UART0Status = true;
		HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
	}

	IDCSerial.begin(115200);
	if (!IDCSerial)
	{
		CSSMStatus.UART2Status = false;
		//HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		CSSMStatus.UART2Status = true;
		//HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
	}

	char buf[32];
	snprintf(buf, 31, "Heartbeat LED pin 0x%02X", LED_BUILTIN);
	pinMode(LED_BUILTIN, OUTPUT);
	_PL();
	_PL(buf);
	pinMode(DebugLED, OUTPUT);
	snprintf(buf, 31, "Debug LED pin 0x%02X", DebugLED);
	_PL(buf);

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

#ifdef _ROBOX
	OSBArray.Init(OSB_NUM_LEVELS, OSB_LEVELS);
#elif
	LOSBArray.Init(LOSB_NUM_LEVELS, LOSB_LEVELS);
	ROSBArray.Init(ROSB_NUM_LEVELS, ROSB_LEVELS);
#endif

	if (!LocalDisplay.Init(LocalDisplayI2CAddress))
	{
		CSSMStatus.LocalDisplayStatus = false;
		HeartbeatLEDTogglePeriod = NoLocalDisplayHeartbeatLEDToggleInterval;
		snprintf(buf, 31, "Local Display not found at I2C 0x%02X", LocalDisplayI2CAddress);
		_PL(buf)
	}
	else
	{
		CSSMStatus.LocalDisplayStatus = true;
		LocalDisplay.Control(LocalDisplayClass::SYSPage);
		snprintf(buf, 31, "Local Display at I2C 0x%02X", LocalDisplayI2CAddress);
		_PL(buf)
	}

	if (!DebugDisplay.Init(DebugDisplayI2CAddress))
	{
		CSSMStatus.DebugDisplayStatus = false;
		//HeartbeatLEDTogglePeriod = NoDebugDisplayHeartbeatLEDToggleInterval;
		snprintf(buf, 31, "Debug Display not found at I2C 0x%02X", DebugDisplayI2CAddress);
		_PL(buf)
	}
	else
	{
		CSSMStatus.DebugDisplayStatus = true;
		DebugDisplay.Control(DebugDisplayClass::HOMPage);
		snprintf(buf, 31, "Debug Display at I2C 0x%02X", DebugDisplayI2CAddress);
		_PL(buf)
	}

	SensorData.Init(LOSBAnalogPin, ESP32VINAnalogPin);
	
	ReadSensorsTask.enable();
	ReadENVDataTask.enable();
	ReadControlsTask.enable();
	UpdateLocalDisplayTask.enable();

	HeartbeatLEDTask.setInterval(HeartbeatLEDTogglePeriod * TASK_MILLISECOND);
	HeartbeatLEDTask.enable();

}

void loop()
{
	MainScheduler.execute();
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void ReadSensorDataCallback()
{
	SensorData.Update();
}

void ReadENVDataCallback()
{
	SensorData.ReadENVData();
}

void ReadControlsCallback()
{
	byte OSBPressed = OSBArray.GetOSBPress();
	if (OSBPressed)
	{
		char buf[32];
		snprintf(buf, 32, "OSB ID 0x%02X pressed", OSBPressed);
		_PL(buf);

		switch (OSBPressed)
		{
		case OSBArrayClass::OSB4:
			LocalDisplay.Control(LocalDisplayClass::Next);
			break;
		case OSBArrayClass::OSB5:
			DebugDisplay.Control(DebugDisplayClass::Next);
			break;
		default:
			break;
		}
	}
	
	// Abandoned use of AceButton library due to issue with one button in a ladder array being unusable:
	//buttonConfig.checkButtons();
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}
