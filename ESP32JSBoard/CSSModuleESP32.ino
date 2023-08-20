/*
 Name:		CSSModuleESP32.ino
 Created:	6/24/2023 3:51:52 PM
 Author:	Mitchell Baldwin copyright 2023

 v1.0	Initial release
 v1.1	Added support for ladder button arrays
 v1.2	
 v1.3	

*/

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
HardwareSerial USBSerial(0);	// Represents the same port as "Serial"

#include "src/CSSMSensorData.h"
constexpr byte KBAnalogPin = 36;

#include "OSBArray.h"
OSBArrayClass LOSBArray(KBAnalogPin);

//#include <AceButton.h>
//using ace_button::AceButton;
//using ace_button::ButtonConfig;
//using ace_button::LadderButtonConfig;
//
//static const uint8_t NUM_BUTTONS = 4;
//static AceButton LOSB1(nullptr, 0);
//static AceButton LOSB2(nullptr, 1);
//static AceButton LOSB3(nullptr, 2);
//static AceButton LOSB4(nullptr, 3);
//
//// button 4 cannot be used because it represents "no button pressed"
//static AceButton* const BUTTONS[NUM_BUTTONS] = {
//	&LOSB1, &LOSB2, &LOSB3, &LOSB4,
//};
//
//static const uint8_t NUM_LEVELS = NUM_BUTTONS + 1;
//static const uint16_t LEVELS[NUM_LEVELS] = {
//  0,		/* 0%, short to ground */
//  1165,
//  1770,
//  2400,
//  3160
//};
//
//// The LadderButtonConfig constructor binds the AceButton objects in the BUTTONS array to the LadderButtonConfig.
//static LadderButtonConfig buttonConfig(KBAnalogPin, NUM_LEVELS, LEVELS, NUM_BUTTONS, BUTTONS);
//
//// The event handler for the buttons.
//void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
//
//	// Print out a message for all events.
//	Serial.print(F("handleEvent(): "));
//	Serial.print(F("virtualPin: "));
//	Serial.print(button->getPin());
//	Serial.print(F("; eventType: "));
//	Serial.print(AceButton::eventName(eventType));
//	Serial.print(F("; buttonState: "));
//	Serial.println(buttonState);
//
//	// Control the LED only for the Pressed and Released events.
//	// Notice that if the MCU is rebooted while the button is pressed down, no
//	// event is triggered and the LED remains off.
//	switch (eventType) {
//	case AceButton::kEventPressed:
//		digitalWrite(DebugLED, 1);
//		break;
//	case AceButton::kEventReleased:
//		digitalWrite(DebugLED, 0);
//		break;
//	}
//}

#include "src/I2CBus.h"
#include "src/LocalDisplay.h"
constexpr byte LocalDisplayI2CAddress = 0x3C;

// Scheduler
Scheduler MainScheduler;

constexpr long HeartbeatLEDToggleInterval = 1000;
constexpr long NoSerialHeartbeatLEDToggleInterval = 500;

long HeartbeatLEDTogglePeriod = HeartbeatLEDToggleInterval;

void ToggleBuiltinLEDCallback();
Task HeartbeatLEDTask(HeartbeatLEDTogglePeriod* TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr long ReadSensorsInterval = 50;

void ReadSensorDataCallback();
Task ReadSensorsTask(ReadSensorsInterval* TASK_MILLISECOND, TASK_FOREVER, &ReadSensorDataCallback, &MainScheduler, false);

constexpr long UpdateLocalDisplayInterval = 100;

void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask(UpdateLocalDisplayInterval* TASK_MILLISECOND, TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

void setup()
{
	USBSerial.begin(115200);
	if (!USBSerial)
	{
		HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		HeartbeatLEDTogglePeriod = HeartbeatLEDToggleInterval;
	}

	char buf[32];
	snprintf(buf, 32, "Heartbeat LED pin 0x%02X", LED_BUILTIN);
	pinMode(LED_BUILTIN, OUTPUT);
	_PL();
	_PL(buf);
	pinMode(DebugLED, OUTPUT);
	snprintf(buf, 32, "Debug LED pin 0x%02X", DebugLED);
	_PL(buf);

	I2CBus.Init();
	I2CBus.Scan();

	if (!LocalDisplay.Init(LocalDisplayI2CAddress))
	{
		HeartbeatLEDTogglePeriod = 250;
	}
	else
	{
		LocalDisplay.Control(LocalDisplayClass::SYSPage);
	}

	HeartbeatLEDTask.setInterval(HeartbeatLEDTogglePeriod);
	HeartbeatLEDTask.enable();

	SensorData.Init(KBAnalogPin);
	ReadSensorsTask.enable();
	UpdateLocalDisplayTask.enable();

	//LOSBArray.Init(KBAnalogPin);

	//// Configure the ButtonConfig with the event handler, and enable all higher level events.
	//buttonConfig.setEventHandler(handleEvent);
	//buttonConfig.setFeature(ButtonConfig::kFeatureClick);
	////buttonConfig.setFeature(ButtonConfig::kFeatureDoubleClick);
	////buttonConfig.setFeature(ButtonConfig::kFeatureLongPress);
	////buttonConfig.setFeature(ButtonConfig::kFeatureRepeatPress);

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
	
	byte OSBPressed = LOSBArray.GetOSBPress();
	if (OSBPressed)
	{
		char buf[32];
		snprintf(buf, 32, "OSB ID 0x%02X pressed", OSBPressed);
		_PL(buf);

		switch (OSBPressed)
		{
		case OSBArrayClass::OSB1:
			LocalDisplay.Control(LocalDisplayClass::Prev);
			break;
		case OSBArrayClass::OSB2:
			LocalDisplay.Control(LocalDisplayClass::Next);
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
