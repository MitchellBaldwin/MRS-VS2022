/*	ESP32WiFi.h
*	ESP32WiFiClass - Base class for managing WiFi connectivity for ESP32-based MRS modules
*
*
*
*
*	Mitchell Baldwin copyright 2024
*
*	v 0.00:	Initial data structure
*	v
*
*/
#ifndef _ESP32WIFI_h
#define _ESP32WIFI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESPAsyncWiFiManager.h>
//#include <AsyncElegantOTA.h>

class ESP32WiFiClass
{
 protected:
	 uint16_t HTTPServerPort = 80;

 public:
	AsyncWebServer* server;
	DNSServer* dns;
	AsyncWiFiManager* wifiManager;

	bool Init(bool);
	bool EnterConfigPortal();
	static void configModeCallback(AsyncWiFiManager* myAsynchWiFiManager);
};

extern ESP32WiFiClass ESP32WiFi;

#endif

