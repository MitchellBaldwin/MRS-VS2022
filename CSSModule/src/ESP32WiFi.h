// ESP32WiFi.h

#ifndef _ESP32WIFI_h
#define _ESP32WIFI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ESP32WiFiClass
{
 protected:


 public:
	bool Init();
};

extern ESP32WiFiClass ESP32WiFi;

extern AsyncWebServer server;
extern DNSServer dns;
extern AsyncWiFiManager wifiManager;

#endif

