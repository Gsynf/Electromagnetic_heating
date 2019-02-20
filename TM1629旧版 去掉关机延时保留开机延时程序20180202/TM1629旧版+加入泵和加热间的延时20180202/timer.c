#include <STC12C52.H>
#include "timer.h"
#include "intrins.h"


sbit Time_Point = P3^7; // 时间中间的两点

/********************
* *名称：Timer_Init
* *功能：读键盘
*********************/
void Timer_Init(void)
{
	AUXR = 0xC0;
	TMOD = 0x00;
	TL0  = T1MS;
	TH0  = T1MS>>8;
	TR0  = 1;
	ET0  = 1;
	EA   = 1; 

}

/********************
* *名称：Time0_intrrupt
* *功能：定时器0中断
*********************/

void Time0_intrrupt() interrupt 1 
{
	static idata int count0 = 1000; // n ms	
	
	if (count0 == 1000)
	{	
		Time_Point = 1;
	}
	if (count0 == 2000)
	{	
		Time_Point = 0;
		count0 = 0;
	}
	count0 ++;
}



