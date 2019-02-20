
#include <STC12C52.H>

#include "systemInit.h"
#include "timer.h"

/********************
* *名称：外部中断初始化（对应4个按键）
* *功能：读键盘
*********************/
/*static void Key_Int_Init(void)
{
	INT0      = 1;
	EX0       = 1;
	INT1      = 1;
	EX1       = 1;
	IT0       = 1;
	IT1       = 1;
   	INT_CLKO |= 0x10;
	INT_CLKO |= 0x20;
	EA        = 1;
}  */


/********************
* *名称：io_configure
* *功能：IO口配置
*********************/
static void io_configure(void)
{
	P0M0 = 0x00;
	P0M1 = 0x00;

	P1M0 = 0x00;
	P1M1 = 0x04;

	P2M0 = 0x00;
	P2M1 = 0x6E;

	P3M0 = 0x00;
	P3M1 = 0x00;
}

void system_init ()
{
	io_configure();
	ADC_CONTR |= 0x80;              //1000,0000 打开 A/D 转换电源

	Timer_Init();

//	Key_Int_Init();
}
