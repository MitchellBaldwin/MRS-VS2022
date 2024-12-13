/*
 Name:		CSSModuleESP32.ino
 Created:	6/24/2023 3:51:52 PM
 Author:	Mitchell Baldwin copyright 2023

 Main executable for the Mobile Robot System Remote Controller Control Stick Steering Module (MRS RC CSSM)

 Controller: DOIT ESP32 DEVKIT V1

 Configuration notes:
	Board: DOIT ESP32 DEVKIT V1
	TFT_eSPI setup: [None; using Adafruit GFX libraries with monochrome oLED displays]

 v1.0	Initial release
 v1.1	Added support for ladder button arrays
 v1.2	Added WiFi and OTA programming support
 v1.3	Added ESP-NOW support
 v1.4	

*/

//#include <WiFiAP.h>
//#include <WiFiClient.h>
//#include <WiFiGeneric.h>
//#include <WiFiMulti.h>
//#include <WiFiScan.h>
//#include <WiFiServer.h>
//#include <WiFiSTA.h>
//#include <WiFiType.h>
//#include <WiFiUdp.h>
//#include <AsyncEventSource.h>
//#include <AsyncJson.h>
//#include <AsyncWebSocket.h>
//#include <AsyncWebSynchronization.h>
//#include <SPIFFSEditor.h>
//#include <StringArray.h>
//#include <WebAuthentication.h>
//#include <WebHandlerImpl.h>
//#include <WebResponseImpl.h>
//#include <elegantWebpage.h>
//#include <Hash.h>
//#include <AsyncTCP.h>
//#include <wm_strings_es.h>
//#include <wm_strings_en.h>
//#include <wm_consts_en.h>
//#include <strings_en.h>
//#include <WiFiManager.h>
//#include <ESPAsyncWebServer.h>

#include "FS.h"
#include <LittleFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

#include <esp_now.h>
//uint8_t MRSRCPCMMAC[] = { 0x48, 0x27, 0xE2, 0xEA, 0xCA, 0x4C };		// Breadboard prototype
//esp_now_peer_info_t MRSRCPCMInfo;
uint8_t MRSMCCMAC[] = { 0xF0, 0xF5, 0xBD, 0x42, 0xB7, 0x78 };
//uint8_t MRSMCCMAC[] = { 0x48, 0x27, 0xE2, 0xEA, 0xCA, 0x4C };		// Breadboard prototype
//uint8_t MRSMCCMAC[] = { 0xF0, 0xF5, 0xBD, 0x42, 0xB3, 0xA0 };		// New module
esp_now_peer_info_t MRSMCCInfo;
bool MRSMCCOKToSend = true;

#include "src/ESP32WiFi.h"

#include <DEBUG Macros.h>

#include "src/BME280Data.h"
#include "src/CSSMStatus.h"

//constexpr byte DebugLED = 0x04;

#include <TaskScheduler.h>
// #define _TASK_TIMECRITICAL		// Enable monitoring scheduling overruns
// #define _TASK_SLEEP_ON_IDLE_RUN	// Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass
// #define _TASK_STATUS_REQUEST		// Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
// #define _TASK_WDT_IDS			// Compile with support for wdt control points and task ids
// #define _TASK_LTS_POINTER		// Compile with support for local task storage pointer
// #define _TASK_PRIORITY			// Support for layered scheduling priority
// #define _TASK_MICRO_RES			// Support for microsecond resolution
// #define _TASK_STD_FUNCTION		// Support for std::function (ESP8266 and ESP32 ONLY)
// #define _TASK_DEBUG				// Make all methods and variables public for debug purposes
// #define _TASK_INLINE				// Make all methods "inline" - needed to support some multi-tab, multi-file implementations
// #define _TASK_TIMEOUT			// Support for overall task timeout
// #define _TASK_OO_CALLBACKS		// Support for dynamic callback method binding
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

#include <HardwareSerial.h>
HardwareSerial USBSerial(0);	// Uses the same UART device as "Serial" but does not allow use of "Serial"?
HardwareSerial IDCSerial(2);	// UART for inter-device communication & control

#include <I2CBus.h>

#include "src/LocalDisplay.h"
constexpr byte LocalDisplayI2CAddress = 0x3C;
#include "src/DebugDisplay.h"
constexpr byte DebugDisplayI2CAddress = 0x3D;

#ifdef _TEST_
char buf[32];
uint32_t testCounter = 0;
#endif

