#include "watchdog.h"


/********************
* *���ƣ�watch_dog
* *���ܣ�
*********************/
void watch_dog(void)
{
	WDT_CONTR = 0x3F;
}