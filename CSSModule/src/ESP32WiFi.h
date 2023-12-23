// ESP32WiFi.h

#ifndef _ESP32WIFI_h
#define _ESP32WIFI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESPAsyncWiFiManager.h>
#include <AsyncElegantOTA.h>

class ESP32WiFiClass
{
 protected:


 public:
	AsyncWebServer* server;
	DNSServer* dns;
	AsyncWiFiManager* wifiManager;

	bool Init();

};

extern ESP32WiFiClass ESP32WiFi;

#endif

