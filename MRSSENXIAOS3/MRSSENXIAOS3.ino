/*
 Name:		MRSSENXIAOS3.ino
 Created:	3/17/2025 3:32:36 PM
 Author:	Mitchell Baldwin	copyright 2025
 
 Main executable for the Mobile Robot System Sensors module (MRS SEN)

 Controller: XIAO ESP32-S3

 Configuration notes:
	Board: Seeed Studio XIAO ESP32-S3 on a Seeed Studio XIAO Expansion Board 
	Local display: 128x64 monochrome oLED; Adafruit GFX & SSD1306

 v1.0	Initial release
 v1.1

*/

#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>
Scheduler MainScheduler;

#include <TinyStepper_28BYJ_48.h>
constexpr int IN1 = 0;
constexpr int IN2 = 1;
constexpr int IN3 = 2;
constexpr int IN4 = 3;
constexpr int StepsPerRev = 2048;
TinyStepper_28BYJ_48 SensorTurretMotor;

constexpr int HeartbeatLEDPin = BUILTIN_LED;
constexpr auto NormalHeartbeatLEDToggleInterval = 1000;		// 1 s on, 1 s off; indicates successful initialization of all subsystems
constexpr auto ErrorHeartbeatLEDToggleInterval = 250;		// 2 Hz to indicate some initialization failure occurred

long HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
void ToggleHeartbeatLEDCallback();
Task ToggleHeartbeatLEDTask((NormalHeartbeatLEDToggleInterval * TASK_MILLISECOND), TASK_FOREVER, &ToggleHeartbeatLEDCallback, &MainScheduler, false);

long LocalDisplayStartupPeriod = 2000;	// ms
void LocalDisplayStartupCallback();
Task LocalDisplayStartupTask((LocalDisplayStartupPeriod * TASK_MILLISECOND), TASK_FOREVER, &LocalDisplayStartupCallback, &MainScheduler, false);

long UpdateLocalDisplayPeriod = 100;	// ms
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((UpdateLocalDisplayPeriod * TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

long UpdateNavSensorsPeriod = 100;	// ms
void UpdateNavSensorsCallback();
Task UpdateNavSensorsTask((UpdateNavSensorsPeriod * TASK_MILLISECOND), TASK_FOREVER, &UpdateNavSensorsCallback, &MainScheduler, false);

#include "src/DEBUG Macros.h"
#include <I2CBus.h>

#include "src/MRSSENStatus.h"
#include "src/MRSSENLocDisplay.h"
#include "src/MRSNavSensors.h"

void setup()
{
	char buf[32];
	
	Serial.begin(115200);
	_PL("")
	_PL("Initializing MRS Sensor module")
	
	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, LOW);

	// Initialize I2C bus:
	if (I2CBus.Init(DefaultSDA, DefaultSCL))
	{
		I2CBus.Scan();
		_PL(I2CBus.GetActiveI2CAddressesString());
	}
	else
	{
		HeartbeatLEDTogglePeriod = ErrorHeartbeatLEDToggleInterval;
		_PL("Error initializing I2C bus...");
	}

	// Initialize the navigation sensors:
	if (mrsNavSensors.Init())
	{
		// Enable nagigation sensor updates:
		UpdateNavSensorsTask.enable();
	}
	else
	{
		HeartbeatLEDTogglePeriod = ErrorHeartbeatLEDToggleInterval;
		_PL("Error initializing sensors...");
	}
	
	mrsSENStatus.LocDispStatus = mrsSENLocDisplay.Init();
	if (mrsSENStatus.LocDispStatus)
	{
		// Enable local display updates:
		//UpdateLocalDisplayTask.enable();
		//LocalDisplayStartupTask.setOnDisable(&LocalDisplayDisableCallback);
		LocalDisplayStartupTask.enable();
	}
	else
	{
		HeartbeatLEDTogglePeriod = ErrorHeartbeatLEDToggleInterval;
		_PL("Error initializing local display...");
	}

	ToggleHeartbeatLEDTask.setInterval(HeartbeatLEDTogglePeriod * TASK_MILLISECOND);
	ToggleHeartbeatLEDTask.enable();

	// Test code:
	SensorTurretMotor.connectToPins(IN1, IN2, IN3, IN4);	// ccw reverse, cw forward
	//SensorTurretMotor.connectToPins(IN1, IN2, IN4, IN3);	// cw reverse, no forward
	//SensorTurretMotor.connectToPins(IN1, IN3, IN2, IN4);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN1, IN3, IN4, IN2);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN1, IN4, IN3, IN2);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN2, IN1, IN3, IN4);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN2, IN1, IN4, IN3);	// cw reverse, no forward
	//SensorTurretMotor.connectToPins(IN2, IN3, IN1, IN4);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN2, IN3, IN4, IN1);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN2, IN4, IN1, IN3);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN2, IN4, IN3, IN1);	// cw reverse, no forward
	//SensorTurretMotor.connectToPins(IN3, IN1, IN2, IN4);	// cw reverse, no forward
	//SensorTurretMotor.connectToPins(IN3, IN1, IN4, IN2);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN3, IN2, IN1, IN4);	// cw reverse, no forward
	//SensorTurretMotor.connectToPins(IN3, IN2, IN4, IN1);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN3, IN4, IN1, IN2);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN3, IN4, IN2, IN1);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN4, IN1, IN2, IN3);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN4, IN1, IN3, IN2);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN4, IN2, IN1, IN3);	// no reverse, cw forward
	//SensorTurretMotor.connectToPins(IN4, IN2, IN3, IN1);	// no reverse, no forward
	//SensorTurretMotor.connectToPins(IN4, IN3, IN1, IN2);	// cw reverse. no forward
	//SensorTurretMotor.connectToPins(IN4, IN3, IN2, IN1);	// cw reverse, no forward
	SensorTurretMotor.setSpeedInStepsPerSecond(256);
	SensorTurretMotor.setAccelerationInStepsPerSecondPerSecond(512);
	_PL("Reverse direction")
	SensorTurretMotor.moveRelativeInSteps(-2048);
	delay(1000);
	_PL("Forward direction")
	SensorTurretMotor.moveRelativeInSteps(2048);
	_PL("Motor test complete")

}

void loop()
{
	MainScheduler.execute();
}

void ToggleHeartbeatLEDCallback()
{
	digitalWrite(HeartbeatLEDPin, !digitalRead(HeartbeatLEDPin));
}

void LocalDisplayStartupCallback()
{
	//_PL("Local display startup callback")
	static bool firstTime = true;
	if (firstTime)
	{
		firstTime = false;
	}
	else
	{
		LocalDisplayStartupTask.disable();
		UpdateLocalDisplayTask.enable();
	}
}

void LocalDisplayDisableCallback()
{
	//UpdateLocalDisplayTask.enable();
}

void UpdateLocalDisplayCallback()
{
	mrsSENLocDisplay.Update();
}

void UpdateNavSensorsCallback()
{
	mrsNavSensors.Update();
}