#include "src/CSSMSensorData.h"
// TODO: If OSB arrays are powered from ESP 32 3.3 V supply then consider running signals directly to the ADC ports
// Scale and buffer VIN signal and use the VP pin (pin 36) for measurement
constexpr byte LOSBAnalogPin = 34;
constexpr byte ROSBAnalogPin = 35;		//TODO: Deconflict with ThrottleSensePin
constexpr byte ThrottlePin = 35;
constexpr byte ESP32VINAnalogPin = 36;

constexpr byte LeftToggleSwitchPin = 0x1B;		// GPIO27
constexpr byte CenterToggleSwitchPin = 0x20;	// GPIO32
constexpr byte RightToggleSwitchPin = 0x21;		// GPIO33
constexpr byte RightRockerSwitchPin = 0x04;		// GPIO04

#include "src/OSBArray.h"

// Uncomment just one of the defines to select OSB array in use:
//#define _LROSBS
//#define _ROBOX
#define _OSBS_WITH_ENCODERS

#ifdef _ROBOX
OSBArrayClass OSBArray(LOSBAnalogPin);
static const uint8_t OBS_NUM_BUTTONS = 8;
static const uint8_t OSB_NUM_LEVELS = OBS_NUM_BUTTONS + 1;
static const uint16_t OSB_LEVELS[OSB_NUM_LEVELS] =
{
  250,
  860,
  1140,
  1440,
  1700,
  2040,
  2360,
  2750,
  3290,
};
#endif
#ifdef defined _LROSBS
OSBArrayClass LOSBArray(LOSBAnalogPin);
static const uint8_t LOBS_NUM_BUTTONS = 4;
static const uint8_t LOSB_NUM_LEVELS = LOBS_NUM_BUTTONS + 1;
static const uint16_t LOSB_LEVELS[LOSB_NUM_LEVELS] = 
{
  0,		/* 0%, short to ground */
  1165,
  1770,
  2400,
  3160
};
OSBArrayClass ROSBArray(LOSBAnalogPin);
static const uint8_t ROBS_NUM_BUTTONS = 4;
static const uint8_t ROSB_NUM_LEVELS = ROBS_NUM_BUTTONS + 1;
static const uint16_tRLOSB_LEVELS[ROSB_NUM_LEVELS] = 
{
  0,		/* 0%, short to ground */
  1165,
  1770,
  2400,
  3160
};
#endif
#ifdef _OSBS_WITH_ENCODERS
OSBArrayClass OSBArray(LOSBAnalogPin);
static const uint8_t OBS_NUM_BUTTONS = 6;
static const uint8_t OSB_NUM_LEVELS = OBS_NUM_BUTTONS + 1;
static const uint16_t OSB_LEVELS[OSB_NUM_LEVELS] =
{
  0,
  797,
  1203,
  1623,
  2078,
  2594,
  3249
};
#endif

// Scheduler
Scheduler MainScheduler;

constexpr long NormalHeartbeatLEDToggleInterval = 1000;
constexpr long NoSerialHeartbeatLEDToggleInterval = 250;
constexpr long NoLocalDisplayHeartbeatLEDToggleInterval = 100;

long HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
void ToggleBuiltinLEDCallback();
Task HeartbeatLEDTask((HeartbeatLEDTogglePeriod * TASK_MILLISECOND), TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

constexpr long ReadControlsInterval = 100;
void ReadControlsCallback();
Task ReadControlsTask((ReadControlsInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadControlsCallback, &MainScheduler, false);

constexpr long ReadSensorsInterval = 50;
void ReadSensorDataCallback();
Task ReadSensorsTask((ReadSensorsInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadSensorDataCallback, &MainScheduler, false);

constexpr long ReadENVDataInterval = 5000;
void ReadENVDataCallback();
Task ReadENVDataTask((ReadENVDataInterval * TASK_MILLISECOND), TASK_FOREVER, &ReadENVDataCallback, &MainScheduler, false);

constexpr long UpdateLocalDisplayInterval = 100;
void UpdateLocalDisplayCallback();
Task UpdateLocalDisplayTask((UpdateLocalDisplayInterval * TASK_MILLISECOND), TASK_FOREVER, &UpdateLocalDisplayCallback, &MainScheduler, false);

constexpr long UpdateDebugDisplayInterval = 500;
void UpdateDebugDisplayCallback();
Task UpdateDebugDisplayTask((UpdateDebugDisplayInterval * TASK_MILLISECOND), TASK_FOREVER, &UpdateDebugDisplayCallback, &MainScheduler, false);

constexpr long SendCSSMPacketInterval = 300;
void SendCSSMPacketCallback();
Task SendCSSMPacketTask((SendCSSMPacketInterval * TASK_MILLISECOND), TASK_FOREVER, &SendCSSMPacketCallback, &MainScheduler, false);

uint32_t NextPacketSendTime = 0;				// ms
uint32_t InitiatePacketSendTime = 0;			// ms

void setup()
{
	USBSerial.begin(115200);
	if (!USBSerial)
	{
		CSSMStatus.UART0Status = false;
		HeartbeatLEDTogglePeriod = NoSerialHeartbeatLEDToggleInterval;
	}
	else
	{
		CSSMStatus.UART0Status = true;
		HeartbeatLEDTogglePeriod = NormalHeartbeatLEDToggleInterval;
		_PL();
	}

	IDCSerial.begin(115200);
	if (!IDCSerial)
	{
		CSSMStatus.UART2Status = false;
	}
	else
	{
		CSSMStatus.UART2Status = true;
		CSSMStatus.ComMode = CSSMStatusClass::ComModes::IDCPktSerial;
	}

	if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
		Serial.println("LittleFS Mount Failed");
		return;
	}
	listDir(LittleFS, "/", 1);

	char buf[32];
	snprintf(buf, 31, "Heartbeat LED pin 0x%02X", LED_BUILTIN);
	pinMode(LED_BUILTIN, OUTPUT);
	_PL(buf);
	//pinMode(DebugLED, OUTPUT);
	//snprintf(buf, 31, "Debug LED pin 0x%02X", DebugLED);
	//_PL(buf);

	I2CBus.Init();
	I2CBus.Scan();
	_PL(I2CBus.GetActiveI2CAddressesString().c_str());

#ifdef _ROBOX
	OSBArray.Init(OSB_NUM_LEVELS, OSB_LEVELS);
#endif
#ifdef _LROSBS
	LOSBArray.Init(LOSB_NUM_LEVELS, LOSB_LEVELS);
	ROSBArray.Init(ROSB_NUM_LEVELS, ROSB_LEVELS);
#endif
#ifdef _OSBS_WITH_ENCODERS
	OSBArray.Init(OSB_NUM_LEVELS, OSB_LEVELS);
#endif

	SensorData.Init(LOSBAnalogPin, ThrottlePin, ESP32VINAnalogPin, LeftToggleSwitchPin, CenterToggleSwitchPin, RightToggleSwitchPin, RightRockerSwitchPin);
	SensorData.LeftToggleSwitchHandler = LeftToggleSwitchStateChanged;

	if (!DebugDisplay.Init(DebugDisplayI2CAddress))
	{
		CSSMStatus.DebugDisplayStatus = false;
		//HeartbeatLEDTogglePeriod = NoDebugDisplayHeartbeatLEDToggleInterval;
		snprintf(buf, 31, "Debug Display not found at I2C 0x%02X", DebugDisplayI2CAddress);
		_PL(buf)
	}
	else
	{
		CSSMStatus.DebugDisplayStatus = true;
		DebugDisplay.Control(DebugDisplayClass::HOMPage);
		snprintf(buf, 31, "Debug Display at I2C 0x%02X", DebugDisplayI2CAddress);
		_PL(buf)
	}

	// Initialize WiFi and update Debug display to confirm success;
	// Perform hard reset of WiFi network association if right rocker switch is ON, invoking WiFi manager config portal on soft AP at 192.168.4.1:
	CSSMStatus.WiFiStatus = false;
	CSSMStatus.WiFiStatus = ESP32WiFi.Init(SensorData.GetRightRockerSwitchStateRaw() == 0);
	CSSMStatus.ComMode = CSSMStatusClass::ComModes::WiFiTCP;

	// Test code:
	WiFi.printDiag(USBSerial);

	// Initialize ESP-NOW
	// Set device as a Wi-Fi Station; turns WiFi radio ON:
	//WiFi.mode(WIFI_STA);	// WiFi radio is already on by virtue of the call to ESP32WiFi.Init(), above
	//WiFi.begin();
	WiFi.mode(WIFI_MODE_APSTA);
	//WiFi.disconnect();

	CSSMStatus.ESPNOWStatus = (esp_now_init() == ESP_OK);
	if (!CSSMStatus.ESPNOWStatus) {
		_PL("Error initializing ESP-NOW")
	}
	else
	{
		_PL("Initializing ESP-NOW")

		CSSMStatus.ComMode = CSSMStatusClass::ComModes::ESPNOW;
		
		// Register OnDataSent callback
		esp_now_register_send_cb(OnMRSMCCDataSent);

		// Register peer
		memcpy(MRSMCCInfo.peer_addr, MRSMCCMAC, 6);
		MRSMCCInfo.channel = 0;
		MRSMCCInfo.encrypt = false;

		// Add peer        
		if (esp_now_add_peer(&MRSMCCInfo) != ESP_OK)
		{
			_PL("Failed to add peer")
			//return;
		}
	}

	uint8_t mac[6];
	WiFi.macAddress(mac);
	snprintf(buf, 22, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	_PL(buf)

	if (CSSMStatus.DebugDisplayStatus)
	{
		DebugDisplay.Update();
	}
	
	if (!LocalDisplay.Init(LocalDisplayI2CAddress))
	{
		CSSMStatus.LocalDisplayStatus = false;
		HeartbeatLEDTogglePeriod = NoLocalDisplayHeartbeatLEDToggleInterval;
		snprintf(buf, 31, "Local Display not found at I2C 0x%02X", LocalDisplayI2CAddress);
		_PL(buf)
	}
	else
	{
		CSSMStatus.LocalDisplayStatus = true;
		LocalDisplay.Control(LocalDisplayClass::SYSPage);
		snprintf(buf, 31, "Local Display at I2C 0x%02X", LocalDisplayI2CAddress);
		_PL(buf)
	}

	ReadSensorsTask.enable();
	ReadENVDataTask.enable();
	ReadControlsTask.enable();
	UpdateLocalDisplayTask.enable();
	UpdateDebugDisplayTask.enable();

	if (CSSMStatus.ESPNOWStatus)
	{
		SendCSSMPacketTask.enable();
	}

	// Test code:
	sprintf(buf, "CSSMDrivePacket size = %d bytes", sizeof(CSSMStatus.cssmDrivePacket));
	_PL(buf)

	HeartbeatLEDTask.setInterval(HeartbeatLEDTogglePeriod * TASK_MILLISECOND);
	HeartbeatLEDTask.enable();

}

void loop()
{
	MainScheduler.execute();

	//if (millis() >= NextPacketSendTime)
	//{
	//	CSSMStatus.SendRetries = 0;
	//	InitiatePacketSendTime = millis();
	//	SendCSSMPacketCallback();
	//	NextPacketSendTime += SendCSSMPacketInterval;
	//}

	// Test code:

}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void ReadSensorDataCallback()
{
	SensorData.Update();
}

void ReadENVDataCallback()
{
	SensorData.ReadENVData();
}

void ReadControlsCallback()
{
	byte OSBPressed = OSBArray.GetOSBPress();
	if (OSBPressed)
	{
		char buf[32];
#ifdef _TEST_
		////snprintf(buf, 32, "OSB ID 0x%02X pressed", OSBPressed);
		////_PL(buf);
#endif // _TEST_

		//TODO: To support multi-function OSBs the response to the press of each OSB must be based on other states
		//including the current LocalDisplay page state and CSSMStatusClass::DriveMode 
		// Replace the PageMenu text displayed on the bottom line of the LocalDisplay with acronyms or pneumonics 
		//indicating the function of each OSB, or at least the center four key-style OSBs 
		//(i.e., not the buttons integral to the rotary encoders).
		
		switch (OSBPressed)
		{
		case OSBArrayClass::OSB1:	// CRS (left) rotary endocer button
			// CRS rotary; press to engage WPT mode
			CSSMStatus.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::SEQ;
			LocalDisplay.Control(LocalDisplayClass::SEQPage);
			break;
		case OSBArrayClass::OSB2:	// OSB2/SYS
			if (LocalDisplay.IsOnSYSPage())
			{
				// Cycle Debug display
				DebugDisplay.Control(DebugDisplayClass::Next);
			}
			else
			{
				LocalDisplay.Control(LocalDisplayClass::SYSPage);
			}
			break;
		case OSBArrayClass::OSB3:	// OSB3/>Mode
			// Return LocalDisplay to the page correspnding to the current CSSMStatus.DriveMode
			LocalDisplay.ShowCurrentDriveModePage();
			break;
		case OSBArrayClass::OSB4:	// OSB4/>Func
			switch (LocalDisplay.GetCurrentPage())
			{
			case LocalDisplayClass::Pages::COM:
				// 'WiFi' Func> OSB response:
				//TODO: ESP32WiFi.Init(true) fails because the method re-creates the WiFiManage, DNS, and server objects;
				//Need to write a new method allowing graceful reset of the WiFi system and then enter the config portal
				//ESP32WiFi.Init(true);
				CSSMStatus.WiFiStatus = ESP32WiFi.EnterConfigPortal();
				break;
			case LocalDisplayClass::Pages::I2C:
				// 'Scan' Func> OSB response:
				DebugDisplay.AddTextLine("Scanning I2C bus...");
				I2CBus.Scan();
				DebugDisplay.AddTextLine("I2C bus scan complete");
				break;
			case LocalDisplayClass::Pages::ENV:
				// 'UPDT' Func> OSB response:
				SensorData.ReadENVData();
				break;
			default:
				break;
			}
			break;
		case OSBArrayClass::OSB5:	// OSB5/>Next
			// Cycle SYS display
			LocalDisplay.Control(LocalDisplayClass::Next);
			break;
		case OSBArrayClass::OSB6:	// HDG (right) rotary encoder button
			CSSMStatus.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::HDG;
			LocalDisplay.Control(LocalDisplayClass::HDGPage);
			break;
		//case OSBArrayClass::OSB7:
		//	break;
		//case OSBArrayClass::OSB8:
		//	break;
		default:
			break;
		}
	}
	
	// Abandoned use of AceButton library for ladder button arrays due to issue with one button in the ladder being unusable:
	//buttonConfig.checkButtons();

	// Consider using AceButton library to respond to changes of toggle / rocker switch states (Already using ezButton library)

}

void LeftToggleSwitchStateChanged(byte data)
{
	if (SensorData.LeftToggleSwitchState)
	{
		// Engage Direct Drive (DRV) mode
		CSSMStatus.SavedDriveMode = CSSMStatus.cssmDrivePacket.DriveMode;
		CSSMStatus.cssmDrivePacket.DriveMode = CSSMDrivePacket::DriveModes::DRV;
	}
	else
	{
		CSSMStatus.cssmDrivePacket.DriveMode = CSSMStatus.SavedDriveMode;
	}

	// Return LocalDisplay to the page correspnding to the current CSSMStatus.DriveMode
	LocalDisplay.ShowCurrentDriveModePage();
}

void UpdateLocalDisplayCallback()
{
	LocalDisplay.Update();
}

void UpdateDebugDisplayCallback()
{
#ifdef _TEST_
	//*** Test code start:
	snprintf(buf, 22, "Testing %06d", testCounter++);
	DebugDisplay.AddTextLine(buf);
	//*** Test code end.
#endif // _TEST_

	DebugDisplay.Update();
}

/// <summary>
/// Sends Control Stick Steering command packets to MRS MCC
/// 
/// </summary>
void SendCSSMPacketCallback()
{
	char buf1[32];
	char buf2[64];

	esp_err_t result = ESP_OK;

	//if (MRSMCCOKToSend)
	//{
		MRSMCCOKToSend = false;
		result = esp_now_send(MRSMCCMAC, (uint8_t*)&CSSMStatus.cssmDrivePacket, sizeof(CSSMStatus.cssmDrivePacket));
	//}
	if (result != ESP_NOW_SEND_SUCCESS)
	{
		sprintf(buf2, "ESP-NOW send error: %S", esp_err_to_name(result));
		_PL(buf2)
	}

}

void OnMRSRCPCMDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
	CSSMStatus.ESPNOWStatus = (status == ESP_NOW_SEND_SUCCESS);
	if (CSSMStatus.ESPNOWStatus)
	{
		CSSMStatus.ESPNOWPacketSentCount++;
	}

	//char buf[64];
	//snprintf(buf, 63, "\r\nLast Packet Send Status:\t %s", (status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail"));
	//_PL(buf)
}

void OnMRSMCCDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
	char buf[64];


	CSSMStatus.ESPNOWStatus = (status == ESP_NOW_SEND_SUCCESS);
	if (CSSMStatus.ESPNOWStatus)
	{
		CSSMStatus.ESPNOWPacketSentCount++;
		CSSMStatus.SendRetries = 0;
		MRSMCCOKToSend = true;				// Once TX callback executes it is OK to send the next packet
		
		// Test code:
		//snprintf(buf, 63, "Packet send success after %d ms, %d retries", (millis() - InitiatePacketSendTime), CSSMStatus.SendRetries);
		//_PL(buf);
	}
	else
	{
		if (millis() < NextPacketSendTime + SendCSSMPacketInterval)
		{
			// Try to re-send:
			CSSMStatus.SendRetries++;
			SendCSSMPacketCallback();
		}
		else
		{
			_PL("ESP-NOW data send FAIL")
		}
	}
}

void listDir(fs::FS& fs, const char* dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\r\n", dirname);

	File root = fs.open(dirname);
	if (!root) {
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println(" - not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) {
				listDir(fs, file.path(), levels - 1);
			}
		}
		else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}