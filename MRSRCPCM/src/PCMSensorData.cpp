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
	pinMode(V5SensePin, INPUT);
	pinMode(VBatSensePin, INPUT);
	pinMode(VBBatSensePin, INPUT);
	pinMode(VMCUSensePin, INPUT);
	pinMode(VExtSensePin, INPUT);

	if (!WSUPS3SINA219->begin())
	{
		_PL("INA219 initialization FAILED")
	}
	else
	{
		_PL("INA219 initialized")
	}

	return SetupADC();

}

void PCMSensorData::Update()
{
	INA219VBus = WSUPS3SINA219->getBusVoltage_V();
	INA219VShunt = WSUPS3SINA219->getShuntVoltage_mV();
	INA219Current = WSUPS3SINA219->getCurrent_mA();
	INA219Power = WSUPS3SINA219->getPower_mW();
	INA219VLoad = INA219VBus + (INA219VShunt / 1000.0f);
	
	// General voltage measurement function, assuming 1/2 voltage divider:
	//		V = ((float)rawADC / 4095.0) * 2.0 * 3.3 * (Vref / 1000.0);
	// Primary 18650 battery voltage scale adjustment (20240701)
	// System measures and displays 1.61 V while externally measured reading is 4.2 V
	// Primary battery sense circuit includes an op amp scaling buffer having a nominal gain of 0.60 (22k + 33k voltage divider (?))
	// 
	uint16_t rawADC = analogRead(VBatSensePin);
	VBat = ((float)rawADC / 4095.0) * 6.191 * (Vref / 1000.0);
	rawADC = analogRead(VBBatSensePin);
	VBBat = ((float)rawADC / 4095.0) * 1.841 * 3.3 * (Vref / 1000.0);
	rawADC = analogRead(VMCUSensePin);
	VMCU = ((float)rawADC / 4095.0) * 1.855 * 3.3 * (Vref / 1000.0);
	rawADC = analogRead(VExtSensePin);
	VExt = ((float)rawADC / 4095.0) * 7.161 * 3.3 * (Vref / 1000.0);
	rawADC = analogRead(V5SensePin);
	V5 = ((float)rawADC / 4095.0) * 1.807 * 3.3 * (Vref / 1000.0);
}

PCMSensorData SensorData;

