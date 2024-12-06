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

#include "src/RC2x15AMC.h"

#include <WiFi.h>
#include <esp_now.h>
uint8_t MRSRCCSSMMAC[] = { 0xA8, 0x42, 0xE3, 0x4A, 0x8B, 0x00 };
esp_now_peer_info_t MRSRCCSSMInfo;

#include <TaskScheduler.h>
//#include <TaskSchedulerDeclarations.h>
//#include <TaskSchedulerSleepMethods.h>
Scheduler MainScheduler;	// Main task scheduler

constexpr int HeartbeatLEDPin = GPIO_NUM_16;
constexpr auto NormalHeartbeatLEDToggleInterval = 1000;		// 1 s on, 1 s off; indicates normal heartbeat
constexpr auto NoSerialHeartbeatLEDToggleInterval = 500;

void ToggleBuiltinLEDCallback();
Task ToggleBuiltinLEDTask((NormalHeartbeatLEDToggleInterval* TASK_MILLISECOND), TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr auto UpdateLocalDisplayInterval = 100;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((UpdateLocalDisplayInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

constexpr auto UpdateMotorControllerInterval = 100;
void UpdateMotorControllerCallback();
Task GetMotContStatusTask((UpdateMotorControllerInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateMotorControllerCallback, &MainScheduler, false);

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

	// Initialize motor controller:
	MCCStatus.RC2x15AMCStatus = RC2x15AMC.Init();

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

	// Initialize ESP-NOW
	// Set device as a Wi-Fi Station; turns WiFi radio ON:
	//WiFi.mode(WIFI_STA);	// WiFi radio is already on by virtue of the call to ESP32WiFi.Init(), above
	WiFi.mode(WIFI_MODE_APSTA);
	MCCStatus.ESPNOWStatus = (esp_now_init() == ESP_OK);
	if (!MCCStatus.ESPNOWStatus) {
		_PL("Error initializing ESP-NOW")
	}
	else
	{
		_PL("Initializing ESP-NOW")

			MCCStatus.ComMode = MCCStatusClass::ComModes::ESPNOW;

		// Register OnDataSent callback
		esp_now_register_send_cb(OnMRSRCCSSMDataSent);

		// Register peer
		memcpy(MRSRCCSSMInfo.peer_addr, MRSRCCSSMMAC, 6);
		MRSRCCSSMInfo.channel = 0;
		MRSRCCSSMInfo.encrypt = false;

		// Add peer        
		if (esp_now_add_peer(&MRSRCCSSMInfo) != ESP_OK)
		{
			_PL("Failed to add peer")
				//return;
		}

		// Register OnDataReceived callback
		esp_now_register_recv_cb(esp_now_recv_cb_t(OnMRSRCCSSMDataReceived));

	}

	uint8_t mac[6];
	WiFi.macAddress(mac);
	snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	_PL(buf)

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

	GetMotContStatusTask.enable();
	if (MCCStatus.RC2x15AMCStatus)
	{
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
		UpdateMotorControllerCallback();
	}
	//GetMotContStatusTask.disable();

	ToggleBuiltinLEDTask.enable();
}

void loop()
{
	MainScheduler.execute();

}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(HeartbeatLEDPin, !digitalRead(HeartbeatLEDPin));
	
	//TODO: Determine if this is the best place to call MCCStatus.Update(), which checks health of
	//communications links and updates status flags accordingly
	MCCStatus.Update();
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}

void UpdateMotorControllerCallback()
{
	RC2x15AMC.Update();
}

void OnMRSRCCSSMDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
	MCCStatus.ESPNOWStatus = (status == ESP_NOW_SEND_SUCCESS);
	if (MCCStatus.ESPNOWStatus)
	{
		MCCStatus.CSSMPacketSentCount++;
	}
}

void OnMRSRCCSSMDataReceived(const uint8_t* mac, const uint8_t* data, int lenght)
{
	char buf[32];
	
	if (data[0] == 0x20)	// Packet type identifier for a CSSMDrivePacket
	{
		memcpy(&(MCCStatus.cssmDrivePacket), data, sizeof(MCCStatus.cssmDrivePacket));
		sprintf(buf, MACSTR, MAC2STR(mac));
		MCCStatus.IncomingCSSMPacketMACString = String(buf);
		MCCStatus.CSSMPacketReceivedCount++;

	// Test code (Move handling of CSSM command packets to the RC2x15AMC class):
		//// Check UART lint to motor controller:
		//if (!MCCStatus.RC2x15AMCStatus)
		//{
		//	// If the UART link is down then try to reset:
		//	if (!RC2x15AMC.ResetUARTLink())
		//	{
		//		// If reset appempt fails then return:
		//		return;
		//	}
		//}
		// While UART link is up pass CSSM steering commands to the motor controller:
		//if (MCCStatus.RC2x15AMCStatus)
		//{
		//	RC2x15AMC.Drive(MCCStatus.cssmDrivePacket.Throttle, MCCStatus.cssmDrivePacket.OmegaXY);
		//}
	// End of test code block

	}
}