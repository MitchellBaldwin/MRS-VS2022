/*
 Name:		MRSRCMCC.ino
 Created:	11/14/2023 12:04:31 PM
 Author:	Mitchell Baldwin	copyright 2023

 Main executable for the Mobile Rotot System Remote Controller Master Communications Computer (MRS RC MCC)

 v1.0	Initial release
 v1.1	

*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() 
{
	tft.init();
	tft.fillScreen(TFT_BLACK);

}

void loop() 
{
  
}
