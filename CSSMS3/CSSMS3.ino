/*
 Name:		CSSMS3.ino
 Created:	1/27/2025 10:12:32 AM
 Author:	Mitchell Baldwin copyright 2025

 Main executable for the Mobile Robot System Remote Controller Control Stick Steering Module (MRS RC CSSM)

 Controller: ESP32 LilyGO T-Display S3

 Configuration notes:
	Board: LilyGo T-Display-S3
	TFT_eSPI setup: #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>

 v1.0	Initial release
 v1.1	

*/

#include <DEBUG Macros.h>

#include <HardwareSerial.h>
HardwareSerial USBSerial(0);	// Uses the same UART device as "Serial" but does not allow use of "Serial"?

constexpr int8_t UART1RXPin = GPIO_NUM_18;
constexpr int8_t UART1TXPin = GPIO_NUM_17;
HardwareSerial IDCSerial(1);	// UART for inter-device communication & control


#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

Scheduler MainScheduler;	// Main task scheduler

constexpr int HeartbeatLEDPin = GPIO_NUM_16;
constexpr auto NormalHeartbeatLEDToggleInterval = 1000;		// 1 s on, 1 s off; indicates normal heartbeat
constexpr auto NoSerialHeartbeatLEDToggleInterval = 500;

long HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
void ToggleHeartbeatLEDCallback();
Task ToggleHeartbeatLEDTask((NormalHeartbeatLEDToggleInterval * TASK_MILLISECOND), TASK_FOREVER, &ToggleHeartbeatLEDCallback, &MainScheduler, false);

#include "src/CSSMS3Controls.h"

