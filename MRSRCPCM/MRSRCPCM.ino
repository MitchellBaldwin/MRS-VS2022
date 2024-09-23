/*
 Name:		MRSRCPCM.ino
 Created:	8/25/2024 3:40:41 PM
 Author:	Mitchell Baldwin
 Main executable for the Mobile Robot System Remote Controller Power Control Module (MRS RC PCM)

 Controller: LilyGo T-Display-S3

 Configuration notes:
	Board: LilyGo T-Display-S3
	TFT_eSPI setup: #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>
	Update Setup206_LilyGo_T_Display_S3.h to select correct color encoding order:
		#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue

 v1.0	Initial release
 v1.1	
 v1.2
 v1.3

 */

#include <HardwareSerial.h>
HardwareSerial IDCSerial(1);


#include <TaskScheduler.h>
//#include <TaskSchedulerDeclarations.h>
//#include <TaskSchedulerSleepMethods.h>

Scheduler MainScheduler;	// Main task scheduler

constexpr int HeartbeatLEDPin = GPIO_NUM_16;
constexpr auto NormalHeartbeatLEDToggleInterval = 1000;		// 1 s on, 1 s off; indicates normal heartbeat
constexpr auto NoSerialHeartbeatLEDToggleInterval = 500;

void ToggleBuiltinLEDCallback();
Task ToggleBuiltinLEDTask((NormalHeartbeatLEDToggleInterval * TASK_MILLISECOND), TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr auto SensorUpdateInterval = 250;
void UpdateSensorsCallback();
Task UpdateSensorsTask((SensorUpdateInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateSensorsCallback, &MainScheduler, false);

constexpr auto ControlUpdateInterval = 50;
void UpdateControlsCallback();
Task UpdateControlsTask((ControlUpdateInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateControlsCallback, &MainScheduler, false);

constexpr auto LocalDisplayUpdateInterval = 100;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((LocalDisplayUpdateInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

#include "src/DEBUG Macros.h"
#include <I2CBus.h>
#include "src/PCMSensorData.h"
#include "src/LocalDisplay.h"
#include "src/PCMControls.h"

void setup()
{
	char buf[32];

	Serial.begin(115200);
	if (!Serial)
	{
		PCMStatus.UART0Status = false;
	}
	else
	{
		PCMStatus.UART0Status = true;
	}

	_PL("");
	
	IDCSerial.begin(115200, SERIAL_8N1, 18, 17);
	if (!IDCSerial)
	{
		PCMStatus.UART1Status = false;
	}
	else
	{
		PCMStatus.UART1Status = true;

		// Test code:
		IDCSerial.println("Hello from the IDCSerial port...");
	}
	
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
	LocalDisplay.ReportHeapStatus();

	SensorData.Init();
	UpdateSensorsTask.enable();
	LocalDisplay.ReportHeapStatus();

	if (LocalDisplay.Init())
	{
		PCMStatus.LocalDisplayStatus = true;
		_PL("Local Display initialized successfully");
	}
	else
	{
		PCMStatus.LocalDisplayStatus = false;
		_PL("Local Display initialization FAIL");
	}
	UpdateLocalDisplayTask.enable();
	LocalDisplay.ReportHeapStatus();

	PCMControls.Init(LocalDisplay.GetTFT());
	PCMControls.MainMenu->Draw();
	LocalDisplay.ReportHeapStatus();

	UpdateControlsTask.enable();

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

void UpdateSensorsCallback()
{
	SensorData.Update();
}

void UpdateControlsCallback()
{
	PCMControls.Update();
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}