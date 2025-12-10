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

//#include <TinyStepper_28BYJ_48.h>
//constexpr int IN1 = 0;
//constexpr int IN2 = 1;
//constexpr int IN3 = 2;
//constexpr int IN4 = 3;
//constexpr int StepsPerRev = 2048;
//TinyStepper_28BYJ_48 SensorTurretMotor;

constexpr int DefaultA0Pin = GPIO_NUM_1;
constexpr int DefaultI2C1SDA = GPIO_NUM_2;			// Slave SDA pin
constexpr int DefaultI2C1SCL = GPIO_NUM_3;			// Slave SCL pin
constexpr int DefaultFwdNeoPixelPin = GPIO_NUM_4;
//constexpr int DefaultSDA = SDA;					// GPIO_NUM_5; default SDA pin declared in I2CBus.h
//constexpr int DefaultSCL = SCL;					// GPIO_NUM_6; default SCL pin declared in I2CBus.h
constexpr int DefaultTurretStepPin = GPIO_NUM_7;	// TMC2209 Step pin
constexpr int DefaultTurretDirPin = GPIO_NUM_8;		// TMC2209 Direction pin
constexpr int DefaultTurretEnablePin = GPIO_NUM_9;	// TMC2209 Enable pin (Low active); optionally can be connected to GND for always enabled

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
#include <Wire.h>
TwoWire MCCI2CBus = TwoWire(1);			// MCC master I2C bus 
constexpr uint8_t MCCI2CAddress = 0x08;	// MRS SEN I2C address on MCC bus
void MCCI2CReceiveEvent(int numBytes);
void MCCI2CRequestEvent();

#include "src/MRSSENStatus.h"
#include "src/MRSSENLocDisplay.h"
#include "src/MRSNavSensors.h"

//#include <AVRStepperPins.h>
//#include <common.h>
#include <FastAccelStepper.h>
//#include <PoorManFloat.h>
//#include <RampCalculator.h>
//#include <RampConstAcceleration.h>
//#include <RampGenerator.h>
//#include <StepperISR.h>
//#include <test_probe.h>

