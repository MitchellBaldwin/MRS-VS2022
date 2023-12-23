// 
// 
// 

#include "ESP32WiFi.h"
#include "DEBUG Macros.h"

bool ESP32WiFiClass::Init()
{
	//TODO: Set CSSMStatus flag to reflect WiFi connection success & state
	//TODO: Implement a hardware means to force the WiFi Manager into configuration mode to allow selection of a different network

	server = new AsyncWebServer(80);
	dns = new DNSServer();
	wifiManager = new AsyncWiFiManager(server, dns);

	bool success = wifiManager->autoConnect("MRS CSSM", "password");

	server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(200, "text/html", "<b>Mobile Robot System</b> Control Stick Steering Module (MRS-CSSM)<br>Enter '[local IP address]/update' in the browser address bar to update firmware");
		});

	AsyncElegantOTA.begin(server);    // Start ElegantOTA
	server->begin();
	_PP("HTTP server started at ");
	IPAddress LocalIP = WiFi.localIP();
	_PP(LocalIP.toString());
	_PP(":");
	_PL();

	return success;
}


ESP32WiFiClass ESP32WiFi;

