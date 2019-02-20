#include "watchdog.h"


/********************
* *Ãû³Æ£ºwatch_dog
* *¹¦ÄÜ£º
*********************/
void watch_dog(void)
{
	WDT_CONTR = 0x3F;
}