/*
 Name:		MRSSENXIAOS3.ino
 Created:	3/17/2025 3:32:36 PM
 Author:	Mitchell Baldwin	copyright 2025
 
 Main executable for the Mobile Robot System Sensors module (MRS SEN)

 Controller: XIAO ESP32-S3

 Configuration notes:
	Board: Seeed Studio XIAO ESP32-S3 on a Seeed Studio XIAO Expansion Board 
	Local display: 128x64 monochrome oLED; Adafruit GFX & SSD1306

 v1.0	Initial release
 v1.1

*/

#include "src/DEBUG Macros.h"
#include <I2CBus.h>

#include "src/MRSSENStatus.h"
#include "src/MRSSENLocDisplay.h"
#include "src/MRSNavSensors.h"

void setup()
{
	char buf[32];
	
	Serial.begin(115200);
	_PL("")
	_PL("Initializing MRS Sensor module")
	
	//pinMode(BUILTIN_LED, OUTPUT);
	//digitalWrite(BUILTIN_LED, HIGH);

	// Initialize I2C bus:
	if (I2CBus.Init(DefaultSDA, DefaultSCL))
	{
		I2CBus.Scan();
		_PL(I2CBus.GetActiveI2CAddressesString());
	}
	else
	{
		_PL("Error initializing I2C bus...");
	}

	mrsSENStatus.LocDispStatus = mrsSENLocDisplay.Init();

	mrsNavSensors.Init();
	
	mrsSENLocDisplay.AddLine(1, "Testing...");
	//mrsSENLocDisplay.GetDisplay()->write(buf);
	//mrsSENLocDisplay.GetDisplay()->display();

}

void loop()
{
	
}
