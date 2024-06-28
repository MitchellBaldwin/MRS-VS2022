/*	PCMSensorData.h
*	PCMSensorData - Base class for managing data from sensors on the MRS Power Control Module
*
*	Mitchell Baldwin copyright 2023-2024
*
*/

#include "PCMSensorData.h"
#include "DEBUG Macros.h"
#include <esp_adc_cal.h>

bool PCMSensorData::SetupADC()
{
	char buf[64];

	esp_adc_cal_characteristics_t adc_chars;
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);    //Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		sprintf(buf, "eFuse Vref:%u mV", adc_chars.vref);
		_PL(buf);
		Vref = adc_chars.vref;
	}
	else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		sprintf(buf, "Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
		_PL(buf);
	}
	else {
		Serial.println("Default Vref: 1100mV");
		_PL("Default Vref: 1100mV");
	}

	return true;
}

PCMSensorData::PCMSensorData()
{
}

bool PCMSensorData::Init()
{
	pinMode(VBatSensePin, INPUT);
	pinMode(VBBatSensePin, INPUT);
	pinMode(VExtSensePin, INPUT);

	return SetupADC();
}

void PCMSensorData::Update()
{
	uint16_t rawADC = analogRead(VBatSensePin);
	VBat = ((float)rawADC / 4095.0) * 2.0 * 3.3 * (Vref / 1000.0);
	rawADC = analogRead(VBBatSensePin);
	VBBat = ((float)rawADC / 4095.0) * 2.0 * 3.3 * (Vref / 1000.0);
	rawADC = analogRead(VExtSensePin);
	VExt = ((float)rawADC / 4095.0) * 2.0 * 3.3 * (Vref / 1000.0);
}

PCMSensorData SensorData;

