/*
 Name:		ESP32PowMgmtBoard.ino
 Created:	1/15/2024 4:26:28 PM
 Author:	Mitchell Baldwin copyright 2024

 Main executable for the Mobile Robot System Remote Controller Power Control Module (MRS RC PCM)

 v1.0	Initial release
 v1.1	
 v1.2	
 v1.3	

*/

#include <WiFi.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "src\DEBUG Macros.h"

#include <TaskSchedulerSleepMethods.h>
#include <TaskSchedulerDeclarations.h>
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
#include <TaskScheduler.h>
Scheduler MainScheduler;	// Main task scheduler

constexpr byte LED_BUILTIN = 0x02;
//constexpr auto HeartbeatLEDToggleInterval = 1000;

//void ToggleBuiltinLEDCallback();
//Task HeartbeatLEDTask(HeartbeatLEDToggleInterval* TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

#include <esp_adc_cal.h>
constexpr byte VIN_PIN = 34;				// TTGO T-Display supply voltage sense pin
int Vref = 1100;							// Default reference voltage for supply voltage measurement
constexpr auto MeasureVinInterval = 1000;

void MeasureVinCallback();
Task MeasureVinTask(MeasureVinInterval* TASK_MILLISECOND, TASK_FOREVER, &MeasureVinCallback, &MainScheduler, false);

void setup() 
{
	char buf[32];
	
	//pinMode(LED_BUILTIN, OUTPUT);
	_PL();
	_PLH(LED_BUILTIN);

	//HeartbeatLEDTask.enable();

	SetupADC();
	MeasureVinTask.enable();

	tft.init();
	tft.setRotation(2);
	tft.fillScreen(TFT_BLACK);
	
	//tft.setCursor();	// Used with 'printxx' statements; not needed when using drawString()
	tft.setTextSize(1);
	tft.setTextColor(TFT_RED, TFT_BLACK, false);
	tft.setTextDatum(TC_DATUM);
	tft.drawString("MRSRC PCM", tft.width() / 2, 2);

	// Measure & display supply voltage:
	uint16_t v = analogRead(VIN_PIN);
	float supply_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (Vref / 1000.0);
	tft.setCursor(2, tft.height() / 2);
	tft.setTextSize(2);
	tft.setTextColor(TFT_YELLOW);
	//tft.setTextDatum(CL_DATUM);
	tft.printf("%5.2F V", supply_voltage);
	
	//tft.setCursor(2, tft.height() / 2 + 24);
	tft.setTextSize(1);
	tft.setTextColor(TFT_GREENYELLOW);
	tft.setTextDatum(CL_DATUM);	//DONE: setTextDatum has NO AFFECT on print() output; print() effectively uses default TL_DATUM
	uint8_t mac[6];
	WiFi.macAddress(mac);
	//tft.printf("MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	sprintf(buf, "MAC:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	tft.drawString(buf, 2, tft.height() / 2 + 24);

	//tft.setCursor(2, tft.height() / 2 + 24);
	tft.setTextColor(TFT_GREEN);
	tft.setTextDatum(CL_DATUM);	//DONE: setTextDatum has NO AFFECT on print() output
	//tft.printf("IP: %s", WiFi.localIP().toString());
	sprintf(buf, "IP: %s", WiFi.localIP().toString());
	tft.drawString(buf, 2, tft.height() / 2 + 32);

	tft.drawRect(0, 0, tft.getViewportWidth(), tft.getViewportHeight(), TFT_DARKCYAN);
}

void loop() 
{
	MainScheduler.execute();
}

void ToggleBuiltinLEDCallback()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void SetupADC()
{
	char buf[64];
	
	esp_adc_cal_characteristics_t adc_chars;
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);    //Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		//Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
		sprintf(buf, "eFuse Vref:%u mV", adc_chars.vref);
		_PL(buf);
		Vref = adc_chars.vref;
	}
	else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		//Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
		sprintf(buf, "Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
		_PL(buf);
	}
	else {
		Serial.println("Default Vref: 1100mV");
		_PL("Default Vref: 1100mV");
	}
}

void MeasureVinCallback()
{
	// Measure & display supply voltage:
	uint16_t v = analogRead(VIN_PIN);
	float supply_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (Vref / 1000.0);
	tft.setCursor(2, tft.height() / 2);
	tft.setTextSize(2);
	tft.setTextColor(TFT_YELLOW, TFT_BLACK, true);	//DONE: Does bgfill = true work with the print() method? -> Yes, newly printed text clears the background
	tft.printf("%5.2F V", supply_voltage);

	// Debug code:
	char buf[32];
	sprintf(buf, "%6.2F V", supply_voltage);
	_PL(buf);

	// Test functionality of the backlight pin:
	//digitalWrite(TFT_BL, !digitalRead(TFT_BL));
}
