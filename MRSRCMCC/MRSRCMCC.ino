/*
 Name:		MRSRCMCC.ino
 Created:	11/14/2023 12:04:31 PM
 Author:	Mitchell Baldwin	copyright 2023

 Main executable for the Mobile Robot System Remote Controller Master Communications Computer (MRS RC MCC)
 Controller: ESP32 LilyGO T-Display S3

 Configuration notes:
	Board: LilyGo T-Display-S3
	TFT_eSPI setup: #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>

 v1.0	Initial release
 v1.1	

*/

//#include <DEBUG Macros.h>

#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

// Scheduler
Scheduler mainScheduler;

constexpr auto NormalHeartbeatLEDTogglePeriodMicros = 1000;		// 1 s on, 1 s off; indicates normal heartbeat
constexpr auto NoSerialHeartbeatLEDTogglePeriodMicros = 500;

void ToggleBuiltinLEDCallback();
Task taskToggleBuiltinLED(NormalHeartbeatLEDTogglePeriodMicros, TASK_FOREVER, &ToggleBuiltinLEDCallback, &mainScheduler, false);

#include <PacketSerial.h>

constexpr auto ComBufferSize = 0x10;
uint8_t comBuffer[ComBufferSize];

const uint8_t COMM_BUFFER_SIZE = 8;
const uint8_t ENCODED_PACKET_SIZE = COMM_BUFFER_SIZE - 1;
const uint8_t PACKET_SIZE = ENCODED_PACKET_SIZE - 1;
const uint8_t PACKET_PAYLOAD_SIZE = PACKET_SIZE - 1;

uint8_t inBuffer[COMM_BUFFER_SIZE];
uint8_t outBuffer[COMM_BUFFER_SIZE];

uint8_t inPacket[PACKET_SIZE];
uint8_t outPacket[PACKET_SIZE];

HardwareSerial IDCSerial(2);
PacketSerial IDCPacketSerial;

boolean newIDCPacketReceived = false;		// Flag indicating a new command message was received over the spUSB port
uint8_t idcPacketType = 0x00;				// Initialize the type of command received

void OnIDCPacket(const uint8_t* buffer, size_t size);
void SendIDCPacket();

constexpr auto HeartbeatLED = 0x02;
//constexpr auto DebugLEDPin = 0x04;

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() 
{
	int HeartbeatLEDTogglePeriod = NormalHeartbeatLEDTogglePeriodMicros;
	pinMode(HeartbeatLED, OUTPUT);

	IDCSerial.begin(115200);
	if (!IDCSerial)
	{
		HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDTogglePeriodMicros;
	}
	else
	{
		IDCPacketSerial.setStream(&IDCSerial);
		IDCPacketSerial.setPacketHandler(&OnIDCPacket);
		//_PL("IDCSerial initialized")
	}

	// Display power is not eanbled by default when the board is powered through the LiPo battery connector
	//so must explicitly turn LCD power on:
	pinMode(LCD_POWER_ON, OUTPUT);
	digitalWrite(LCD_POWER_ON, HIGH);

	tft.init();
	tft.setRotation(1);
	tft.fillScreen(TFT_BLACK);
	tft.println("Serial debug port disconnected...");

	taskToggleBuiltinLED.enable();
}

void loop() 
{
	mainScheduler.execute();
	IDCPacketSerial.update();
	if (newIDCPacketReceived)
	{
		switch (idcPacketType)
		{
		case 0:
			break;
		default:
			break;
		}
		newIDCPacketReceived = false;
	}
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(HeartbeatLED, !digitalRead(HeartbeatLED));
}

void OnIDCPacket(const uint8_t* buffer, size_t size)
{
	// Transfer incomming buffer contents (including message type and checksum)	to inPacket
	for (int i = 0; i < PACKET_SIZE; ++i)
	{
		inPacket[i] = buffer[i];
	}
	
	idcPacketType = buffer[0];	// Determine message type
	newIDCPacketReceived = true;	// Set new message received flag

	//TODO: Map byte array to appropriate (based on packet type) class or struct


}

void SendIDCPacket()
{
	int checksum = 0;
	for (int i = 0; i < PACKET_SIZE - 1; ++i)
	{
		checksum += outPacket[i];
	}
	outPacket[PACKET_SIZE - 1] = checksum;
	IDCPacketSerial.send(outPacket, PACKET_SIZE);
}
