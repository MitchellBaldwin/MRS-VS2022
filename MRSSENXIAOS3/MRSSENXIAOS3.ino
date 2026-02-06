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

#include <Adafruit_NeoPixel.h>
constexpr uint8_t FwdNeoPixelCount = 8;
Adafruit_NeoPixel FwdNeoPixelStrip(FwdNeoPixelCount, DefaultFwdNeoPixelPin, NEO_GRB + NEO_KHZ800);

#include "C:\Repos\MRS-VS2022\MRSMCC\src\MRSSENsors.h"

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

		TestSTMotor();
	}

	// Initialize forward NeoPixel strip:
	FwdNeoPixelStrip.begin();
	FwdNeoPixelStrip.show(); // Initialize all pixels to 'off'
	_PL("Forward NeoPixel strip initialized successfully");
	// Set forward NeoPixel strip to blue:
	for (uint8_t i = 0; i < FwdNeoPixelCount; i++)
	{
		FwdNeoPixelStrip.setPixelColor(i, FwdNeoPixelStrip.Color(0, 0, 255)); // Blue
	}
	FwdNeoPixelStrip.show();


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

/// <summary>
/// Handles an I2C receive event from the MCC: validates and reads an incoming CSSMCommandPacket, updates internal status, 
/// and processes SetTurretPosition commands by moving the sensor turret if initialized.
/// </summary>
/// <param name="numBytes">The number of bytes received from the I2C bus.</param>
void MCCI2CReceiveEvent(int numBytes)
{
	char buf[64];
	uint8_t data[numBytes];

	// Currently only CSSMCommandPacket messages are expected from the MCC:
	if (numBytes != sizeof(mrsSENStatus.cssmCommandPacket))
	{
		sprintf(buf, "MCCI2CReceiveEvent: Unexpected command packet size: %d bytes", numBytes);
		_PL(buf);
		return;
	}
	MCCI2CBus.readBytes(data, numBytes);	// Read incomming command packet
	memcpy(&mrsSENStatus.cssmCommandPacket, data, sizeof(mrsSENStatus.cssmCommandPacket));
	if (mrsSENStatus.cssmCommandPacket.command == mrsSENStatus.cssmCommandPacket.SetTurretPosition)
	{
		if (SensorTurretMotor != NULL)
		{
			TestSTMotor(mrsSENStatus.cssmCommandPacket.turretPosition, false);
			mrsSENStatus.mrsSensorPacket.TurretPosition = mrsSENStatus.cssmCommandPacket.turretPosition;
			//mrsSENStatus.mrsSensorPacket.TurretPosition = SensorTurretMotor->getPositionAfterCommandsCompleted();
			sprintf(buf, "New ST Position: %d %c", mrsSENStatus.mrsSensorPacket.TurretPosition, 0xF7);
			_PL(buf);
		}
		else
		{
			_PL("Error: SensorTurretMotor not initialized");
		}
	}
	else if (mrsSENStatus.cssmCommandPacket.command == mrsSENStatus.cssmCommandPacket.GetTurretPosition)
	{
		size_t bytesWritten = 0;
		// Cannot write data here; must be done in MCCI2CRequestEvent:
		//bytesWritten = MCCI2CBus.slaveWrite((uint8_t*)&mrsSENStatus.mrsSensorPacket.TurretPosition, sizeof(int));
	}
	else if (mrsSENStatus.cssmCommandPacket.command == mrsSENStatus.cssmCommandPacket.NoCommand)
	{
		size_t bytesWritten = 0;
		// Cannot write data here; must be done in MCCI2CRequestEvent:
		//bytesWritten = MCCI2CBus.slaveWrite((uint8_t*)&mrsSENStatus.mrsSensorPacket.TurretPosition, sizeof(int));
	}
	else
	{
		_PL("MCCI2CReceiveEvent: Unknown command received from MCC");
	}
}

/// <summary>
/// Writes the global MRSSensorPacket (mrsSENStatus.mrsSensorPacket) to the MCC I2C bus using MCCI2CBus.slaveWrite. 
/// The function performs a raw byte write as an I2C slave and does not return a status.
/// This function is called in response to an I2C request event from the MCC.
/// 
/// TODO: Consider adding error handling for write failures or incomplete writes.
/// TODO: Develop individual functions to return specific sensor data on demand.
/// 
/// Note: Ensure that the MCC I2C master is prepared to read the exact size of MRSSensorPacket to avoid data corruption.
/// </summary>
void MCCI2CRequestEvent()
{
	char buf[64];

	size_t bytesWritten = 0;
	//if (MCCI2CBus.availableForWrite() < sizeof(MRSSENStatus::mrsSensorPacket))
	//{
	//	_PL("MCCI2CRequestEvent: Not enough buffer space to write sensor packet");
	//	return;
	//}

	//sprintf(buf, "MCCI2CRequestEvent: Received data request from MCC; command = %d", mrsSENStatus.cssmCommandPacket.command);
	//_PL(buf)

	if (mrsSENStatus.cssmCommandPacket.command == CSSMCommandPacket::NoCommand)
	{
		bytesWritten = MCCI2CBus.slaveWrite((uint8_t*)&mrsSENStatus.mrsSensorPacket, sizeof(MRSSensorPacket));
	}
	//SendSTPosition();

	//sprintf(buf, "Read ST Position %d (%db)", mrsSENStatus.mrsSensorPacket.TurretPosition, bytesWritten);
	//_PL(buf)
	//sprintf(buf, "Read VL53L1X Range %d mm (%db)", mrsSENStatus.mrsSensorPacket.FWDVL53L1XRange, bytesWritten);
	//_PL(buf)

}

