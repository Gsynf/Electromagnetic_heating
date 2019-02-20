
#include <STC12C52.H>

#include "systemInit.h"
#include "timer.h"

/********************
* *���ƣ��ⲿ�жϳ�ʼ������Ӧ4��������
* *���ܣ�������
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
* *���ƣ�io_configure
* *���ܣ�IO������
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
	ADC_CONTR |= 0x80;              //1000,0000 �� A/D ת����Դ

	Timer_Init();

//	Key_Int_Init();
}
