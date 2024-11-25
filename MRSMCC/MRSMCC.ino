/*
 Name:		MRSMCC.ino
 Created:	11/25/2024 1:27:41 PM
 Author:	Mitchell Baldwin	copyright 2024
 Main executable for the Mobile Robot System Master Communications Computer (MRS MCC)
 Controller: ESP32 LilyGO T-Display S3

 Configuration notes:
	Board: LilyGo T-Display-S3
	TFT_eSPI setup: #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>

 v1.0	Initial release
 v1.1

 */

#include <TaskScheduler.h>
//#include <TaskSchedulerDeclarations.h>
//#include <TaskSchedulerSleepMethods.h>
Scheduler MainScheduler;	// Main task scheduler

constexpr int HeartbeatLEDPin = GPIO_NUM_16;
constexpr auto NormalHeartbeatLEDToggleInterval = 1000;		// 1 s on, 1 s off; indicates normal heartbeat
constexpr auto NoSerialHeartbeatLEDToggleInterval = 500;

void ToggleBuiltinLEDCallback();
Task ToggleBuiltinLEDTask((NormalHeartbeatLEDToggleInterval* TASK_MILLISECOND), TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr auto LocalDisplayUpdateInterval = 100;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((LocalDisplayUpdateInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

#include "src/DEBUG Macros.h"
#include "src/MCCStatus.h"
#include "src/LocalDisplay.h"

#include <I2CBus.h>

void setup()
{
	char buf[32];

	Serial.begin(115200);
	if (!Serial)
	{
		MCCStatus.UART0Status = false;
	}
	else
	{
		MCCStatus.UART0Status = true;
	}

	_PL("");

	pinMode(HeartbeatLEDPin, OUTPUT);
	sprintf(buf, "Heartbeat LED on GPIO%02D", HeartbeatLEDPin);
	_PL(buf);

	if (I2CBus.Init(GPIO_NUM_43, GPIO_NUM_44))
	{
		I2CBus.Scan();
		_PL(I2CBus.GetActiveI2CAddressesString());
	}
	else
	{
		_PL("Error initializing I2C bus...");
	}
	//LocalDisplay.ReportHeapStatus();

	if (LocalDisplay.Init())
	{
		MCCStatus.LocalDisplayStatus = true;
		_PL("Local Display initialized successfully");
	}
	else
	{
		MCCStatus.LocalDisplayStatus = false;
		_PL("Local Display initialization FAIL");
	}
	UpdateLocalDisplayTask.enable();


	ToggleBuiltinLEDTask.enable();
}

void loop()
{
	MainScheduler.execute();

}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(HeartbeatLEDPin, !digitalRead(HeartbeatLEDPin));
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}