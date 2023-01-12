/*
 Name:		SFTPPowMgmtBoard.ino
 Created:	11/16/2022 1:27:19 PM
 Author:	Mitchell Baldwin copyriht 2002
*/

// the setup function runs once when you press reset or power the board
#include <TaskSchedulerSleepMethods.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskScheduler.h>

// Select Debug and/or Test options here
#define _DEBUG_
//#define _TEST_

#ifdef _DEBUG_
#define _PP(a) Serial.print(a);
#define _PL(a) Serial.println(a);
#define _PLH(a) Serial.println(a, HEX);
#else
#define _PP(a)
#define _PL(a)
#define _PLH(a)
#endif

constexpr auto BuiltinLEDTogglePeriod = 500;

// Scheduler
Scheduler mainScheduler;

void ToggleBuiltinLEDCallback();
Task taskToggleBuiltinLED(BuiltinLEDTogglePeriod * TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &mainScheduler, false);

void setup() {

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}

void ToggleBuiltinLEDCallback()
{

}
