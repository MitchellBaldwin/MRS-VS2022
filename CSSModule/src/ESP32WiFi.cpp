/*	ESP32WiFi.cpp
*	ESP32WiFiClass - Base class for managing WiFi connectivity for ESP32-based MRS modules
*
*
*/

#include "ESP32WiFi.h"
#include "DEBUG Macros.h"
#include "CSSMStatus.h"
#include "DebugDisplay.h"
#include "CSSMSensorData.h"

bool ESP32WiFiClass::Init(bool resetWiFiAndInvokeConfigPortal = false)
{
	//DONE: Set CSSMStatus flag to reflect WiFi connection success & state
	//DONE: Implement a hardware means to force the WiFi Manager into configuration mode to allow selection of a different network
	
	server = new AsyncWebServer(80);
	dns = new DNSServer();
	wifiManager = new AsyncWiFiManager(server, dns);

	if (CSSMStatus.DebugDisplayStatus)
	{
		DebugDisplay.AddTextLine("Initializing WiFi");
		DebugDisplay.Update();

	}
	
	wifiManager->setConnectTimeout(30);			// 30 s connection timeout period allowed
	wifiManager->setConfigPortalTimeout(180);	// 3 minute configuration portal timeout

	bool success = false;
	if (resetWiFiAndInvokeConfigPortal)
	{
		if (CSSMStatus.DebugDisplayStatus)
		{
			DebugDisplay.AddTextLine("Start config portal");
			DebugDisplay.AddTextLine("MRS CSSM 192.168.4.1");
			DebugDisplay.Update();

		}
		_PL("On-demand config portal start");
		wifiManager->resetSettings();
		delay(1000);
		success = wifiManager->startConfigPortal("MRS CSSM", "password");
	}
	else
	{
		if (CSSMStatus.DebugDisplayStatus)
		{
			DebugDisplay.AddTextLine("Attempt auto-connext");
			//DebugDisplay.AddTextLine("MRS CSSM 192.168.4.1");
			DebugDisplay.Update();

		}
		_PL("Config portal auto-connect");
		success = wifiManager->autoConnect("MRS CSSM", "password");
	}

	if (CSSMStatus.DebugDisplayStatus)
	{
		if (success)
		{
			DebugDisplay.AddTextLine("WiFi connected");
		}
		else
		{
			DebugDisplay.AddTextLine("Not connected!");
		}
	}

	server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(200, "text/html", "<b>Mobile Robot System</b> Control Stick Steering Module (MRS-CSSM)<br>Enter '[local IP address]/update' in the browser address bar to update firmware");
		});

	//AsyncElegantOTA.begin(&server);    // Start ElegantOTA
	server->begin();
	_PP("HTTP server started at ");
	IPAddress LocalIP = WiFi.localIP();
	_PP(LocalIP.toString());
	_PP(":");
	_PL();

	return success;
}

ESP32WiFiClass ESP32WiFi;

