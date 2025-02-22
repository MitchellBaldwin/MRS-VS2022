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

uint8_t MRSRCCSSMS3MAC[] = { 0xF0, 0xF5, 0xBD, 0x48, 0x0A, 0x4C };
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

#include "src/MCCControls.h"

constexpr long ReadControlsInterval = 100;
void ReadControlsCallback();
Task ReadControlsTask((ReadControlsInterval* TASK_MILLISECOND), TASK_FOREVER, &ReadControlsCallback, &MainScheduler, false);

constexpr long ReadButtonsInterval = 5;
void ReadButtonsCallback();
Task ReadButtonsTask((ReadButtonsInterval* TASK_MILLISECOND), TASK_FOREVER, &ReadButtonsCallback, &MainScheduler, false);

constexpr auto UpdateLocalDisplayInterval = 100;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((UpdateLocalDisplayInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

constexpr auto UpdateMotorControllerInterval = 100;
void UpdateMotorControllerCallback();
Task UpdateMotorControllerTask((UpdateMotorControllerInterval* TASK_MILLISECOND), TASK_FOREVER, &UpdateMotorControllerCallback, &MainScheduler, false);

constexpr long SendRC2x15AMCStatusPacketInterval = 300;
void SendRC2x15AMCStatusPacketCallback();
Task SendRC2x15AMCStatusPacketTask((SendRC2x15AMCStatusPacketInterval* TASK_MILLISECOND), TASK_FOREVER, &SendRC2x15AMCStatusPacketCallback, &MainScheduler, false);

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

	// Initialize LocalDisplay and show Debug page to provide information on progress initializing other components:
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

	// Initialize motor controller:
	MCCStatus.RC2x15AUARTStatus = RC2x15AMC.Init();

	pinMode(HeartbeatLEDPin, OUTPUT);
	sprintf(buf, "Heartbeat LED on GPIO%02D", HeartbeatLEDPin);
	_PL(buf);

	//Test code:
	Wire.begin(GPIO_NUM_43, GPIO_NUM_44, 100000);
	//Wire.setClock(100000);
	//End test code block

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

	// Initialize ESP-NOW
	// Set device as a Wi-Fi Station; turns WiFi radio ON:
	//WiFi.mode(WIFI_STA);	// WiFi radio is already on by virtue of the call to ESP32WiFi.Init(), above
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
		memcpy(MRSRCCSSMInfo.peer_addr, MRSRCCSSMS3MAC, 6);
		MRSRCCSSMInfo.channel = channel;
		MRSRCCSSMInfo.encrypt = false;

		// Add peer        
		if (esp_now_add_peer(&MRSRCCSSMInfo) != ESP_OK)
		{
			_PL("Failed to add peer")
		}

		// Register OnDataReceived callback
		esp_now_register_recv_cb(esp_now_recv_cb_t(OnMRSRCCSSMDataReceived));

	}

	uint8_t mac[6];
	WiFi.macAddress(mac);
	snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	_PL(buf)

	if (mccControls.Init(LocalDisplay.GetTFT()))
	{
		_PL("mccControls initialized successfully");
	}
	else
	{
		_PL("mccControls initialization FAILED");
	}
	ReadButtonsTask.enable();
	ReadControlsTask.enable();

	UpdateMotorControllerTask.enable();
	if (MCCStatus.RC2x15AUARTStatus)
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

	sprintf(buf, "CSSMDrivePacket: %d b", sizeof(CSSMDrivePacket));
	MCCStatus.AddDebugTextLine(buf);
	_PL(buf)

	sprintf(buf, "RC2x15AMCStatusPacket: %d b", sizeof(RC2x15AMCStatusPacket));
	MCCStatus.AddDebugTextLine(buf);
	_PL(buf)

	// Components initialzed; switch LocalDisplay to normal operation:
	LocalDisplay.Control(LocalDisplayClass::Commands::SYSPage);
	UpdateLocalDisplayTask.enable();

	// Set ESPNOWStatus to match initial setting of the ESP-NOW menu item used to enable / disable the telemetry stream from 
	//the MCC to the MRS RC CSSM, which should be TRUE to start
	// User initiates telemetry to the MRS through the on-screen menu system when ready:
	MCCStatus.ESPNOWStatus = mccControls.GetESPNowStatus();
	if (MCCStatus.ESPNOWStatus)
	{
		SendRC2x15AMCStatusPacketTask.enable();
	}

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

void ReadControlsCallback()
{
	mccControls.Update();
}

void ReadButtonsCallback()
{
	mccControls.CheckButtons();
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}

void UpdateMotorControllerCallback()
{
	RC2x15AMC.Update();
}

void SendRC2x15AMCStatusPacketCallback()
{
	char buf2[64];

	esp_err_t result = ESP_OK;

	if (MCCStatus.ESPNOWStatus)
	{
		result = esp_now_send(MRSRCCSSMS3MAC, (uint8_t*)&MCCStatus.mcStatus, sizeof(MCCStatus.mcStatus));

		if (result != ESP_NOW_SEND_SUCCESS)
		{
			sprintf(buf2, "ESP-NOW send error: %S", esp_err_to_name(result));
			_PL(buf2)
		}

	}
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
		//sprintf(buf, MACSTR, MAC2STR(mac));
		//MCCStatus.IncomingCSSMPacketMACString = String(buf);
		MCCStatus.CSSMPacketReceivedCount++;
		uint64_t receiptTime = millis();
		MCCStatus.CSSMPacketReceiptInterval = receiptTime - MCCStatus.LastCSSMPacketReceivedTime;
		MCCStatus.LastCSSMPacketReceivedTime = receiptTime;

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
		//	RC2x15AMC.DriveThrottleTurnRate(MCCStatus.cssmDrivePacket.Throttle, MCCStatus.cssmDrivePacket.OmegaXY);
		//}
	// End of test code block

	}
}