#include "delay.h"
#include "watchdog.h"

/********************
* *名称：Delay
* *功能：延时
*********************/
void Delay(unsigned int delay_time)   
{
	unsigned int n;
    while(delay_time--) 
    { 
        n = 980;              //0.1ms
        while(--n);

		watch_dog();
    }
}

void Delay10uS(char nn)   
{
	char n;
	while (--nn >= 0)
	{
	    n = 20;              //0.002ms
	    while(--n); 
 	}
}
