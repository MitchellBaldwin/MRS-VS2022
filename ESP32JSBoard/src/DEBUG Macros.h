#pragma once
// Select Debug and/or Test options here
#define _DEBUG_
//#define _TEST_

#ifdef _DEBUG_
extern HardwareSerial USBSerial;
#define _PP(a) USBSerial.print(a);
#define _PL(a) USBSerial.println(a);
#define _PLH(a) USBSerial.println(a, HEX);
#else
#define _PP(a)
#define _PL(a)
#define _PLH(a)
#endif