constexpr long ReadControlsInterval = 100;
void ReadControlsCallback();
Task ReadControlsTask((ReadControlsInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadControlsCallback, &MainScheduler, false);

constexpr long ReadButtonsInterval = 5;
void ReadButtonsCallback();
Task ReadButtonsTask((ReadButtonsInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadButtonsCallback, &MainScheduler, false);

#include "src/CSSMS3Display.h"

constexpr long UpdateDisplayInterval = 100;
void UpdateDisplayCallback();
Task UpdateDisplayTask((UpdateDisplayInterval * TASK_MILLISECOND), TASK_FOREVER, &UpdateDisplayCallback, &MainScheduler, false);

constexpr long SendCSSMPacketInterval = 100;
void SendCSSMPacketCallback();
Task SendCSSMPacketTask((SendCSSMPacketInterval * TASK_MILLISECOND), TASK_FOREVER, &SendCSSMPacketCallback, &MainScheduler, false);

constexpr long ReadEnvSensorsInterval = 2000;
void ReadEnvSensorsCallback();
Task ReadEnvSensorsTask((ReadEnvSensorsInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadEnvSensorsCallback, &MainScheduler, false);

#include "src/CSSMS3Status.h"
#include <I2CBus.h>

#include <esp_now.h>
uint8_t MRSMCCMAC[] = { 0xF0, 0xF5, 0xBD, 0x42, 0xB7, 0x78 };
//uint8_t MRSMCCMAC[] = { 0x48, 0x27, 0xE2, 0xEA, 0xCA, 0x4C };		// Breadboard prototype
esp_now_peer_info_t MRSMCCInfo;

constexpr uint16_t MaxCSSMSendRetries = 16;

//#include "src/ESP32WiFi.h"
#include <WiFi.h>

#include "src/CSSMS3EnvSensors.h"

void setup()
{
	char buf[64];

	USBSerial.begin(115200);
	if (!USBSerial)
	{
		CSSMS3Status.UART0Status = false;
		HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		CSSMS3Status.UART0Status = true;
		HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
		_PL();
	}

	IDCSerial.begin(115200, SERIAL_8N1, GPIO_NUM_18, GPIO_NUM_17);
	if (!IDCSerial)
	{
		CSSMS3Status.UART1Status = false;
	}
	else
	{
		CSSMS3Status.UART1Status = true;
		CSSMS3Status.ComMode = CSSMS3StatusClass::ComModes::IDCPktSerial;
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

	// Initialize local display and show Debug page to provide information on progress initializing other components:
	if (cssmS3Display.Init())
	{
		CSSMS3Status.LocalDisplayStatus = true;
		_PL("Local Display initialized successfully");
	}
	else
	{
		CSSMS3Status.LocalDisplayStatus = false;
		_PL("Local Display initialization FAIL");
	}

	cssmS3Display.ReportHeapStatus(0);
	// Initialize WiFi and update display to confirm success:
	CSSMS3Status.WiFiStatus = false;
	CSSMS3Status.AddDebugTextLine("Initializing WiFi...");

	// Determine the channel used by local WiFi router so we can ensure compatibility 
	//when initializing ESP-NOW:
	int32_t channel = 0;
	if (int32_t n = WiFi.scanNetworks())
	{
		for (uint8_t i = 0; i < n; i++)
		{
			if (!strcmp("320", WiFi.SSID(i).c_str()))
			{
				channel = WiFi.channel(i);
			}
		}
	}
	sprintf(buf, "Using WiFi channel %d", channel);
	_PL(buf);

	WiFi.mode(WIFI_MODE_APSTA);
	WiFi.begin("320", "103187OS", channel);

	WiFi.printDiag(Serial);

	////Test code:
	//WiFi.disconnect();

	CSSMS3Status.ComMode = CSSMS3StatusClass::ComModes::WiFiTCP;

	uint8_t mac[6];
	WiFi.macAddress(mac);
	snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	_PL(buf)

	// Initialize ESP-NOW
	//// Set device as a Wi-Fi Station; turns WiFi radio ON:
	//WiFi.mode(WIFI_MODE_APSTA);
	//// Test code:
	//WiFi.printDiag(Serial);

	cssmS3Display.ReportHeapStatus(0);
	CSSMS3Status.AddDebugTextLine("Initializing ESP-NOW...");
	CSSMS3Status.ESPNOWStatus = (esp_now_init() == ESP_OK);
	if (!CSSMS3Status.ESPNOWStatus) {
		_PL("Error initializing ESP-NOW")
	}
	else
	{
		_PL("Initializing ESP-NOW")

			CSSMS3Status.ComMode = CSSMS3StatusClass::ComModes::ESPNOW;

		// Register OnDataSent callback
		esp_now_register_send_cb(OnMRSMCCDataSent);

		// Register peer
		memcpy(MRSMCCInfo.peer_addr, MRSMCCMAC, 6);
		//MRSMCCInfo.channel = 0;
		MRSMCCInfo.channel = channel;
		MRSMCCInfo.encrypt = false;

		// Add peer        
		if (esp_now_add_peer(&MRSMCCInfo) != ESP_OK)
		{
			_PL("Failed to add peer")
		}

		// Register OnDataReceived callback
		esp_now_register_recv_cb(esp_now_recv_cb_t(OnMRSMCCDataReceived));

	}
	sprintf(buf, "CSSMDrivePacket: %d b", sizeof(CSSMDrivePacket));
	CSSMS3Status.AddDebugTextLine(buf);
	_PL(buf)

	cssmS3Display.ReportHeapStatus(0);
	if (cssmS3Controls.Init(cssmS3Display.GetTFT()))
	{
		CSSMS3Status.SysDrvDisplayState = !cssmS3Controls.GetTS2State();
		_PP("TS2 state: ")
		_PL(!CSSMS3Status.SysDrvDisplayState)
		_PL("CSSMControls initialized successfully");
	}
	else
	{
		_PL("CSSMControls initialization FAILED");
	}
	ReadControlsTask.enable();
	ReadButtonsTask.enable();

	if (CSSMS3Status.SysDrvDisplayState)
	{
		cssmS3Display.Control(CSSMS3Display::Commands::DRVPage);
	}
	else
	{
		cssmS3Display.Control(CSSMS3Display::Commands::SYSPage);
	}
	UpdateDisplayTask.enable();

	EnvSensors.Init();
	
	if (CSSMS3Status.ESPNOWStatus)
	{
		SendCSSMPacketTask.enable();
		// Set ESPNOWStatus to match initial setting of the ESP-NOW menu item used to enable / disable the command stream from 
		//the CSSM to the MRS MCC, which should be FALSE to start
		// User initiates telemetry to the MRS through the on-screen menu system when ready:
		CSSMS3Status.ESPNOWStatus = cssmS3Controls.GetESPNowStatus();
	}

	ToggleHeartbeatLEDTask.setInterval(HeartbeatLEDTogglePeriod * TASK_MILLISECOND);
	ToggleHeartbeatLEDTask.enable();
}

void loop()
{
	MainScheduler.execute();
}

void ToggleHeartbeatLEDCallback()
{
	digitalWrite(HeartbeatLEDPin, !digitalRead(HeartbeatLEDPin));
}

void ReadControlsCallback()
{
	cssmS3Controls.Update();
}

void ReadButtonsCallback()
{
	cssmS3Controls.CheckButtons();
}

void UpdateDisplayCallback()
{
	CSSMS3Status.Update();		// To check health of ESP-NOW telemetry
	cssmS3Display.Update();
}

void SendCSSMPacketCallback()
{
	char buf1[32];
	char buf2[64];

	esp_err_t result = ESP_OK;

	if (CSSMS3Status.ESPNOWStatus)
	{
		result = esp_now_send(MRSMCCMAC, (uint8_t*)&CSSMS3Status.cssmDrivePacket, sizeof(CSSMS3Status.cssmDrivePacket));

		if (result != ESP_NOW_SEND_SUCCESS)
		{
			sprintf(buf2, "ESP-NOW send error: %S", esp_err_to_name(result));
			_PL(buf2)
		}

	}
}

void ReadEnvSensorsCallback()
{
}

void OnMRSMCCDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
	char buf[64];

	bool result = (status == ESP_NOW_SEND_SUCCESS);

	//cssmS3Controls.SetESPNOWStatus(status == ESP_NOW_SEND_SUCCESS);
	if (result)
	{
		CSSMS3Status.ESPNOWPacketSentCount++;
		CSSMS3Status.SendRetries = 0;
	}
	else
	{
		CSSMS3Status.SendRetries++;

		//_PL("ESP-NOW data send FAIL")
		
		//sprintf(buf, "ESP-NOW send error: %d", result);
		//_PL(buf)
		
		////TODO: Need to change this to base test on whether sufficient time is left in the current task cycle to retry
		////Consider defining a MAX_RETRIES parameter to limit the number of potential retries and so limit the time use on retries.  
		//if (millis() < NextPacketSendTime + SendCSSMPacketInterval)
		//	if (CSSMS3Status.SendRetries < MaxCSSMSendRetries)
		//	{
		//		// Try to re-send:
		//		CSSMS3Status.SendRetries++;
		//		SendCSSMPacketCallback();
		//	}
		//	else
		//	{
		//		CSSMS3Status.SendRetries = 0;
		//		_PL("ESP-NOW data send FAIL")
		//	}
	}

}

void OnMRSMCCDataReceived(const uint8_t* mac, const uint8_t* data, int lenght)
{
	char buf[32];

	switch (data[0])
	{
	case 0x30:
		memcpy(&(CSSMS3Status.mcStatus), data, sizeof(CSSMS3Status.mcStatus));
		break;
	case 0x31:
		memcpy(&(CSSMS3Status.mcStatus), data, sizeof(CSSMS3Status.mcStatus));
		break;
	default:
		break;
	}

	CSSMS3Status.MRSMCCPacketReceivedCount++;
	uint64_t receiptTime = millis();
	CSSMS3Status.MCCPacketReceiptInterval = receiptTime - CSSMS3Status.LastMCCPacketReceivedTime;
	CSSMS3Status.LastMCCPacketReceivedTime = receiptTime;

	//if (data[0] == 0x30)	// Packet type identifier for a RC2x15AMCStatusPacket
	//{
	//	memcpy(&(CSSMS3Status.mcStatus), data, sizeof(CSSMS3Status.mcStatus));
	//	//sprintf(buf, MACSTR, MAC2STR(mac));
	//	//MCCStatus.IncomingCSSMPacketMACString = String(buf);
	//	CSSMS3Status.MRSMCCPacketReceivedCount++;
	//	uint64_t receiptTime = millis();
	//	CSSMS3Status.MCCPacketReceiptInterval = receiptTime - CSSMS3Status.LastMCCPacketReceivedTime;
	//	CSSMS3Status.LastMCCPacketReceivedTime = receiptTime;

	//}
}