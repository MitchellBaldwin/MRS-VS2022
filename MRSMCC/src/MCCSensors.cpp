/*	MCCSensors.h
*	MCCSensors - Manage MRC Master Communication & Coltrol module sensors
*
*/

#include "MCCSensors.h"
#include "DEBUG Macros.h"
#include "MCCStatus.h"

float MCCSensors::BME680Altitude(const int32_t press, const float seaLevel)
{
	float alt = 44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));	// Convert into meters
	return alt;
}

bool MCCSensors::SetupADC()
{
	char buf[64];

	esp_adc_cal_characteristics_t adc_chars;
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);    //Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		sprintf(buf, "eFuse Vref:%u mV", adc_chars.vref);
		_PL(buf);
		VRef = adc_chars.vref;
	}
	else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		sprintf(buf, "Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
		_PL(buf);
	}
	else {
		ADC1Chars.vref = VRef;
		_PL("Default Vref: 1100 mV");
	}

	return true;
}

uint32_t MCCSensors::ReadCalibratedADC1(int rawADC1)
{
	return  esp_adc_cal_raw_to_voltage(rawADC1, &ADC1Chars);
}

bool MCCSensors::Init()
{
	// Initialize analog measurements:
	SetupADC();
	pinMode(defaultVMCUPin, INPUT);
	VMCU.Init(0, 66, 40960, "V");

	if (!WSUPS3SINA219->begin())
	{
		_PL("INA219 initialization FAILED")
	}
	else
	{
		_PL("INA219 initialized")
	}

	// Initialize BME680 sensor:
	BME680 = new BME680_Class();
	bool success = BME680->begin(Wire.getClock(), defaultBME680Address);
	if (!success)
	{
		_PL("Failed to initialize BME680 sensor")

	}
	else
	{
		BME680->setOversampling(TemperatureSensor, Oversample16);	// Use enumerated type values
		BME680->setOversampling(HumiditySensor, Oversample16);		// Use enumerated type values
		BME680->setOversampling(PressureSensor, Oversample16);		// Use enumerated type values
		BME680->setIIRFilter(IIR4);									// Use enumerated type values
		BME680->setGas(320, 150);									// 320?c for 150 milliseconds
	}

	// Initialize VL53L1X distance sensor:

	// Initialoze Odometry sensor:


	return success;
}

void MCCSensors::Update()
{
	char buf[32];
	static int32_t temp, rh, pbaro, gas;
	static uint32_t loopCounter = 0;

	// Start a measurement the first time through:
	if (!loopCounter)
	{
		BME680->getSensorData(temp, rh, pbaro, gas, true);				// Setting waitSwitch = false to read asynchronously?
	}
	// Read environment sensors at a lower frequency (e.g., 200 ms x 10 x 2 = 4000 ms sampling period):
	if (loopCounter++ % 10 == 0)
	{
		// If not measuring then assume the last measurement period has ended:
		MCCStatus.mrsSensorPacket.BME280Temp = (float)temp / 100.0f;	// Convert from centidegrees
		MCCStatus.mrsSensorPacket.BME280RH = (float)rh / 1000.0f;		// Convert from milli-%
		MCCStatus.mrsSensorPacket.BME280Pbaro = (float)pbaro / 100.0f;	// Convert from Pa to hPa
		MCCStatus.mrsSensorPacket.BME280Gas = (float)gas / 100.0f;		// Convert from milliohms (?)
		MCCStatus.mrsSensorPacket.BME280Alt = BME680Altitude(pbaro);	// m
		BME680->getSensorData(temp, rh, pbaro, gas, false);				// Setting waitSwitch = false to read asynchronously
	}

	// Update at main task frequency:
	uint16_t newReading = analogRead(defaultVMCUPin);					// ADC counts
	//int calibratedReading = ReadCalibratedADC1(newReading);			// mV
	//_PL(calibratedReading)
	VMCU.AddReading(newReading);

	MCCStatus.mrsSensorPacket.INA219VBus = WSUPS3SINA219->getBusVoltage_V();
	MCCStatus.mrsSensorPacket.INA219VShunt = WSUPS3SINA219->getShuntVoltage_mV();
	MCCStatus.mrsSensorPacket.INA219Current = WSUPS3SINA219->getCurrent_mA();
	MCCStatus.mrsSensorPacket.INA219Power = WSUPS3SINA219->getPower_mW();
	MCCStatus.mrsSensorPacket.INA219VLoad = MCCStatus.mrsSensorPacket.INA219VBus + (MCCStatus.mrsSensorPacket.INA219VShunt / 1000.0f);



}

uint16_t MCCSensors::GetMCURawADC()
{
	return VMCU.GetAverageRawValue();
}

float MCCSensors::GetMCUVoltageReal()
{
	return VMCU.GetAverageRealValue();
}

String MCCSensors::GetMCUVoltageString()
{
	return VMCU.GetRealString();
}

String MCCSensors::GetMCUVoltageString(String format)
{
	return VMCU.GetRealString(format);
}


MCCSensors mccSensors;

