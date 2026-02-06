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

	pinMode(defaultVBBAKPin, INPUT);
	VBBAK.Init(0, 57, 40960, "V");

	// Initialize digital input to sense TS3 power supply presence:
	pinMode(TS3MCSupplySensePin, INPUT);
	sprintf(buf, "TS3 MC Supply Sense on GPIO%02D: %02D", TS3MCSupplySensePin, digitalRead(TS3MCSupplySensePin));
	_PL(buf)

	//if (!WSUPS3SINA219->begin())
	//{
	//	WSUPS3SINA219->setCalibration_32V_2A();	// Configure for 32V, 2A operation

	//	_PL("INA219 initialization FAILED")
	//}
	//else
	//{
	//	_PL("INA219 initialized")
	//}

	WSUPS3SINA219->begin();
	WSUPS3SINA219->setCalibration_32V_2A();	// Configure for 32V, 2A operation
	if (WSUPS3SINA219->getBusVoltage_V() > 0.0)
	{
		MCCStatus.WSUPS3SINA219Status = true;
		_PL("WS UPS 3S INA219 initialized")
	}
	else
	{
		MCCStatus.WSUPS3SINA219Status = false;
		_PL("WS UPS 3S INA219 initialization FAILED")
	}

	//TODO: Check integrity of INA219 initialization so we can avoid delays in Update() if it failed


	// Initialize BME680 sensor:
	BME680 = new BME680_Class();
	MCCStatus.BME680Status = BME680->begin(Wire.getClock(), defaultBME680Address);
	if (!MCCStatus.BME680Status)
	{
		//MCCStatus.BME680Status = false;
		_PL("Failed to initialize BME680 sensor")
	}
	else
	{
		BME680->setOversampling(TemperatureSensor, Oversample16);	// Use enumerated type values
		BME680->setOversampling(HumiditySensor, Oversample16);		// Use enumerated type values
		BME680->setOversampling(PressureSensor, Oversample16);		// Use enumerated type values
		BME680->setIIRFilter(IIR4);									// Use enumerated type values
		BME680->setGas(320, 150);									// 320?c for 150 milliseconds
		//MCCStatus.BME680Status = true;
	}

	MRSSENsors = new MRSSENsorsClass();
	MCCStatus.MRSSENModuleStatus = MRSSENsors->Init();

	bool success = MCCStatus.WSUPS3SINA219Status && MCCStatus.BME680Status;
	return success;
}

void MCCSensors::Update()
{
	char buf[64];
	uint8_t data[sizeof(MRSSensorPacket)];

	static int32_t temp, rh, pbaro, gas;
	static uint32_t loopCounter = 0;

	if (MCCStatus.BME680Status)
	{
		// Start a measurement the first time through:
		if (!loopCounter)
		{
			BME680->getSensorData(temp, rh, pbaro, gas, true);				// Setting waitSwitch = false to read asynchronously?
		}
		// Read environment sensors at a lower frequency (e.g., 200 ms x 10 x 2 = 4000 ms sampling period):
		if (loopCounter++ % 10 == 0)
		{
			// If not measuring then assume the last measurement period has ended:
			MCCStatus.mrsSensorPacket.BME680Temp = (float)temp / 100.0f;	// Convert from centidegrees
			MCCStatus.mrsSensorPacket.BME680RH = (float)rh / 1000.0f;		// Convert from milli-%
			MCCStatus.mrsSensorPacket.BME680Pbaro = (float)pbaro / 100.0f;	// Convert from Pa to hPa
			MCCStatus.mrsSensorPacket.BME680Gas = (float)gas / 100.0f;		// Convert from milliohms (?)
			MCCStatus.mrsSensorPacket.BME680Alt = BME680Altitude(pbaro);	// m
			BME680->getSensorData(temp, rh, pbaro, gas, false);				// Setting waitSwitch = false to read asynchronously
		}
	}

	// Update at main task frequency:
	uint16_t newReading = analogRead(defaultVMCUPin);					// ADC counts
	//int calibratedReading = ReadCalibratedADC1(newReading);			// mV
	//_PL(calibratedReading)
	VMCU.AddReading(newReading);
	newReading = analogRead(defaultVBBAKPin);							// ADC counts
	VBBAK.AddReading(newReading);

	if (MCCStatus.WSUPS3SINA219Status)
	{
		MCCStatus.mrsSensorPacket.INA219VBus = WSUPS3SINA219->getBusVoltage_V();
		MCCStatus.mrsSensorPacket.INA219VShunt = WSUPS3SINA219->getShuntVoltage_mV();
		MCCStatus.mrsSensorPacket.INA219Current = WSUPS3SINA219->getCurrent_mA();
		MCCStatus.mrsSensorPacket.INA219Power = WSUPS3SINA219->getPower_mW();
		MCCStatus.mrsSensorPacket.INA219VLoad = MCCStatus.mrsSensorPacket.INA219VBus + (MCCStatus.mrsSensorPacket.INA219VShunt / 1000.0f);
	}

	// Get sensor packet from MRS SEN module over I2C:
	MCCStatus.MRSSENModuleStatus = MRSSENsors->TestI2CConnection();
	if (MCCStatus.MRSSENModuleStatus)
	{
		MRSSENsors->Update();
		MRSSensorPacket senPacket;
		MRSSENsors->getMRSSensorPacket(senPacket);

		MCCStatus.mrsSensorPacket.FWDVL53L1XRange = senPacket.FWDVL53L1XRange;
		MCCStatus.mrsSensorPacket.ODOSPosX = senPacket.ODOSPosX;
		MCCStatus.mrsSensorPacket.ODOSPosY = senPacket.ODOSPosY;
		MCCStatus.mrsSensorPacket.ODOSHdg = senPacket.ODOSHdg;
		MCCStatus.mrsSensorPacket.TurretPosition = senPacket.TurretPosition;
	
		// Test code:
		MCCStatus.mrsSensorPacket.FWDVL53L1XRange = MRSSENsors->GetFWDLIDARRangeMM();
	}
	else
	{
		_PL("MRS SEN I2C connection FAILED")
		//return;
	}


}

bool MCCSensors::TestMRSSENCommunication()
{
	bool success = false;

	CSSMCommandPacket testPacket;
	testPacket.command = CSSMCommandPacket::CSSMCommandCodes::SetTurretPosition;
	testPacket.turretPosition = 800;

	Wire.beginTransmission(defaultMRSSENAddress);
	size_t bytesWritten = Wire.write((uint8_t*)&testPacket, sizeof(CSSMCommandPacket));
	Wire.endTransmission();

	if (bytesWritten != sizeof(CSSMCommandPacket))
	{
		_PL("MRS Sensors I2C write FAILED")
		success = false;
	}
	else
	{
		success = true;
		_PL("MRS Sensors I2C write test successful")
	}
	
	return success;
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

uint16_t MCCSensors::GetBBAKRawADC()
{
	return VBBAK.GetAverageRawValue();
}

float MCCSensors::GetBBAKVoltageReal()
{
	return VBBAK.GetAverageRealValue();
}

String MCCSensors::GetBBAKVoltageString()
{
	return VBBAK.GetRealString();
}

String MCCSensors::GetBBAKVoltageString(String format)
{
	return VBBAK.GetRealString(format);
}


MCCSensors mccSensors;

