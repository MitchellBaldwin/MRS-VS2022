/*
 Name:		SFTPPowMgmtBoard.ino
 Created:	11/16/2022 1:27:19 PM
 Author:	Mitchell Baldwin copyriht 2022
*/

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

constexpr auto HeartbeatLEDToggleInterval = 500;

// Scheduler
Scheduler MainScheduler;

void ToggleBuiltinLEDCallback();
Task HeartbeatLEDTask(HeartbeatLEDToggleInterval * TASK_MILLISECOND, TASK_FOREVER, &ToggleBuiltinLEDCallback, &MainScheduler, false);

void setup() {

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}

void ToggleBuiltinLEDCallback()
{

}