FastAccelStepperEngine fasEngine = FastAccelStepperEngine();
FastAccelStepper* SensorTurretMotor = NULL;

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

	// Initialize MCC I2C bus:
	MCCI2CBus.onReceive(MCCI2CReceiveEvent);								// Register event handler for receiving commands from MCC
	MCCI2CBus.onRequest(MCCI2CRequestEvent);								// Register handler for MCC data requests
	MCCI2CBus.begin(MCCI2CAddress, DefaultI2C1SDA, DefaultI2C1SCL, 100000);	// Set up I2C1 (Wire1) as I2C slave at 100 kHz
	//TODO: Verify MCC I2C bus initialization success:
	//if (true)
	//{
	//	_PL("MCC I2C bus initialized successfully");
	//}
	//else
	//{
	//	HeartbeatLEDTogglePeriod = ErrorHeartbeatLEDToggleInterval;
	//	_PL("Error initializing MCC I2C bus...");
	//}

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

	fasEngine.init();
	SensorTurretMotor = fasEngine.stepperConnectToPin(DefaultTurretStepPin);
	if (SensorTurretMotor == NULL)
	{
		_PL("Error initializing sensor turret motor...");
	}
	else
	{
		SensorTurretMotor->setDirectionPin(DefaultTurretDirPin, true, 0);	// true: High counts up (clockwise)
		SensorTurretMotor->setEnablePin(DefaultTurretEnablePin, true);		// Low active enables TMC2209
		SensorTurretMotor->setAutoEnable(true);
		_PL("Sensor turret motor initialized successfully");

		SensorTurretMotor->setSpeedInHz(500);
		SensorTurretMotor->setAcceleration(100);
		//SensorTurretMotor->runForward();
		_PL("Rotating sensor turret motor 1 revolution clockwise");
		SensorTurretMotor->moveTo(1600, true);	// MS1 & MS2 set for 1/8 microstepping => 1600 steps/rev
		_PL("Rotating sensor turret motor 1 revolution counter-clockwise");
		SensorTurretMotor->moveTo(0, true);
		_PL("Sensor turret motor test complete");
	}

	//// Test code:
	//SensorTurretMotor.connectToPins(IN1, IN2, IN3, IN4);	// ccw reverse, cw forward
	////SensorTurretMotor.connectToPins(IN1, IN2, IN4, IN3);	// cw reverse, no forward
	////SensorTurretMotor.connectToPins(IN1, IN3, IN2, IN4);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN1, IN3, IN4, IN2);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN1, IN4, IN3, IN2);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN2, IN1, IN3, IN4);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN2, IN1, IN4, IN3);	// cw reverse, no forward
	////SensorTurretMotor.connectToPins(IN2, IN3, IN1, IN4);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN2, IN3, IN4, IN1);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN2, IN4, IN1, IN3);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN2, IN4, IN3, IN1);	// cw reverse, no forward
	////SensorTurretMotor.connectToPins(IN3, IN1, IN2, IN4);	// cw reverse, no forward
	////SensorTurretMotor.connectToPins(IN3, IN1, IN4, IN2);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN3, IN2, IN1, IN4);	// cw reverse, no forward
	////SensorTurretMotor.connectToPins(IN3, IN2, IN4, IN1);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN3, IN4, IN1, IN2);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN3, IN4, IN2, IN1);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN4, IN1, IN2, IN3);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN4, IN1, IN3, IN2);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN4, IN2, IN1, IN3);	// no reverse, cw forward
	////SensorTurretMotor.connectToPins(IN4, IN2, IN3, IN1);	// no reverse, no forward
	////SensorTurretMotor.connectToPins(IN4, IN3, IN1, IN2);	// cw reverse. no forward
	////SensorTurretMotor.connectToPins(IN4, IN3, IN2, IN1);	// cw reverse, no forward
	//SensorTurretMotor.setSpeedInStepsPerSecond(256);
	//SensorTurretMotor.setAccelerationInStepsPerSecondPerSecond(512);
	//_PL("Reverse direction")
	//SensorTurretMotor.moveRelativeInSteps(-2048);
	//delay(1000);
	//_PL("Forward direction")
	//SensorTurretMotor.moveRelativeInSteps(2048);
	//_PL("Motor test complete")

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

void MCCI2CReceiveEvent(int numBytes)
{
	uint8_t data[numBytes];
	// Currently only CSSMCommandPacket messages are expected from the MCC:
	if (numBytes != sizeof(mrsSENStatus.cssmCommandPacket))
	{
		_PL("MCCI2CReceiveEvent: Unexpected command packet size");
		return;
	}
	MCCI2CBus.readBytes(data, numBytes);	// Read incomming command packet
	memcpy(&mrsSENStatus.cssmCommandPacket, data, sizeof(mrsSENStatus.cssmCommandPacket));
	if (mrsSENStatus.cssmCommandPacket.command == mrsSENStatus.cssmCommandPacket.SetTurretPosition)
	{
		_PL("MCCI2CReceiveEvent: Received SetTurretPosition command from MCC");
		if (SensorTurretMotor != NULL)
		{
			_PP("Moving sensor turret to target position: ");
			_PL(mrsSENStatus.cssmCommandPacket.turretPosition);
			SensorTurretMotor->moveTo(mrsSENStatus.cssmCommandPacket.turretPosition, true);
		}
		else
		{
			_PL("Error: SensorTurretMotor not initialized");
		}
	}
}

void MCCI2CRequestEvent()
{
	MCCI2CBus.slaveWrite((uint8_t*)&mrsSENStatus.mrsSensorPacket, sizeof(MRSSENStatus::mrsSensorPacket));
}
