#pragma once
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
