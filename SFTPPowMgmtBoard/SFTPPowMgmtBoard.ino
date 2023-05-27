/*
 Name:		SFTPPowMgmtBoard.ino
 Created:	11/16/2022 1:27:19 PM
 Author:	Mitchell Baldwin copyriht 2002
*/

// the setup function runs once when you press reset or power the board
#include <PacketSerial.h>
#include <TaskSchedulerSleepMethods.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskScheduler.h>

// Select Debug and/or Test options here
#define _DEBUG_
//#define _TEST_

#ifdef _DEBUG_
#define _PP(a) Serial.print(a);
#define _PL(a) Serial.println(a);
#define _PLH(a) Serial.println(a, HEX);
#else
#define _PP(a)
#define _PL(a)
#define _PLH(a)
#endif

constexpr auto BuiltinLEDTogglePeriod = 500;

const uint8_t COMM_BUFFER_SIZE = 0x10;
const uint8_t PACKET_SIZE = COMM_BUFFER_SIZE - 1;

uint8_t inBuffer[COMM_BUFFER_SIZE];
uint8_t outBuffer[COMM_BUFFER_SIZE];

uint8_t inPacket[PACKET_SIZE];
uint8_t outPacket[PACKET_SIZE];

PacketSerial spUSB;							// Serial port driver with COBS encoding
boolean newCommandMsgReceived = false;		// Flag indicating a new command message was received over the spUSB port
uint8_t commandMsgReceived = 0x00;			// Initialize the type of command received

// Scheduler
Scheduler mainScheduler;

void ToggleBuiltinLEDCallback();
Task taskToggleBuiltinLED(BuiltinLEDTogglePeriod * TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &mainScheduler, false);

void setup() {
	
	// Initialize encoded serial communication
	spUSB.setPacketHandler(&OnUSBPacket);
	spUSB.begin(115200);


}

// the loop function runs over and over again until power down or reset
void loop() {

	spUSB.update();		// Let PacketSerial do its thing
	mainScheduler.execute();

}

void OnUSBPacket(const uint8_t* buffer, size_t size)
{

	// Transfer incomming buffer contents (including message type and checksum)	to inPacket
	for (int i = 0; i < PACKET_SIZE; ++i)
	{
		inPacket[i] = buffer[i];
	}

	// Determine message type
	commandMsgReceived = buffer[0];

	// Set new message received flag
	newCommandMsgReceived = true;

}

void SendUSBPacket()
{
	int checksum = 0;
	for (int i = 0; i < PACKET_SIZE - 1; ++i)
	{
		checksum += outPacket[i];
	}
	outPacket[PACKET_SIZE - 1] = checksum;
	spUSB.send(outPacket, PACKET_SIZE);
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
}
