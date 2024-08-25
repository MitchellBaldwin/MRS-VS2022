/*
 Name:		MRSRCPCM.ino
 Created:	8/25/2024 3:40:41 PM
 Author:	Mitchell Baldwin
 Main executable for the Mobile Robot System Remote Controller Power Control Module (MRS RC PCM)

 Controller: LilyGo T-Display-S3

 Configuration notes:
	Board: LilyGo T-Display-S3
	TFT_eSPI setup: #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>

 v1.0	Initial release
 v1.1	
 v1.2
 v1.3

 */

#include <I2CBus.h>
#include <TaskScheduler.h>
//#include <TaskSchedulerDeclarations.h>
//#include <TaskSchedulerSleepMethods.h>

Scheduler mainScheduler;	// Main task scheduler

constexpr int HeartbeatLEDPin = GPIO_NUM_16;
constexpr auto NormalHeartbeatLEDToggleInterval = 1000;		// 1 s on, 1 s off; indicates normal heartbeat
constexpr auto NoSerialHeartbeatLEDToggleInterval = 500;

void ToggleBuiltinLEDCallback();
Task taskToggleBuiltinLED((NormalHeartbeatLEDToggleInterval * TASK_MILLISECOND), TASK_FOREVER, &ToggleBuiltinLEDCallback, &mainScheduler, false);

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup()
{
	pinMode(HeartbeatLEDPin, OUTPUT);

	//// Test code:
	//Serial.println("");
	//Serial.println("In setup()");

	// Display power is not eanbled by default when the board is powered through the LiPo battery connector
	//so must explicitly turn LCD power on:
	pinMode(LCD_POWER_ON, OUTPUT);
	digitalWrite(LCD_POWER_ON, HIGH);

	tft.init();
	tft.setRotation(1);
	tft.fillScreen(TFT_BLACK);
	tft.println("Serial debug port still connected?");

	//Serial.println("Serial debug port still connected?");

	I2CBus.Init(GPIO_NUM_43, GPIO_NUM_44);
	I2CBus.Scan();
	tft.println(I2CBus.Get1st6ActiveI2CAddressesString());

	taskToggleBuiltinLED.enable();

}

void loop()
{
	mainScheduler.execute();

}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(HeartbeatLEDPin, !digitalRead(HeartbeatLEDPin));
}
