/*	ESP32WiFi.cpp
*	ESP32WiFiClass - Base class for managing WiFi connectivity for ESP32-based MRS modules
*
*
*/

#include "ESP32WiFi.h"
#include "DEBUG Macros.h"
#include "CSSMStatus.h"
#include <AsyncElegantOTA.h>
#include <elegantWebpage.h>
#include <Hash.h>

bool ESP32WiFiClass::Init(bool resetWiFiAndInvokeConfigPortal = false)
{
	//DONE: Set CSSMStatus flag to reflect WiFi connection success & state
	//DONE: Implement a hardware means to force the WiFi Manager into configuration mode to allow selection of a different network
	char buf[32];

	server = new AsyncWebServer(HTTPServerPort);
	dns = new DNSServer();
	wifiManager = new AsyncWiFiManager(server, dns);


	//if (CSSMStatus.DebugDisplayStatus)
	//{
	//	DebugDisplay.AddTextLine("Initializing WiFi");
	//	DebugDisplay.Update();

	//}
	
	wifiManager->setConnectTimeout(30);			// 30 s connection timeout period allowed
	wifiManager->setConfigPortalTimeout(180);	// 3 minute configuration portal timeout
	wifiManager->setAPCallback(ESP32WiFiClass::configModeCallback);

	bool success = false;
	if (resetWiFiAndInvokeConfigPortal)
	{
		// Replaced by callback function added to WiFiManager:
		//if (CSSMStatus.DebugDisplayStatus)
		//{
		//	DebugDisplay.AddTextLine("Starting config portal");
		//	DebugDisplay.AddTextLine("MRS CSSM 192.168.4.1");
		//	DebugDisplay.Update();
		//}
		_PL("Resetting WiFiManager and starting on-demand config portal...");
		wifiManager->resetSettings();
		delay(1000);
		success = wifiManager->startConfigPortal("MRS CSSMS3", "password");
	}
	else
	{
		//if (CSSMStatus.DebugDisplayStatus)
		//{
		//	//DebugDisplay.AddTextLine("Try auto-connect...");
		//	
		//	// Try to display the saved SSID that the autoConnect method will use:
		//	//snprintf(buf, 22, "SSID: %s", wifiManager->getConfigPortalSSID().c_str());
		//	//WiFi.mode(WIFI_STA);
		//	//WiFi.begin();
		//	//snprintf(buf, 22, "Saved SSID: %s", WiFi.SSID().c_str());
		//	//DebugDisplay.AddTextLine(buf);
		//	//DebugDisplay.Update();

		//}
		_PL("Config portal auto-connect");
		success = wifiManager->autoConnect("MRS CSSM", "password");
	}

	//if (CSSMStatus.DebugDisplayStatus)
	//{
	//	if (success)
	//	{
	//		snprintf(buf, 22, "Connected (%d dBm)", WiFi.RSSI());
	//		//DebugDisplay.AddTextLine(buf);
	//		snprintf(buf, 22, "SSID: %s", WiFi.SSID().c_str());
	//		//DebugDisplay.AddTextLine(buf);

	//	}
	//	else
	//	{
	//		//DebugDisplay.AddTextLine("Not connected!");
	//	}
	//	//DebugDisplay.Update();
	//}

	if (success)
	{
		server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
			request->send(200, "text/html", "<b>Mobile Robot System</b> Control Stick Steering Module (MRS-CSSM)<br>Enter '[local IP address]/update' in the browser address bar to update firmware");
			});

		AsyncElegantOTA.begin(server);    // Start ElegantOTA
		server->begin();
		_PP("HTTP server started at ");
		IPAddress LocalIP = WiFi.localIP();
		_PP(LocalIP.toString());
		_PP(":");
		_PL(HTTPServerPort);

	}
	return success;
}

bool ESP32WiFiClass::EnterConfigPortal()
{
	char buf[32];
	bool success = false;

	// Replaced by callback function added to WiFiManager:
	//if (CSSMStatus.DebugDisplayStatus)
	//{
	//	DebugDisplay.AddTextLine("Start config portal");
	//	DebugDisplay.AddTextLine("MRS CSSM 192.168.4.1");
	//	DebugDisplay.Update();
	//}

	_PL("On-demand config portal start");
	wifiManager->resetSettings();
	delay(1000);
	success = wifiManager->startConfigPortal("MRS CSSMS3", "password");

	//if (CSSMStatus.DebugDisplayStatus)
	//{
	//	if (success)
	//	{
	//		snprintf(buf, 22, "Connected (%d dBm)", WiFi.RSSI());
	//		DebugDisplay.AddTextLine(buf);
	//		snprintf(buf, 22, "SSID: %s", WiFi.SSID().c_str());
	//		DebugDisplay.AddTextLine(buf);

	//	}
	//	else
	//	{
	//		DebugDisplay.AddTextLine("Not connected!");
	//	}
	//	DebugDisplay.Update();
	//}

	//DONE: Entering the config protal kills the server, so need to restart or re-initialize it...
	if (success)
	{
		server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
			request->send(200, "text/html", "<b>Mobile Robot System</b> Control Stick Steering Module (MRS-CSSM)<br>Enter '[local IP address]/update' in the browser address bar to update firmware");
			});

		AsyncElegantOTA.begin(server);    // Start ElegantOTA
		server->begin();
		_PP("HTTP server started at ");
		IPAddress LocalIP = WiFi.localIP();
		_PP(LocalIP.toString());
		_PP(":");
		_PL(HTTPServerPort);

	}

	return success;
}

void ESP32WiFiClass::configModeCallback(AsyncWiFiManager* myAsynchWiFiManager)
{
	//if (CSSMStatus.DebugDisplayStatus)
	//{
	//	DebugDisplay.AddTextLine("Start config portal");
	//	DebugDisplay.AddTextLine("MRS CSSM 192.168.4.1");
	//	DebugDisplay.Update();

	//}
	_PL("On-demand config portal start");
}

ESP32WiFiClass ESP32WiFi;