void SendSTPosition()
{
	char buf[64];
	size_t bytesWritten = 0;
	//if (MCCI2CBus.availableForWrite() < sizeof(int))
	//{
	//	_PL("MCCI2CRequestEvent: Not enough buffer space to write ST position");
	//	return;
	//}
	bytesWritten = MCCI2CBus.slaveWrite((uint8_t*)&mrsSENStatus.mrsSensorPacket.TurretPosition, sizeof(int));
	//sprintf(buf, "Sent ST Position %d (%db) to MRS MCC", mrsSENStatus.mrsSensorPacket.TurretPosition, bytesWritten);
	//_PL(buf)
}

void SendFWDVL53L1XRange()
{
	char buf[64];
	size_t bytesWritten = 0;
	//if (MCCI2CBus.availableForWrite() < sizeof(int))
	//{
	//	_PL("MCCI2CRequestEvent: Not enough buffer space to write VL53L1X range");
	//	return;
	//}
	bytesWritten = MCCI2CBus.slaveWrite((uint8_t*)&mrsSENStatus.mrsSensorPacket.FWDVL53L1XRange, sizeof(int));
	//sprintf(buf, "Read VL53L1X Range %d mm (%db)", mrsSENStatus.mrsSensorPacket.FWDVL53L1XRange, bytesWritten);
	//_PL(buf)
}

/// <summary>
/// Tests the SensorTurretMotor by configuring it and performing one clockwise and one counter-clockwise revolution while logging progress. 
/// The moves are performed synchronously (blocking).
/// </summary>
/// <returns>true if the motor was initialized and the test moves completed; false if SensorTurretMotor was not initialized.</returns>
bool TestSTMotor()
{
	bool success = false;
	if (SensorTurretMotor != NULL)
	{
		SensorTurretMotor->setSpeedInHz(500);
		SensorTurretMotor->setAcceleration(100);
		//SensorTurretMotor->runForward();
		_PL("Rotating sensor turret motor 1 revolution clockwise");
		SensorTurretMotor->moveTo(1600, true);	// MS1 & MS2 set for 1/8 microstepping => 1600 steps/rev
		_PL("Rotating sensor turret motor 1 revolution counter-clockwise");
		SensorTurretMotor->moveTo(0, true);
		_PL("Sensor turret motor test complete");
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

/// <summary>
/// Checks that the sensor turret motor is initialized, configures its speed and acceleration, logs the action, 
/// and commands the motor to move to a given position.
/// </summary>
/// <param name="targetPosition">
/// Target position (in motor/encoder units) to move the sensor turret to.
/// </param>
/// <param name="blocking">
/// If true, request a blocking move (wait until the move completes); if false, return immediately after issuing the move command.
/// </param>
/// <returns>
/// True if the SensorTurretMotor was initialized and the move command was issued; 
/// false if the SensorTurretMotor is not initialized.
/// </returns>
bool TestSTMotor(int32_t targetPosition, bool blocking)
{
	char buf[64];

	bool success = false;
	if (SensorTurretMotor != NULL)
	{
		SensorTurretMotor->setSpeedInHz(500);
		SensorTurretMotor->setAcceleration(100);
		sprintf(buf, "Testing sensor turret motor move to position %d", targetPosition);
		_PL(buf)
		SensorTurretMotor->moveTo(targetPosition, blocking);
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}

/// <summary>
/// Moves the sensor turret to the specified target position if the turret motor is initialized.
/// </summary>
/// <param name="targetPosition">
/// The desired position for the sensor turret (int32_t).
/// </param>
/// <returns>
/// true if the move command was dispatched (SensorTurretMotor was initialized); 
/// false if the motor was not initialized and no command was issued.
/// </returns>
bool MoveST(int32_t targetPosition, bool blocking = false)
{
	bool success = false;
	if (SensorTurretMotor != NULL)
	{
		SensorTurretMotor->moveTo(targetPosition, blocking);
		success = true;
	}
	else
	{
		_PL("Error: SensorTurretMotor not initialized");
	}
	return success;
}