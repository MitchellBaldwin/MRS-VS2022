/* RC2x15AMC.h
* RC2x15AMC class - Container class for working with the RoboClaw 2x 15A Motor Controller
*
* Mitchell Baldwin copyright 2024
* 
*/

#include "DEBUG Macros.h"
#include "RC2x15AMC.h"
#include "MCCStatus.h"

void RC2x15AMCClass::Init()
{
	char buf[64];

	RC2x15ASerial = new HardwareSerial(1);
	RC2x15A = new RoboClaw(RC2x15ASerial, 10000);

	RC2x15ASerial->begin(115200, SERIAL_8N1, 18, 17);
	if (!RC2x15ASerial)
	{
		MCCStatus.UART1Status = false;
		_PL("Failed to initialize RC2x15ASerial")
	}
	else
	{
		MCCStatus.UART1Status = true;

		// Test code:
		//RC2x15ASerial.println("Hello from the RC2x15ASerial port...");
	}

	// Test code:
	uint32_t maxI = 0;
	bool success = RC2x15A->ReadM1MaxCurrent(RC2x15AAddress, maxI);
	sprintf(buf, "%d %d", success, maxI);
	_PL(buf)

	RC2x15A->SpeedM1(RC2x15AAddress, 1000);
	delay(1000);
	RC2x15A->SpeedM1(RC2x15AAddress, 0);
	// End of test code block
}

bool RC2x15AMCClass::ReadStatus()
{
	uint16_t data1, data2;
	uint32_t data3, data4;
	int16_t data5, data6;
	bool success = false;

	data1 = RC2x15A->ReadMainBatteryVoltage(PSAddress, &success);
	MCCStatus.mcStatus.SupBatV = data1 / 10.0;

	success = RC2x15A->ReadTemp(PSAddress, data1);
	MCCStatus.mcStatus.Temp1 = data1 / 10.0;
	success = RC2x15A->ReadTemp2(PSAddress, data1);
	MCCStatus.mcStatus.Temp2 = data1 / 10.0;

	success = RC2x15A->ReadCurrents(PSAddress, data5, data6);
	MCCStatus.mcStatus.M1Current = (float)data5 / 100.0f;
	MCCStatus.mcStatus.M2Current = (float)data6 / 100.0f;

	//success = RC2x15A->ReadEncoders(PSAddress, data3, data4);
	//MCCStatus.mcStatus.M1Encoder = data3;
	//MCCStatus.mcStatus.M2Encoder = data4;

	//success = RC2x15A->ReadISpeeds(PSAddress, data3, data4);
	//MCCStatus.mcStatus.M1Speed = data3;
	//MCCStatus.mcStatus.M1Speed = data4;

	return success;
}

void RC2x15AMCClass::Drive(float throttle, float turnRate)
{
	uint32_t m1Speed = throttle / 100.0f * 7000.0f;
	uint32_t m2Speed = throttle / 100.0f * 7000.0f;
	
	RC2x15A->SpeedM1M2(PSAddress, m1Speed, m2Speed);

}

RC2x15AMCClass RC2x15AMC;
