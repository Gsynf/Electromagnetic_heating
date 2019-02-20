#include "TM1629.h"
#include "intrins.h"
#include "delay.h"
#include "watchdog.h"

#include <STC12C52.H>


/***********无小数点0-9数组***********/
static unsigned char undecode[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
/***********含小数点0-9数组***********/
static unsigned char withdp[10] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xDF};
/**********读键盘的值**********/
unsigned char g_key_value;

unsigned char* gp;

static void TM1629_SendByte(unsigned char value);
static void delay_nops ();

/************************************/
//函数名称：Init_TM1629（）
//函数功能：TM1629初始化
/************************************/
void InitDisplay (void)
{
	unsigned char idata i;
	
	STB = 1;
	CLK = 1;
	DIO = 0;

	delay_nops ();

	//设置显示控制命令
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0x80); //显示，最亮

	STB = 1;
	delay_nops ();

	//设置数据命令
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0x40); //固定地址写数据

	STB = 1;
	delay_nops ();

	//设置显示地址及数据
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0xC0);

	delay_nops ();

	for(i=0;i<16;i++)
	{
		TM1629_SendByte(0x00);

		delay_nops ();
	}

	STB = 1;
	delay_nops ();

	//设置显示控制命令
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0x8a); //显示，最亮

	STB = 1;
	delay_nops ();

}


/************************************/
//函数名称：Set_TM1629()
//函数功能：显示设置(digital_num为位码，
//  digital_value为所显示的数字)
/************************************/
void WriteWord (unsigned char digital_num, unsigned char digital_value)
{
	//设置数据命令
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0x44); //固定地址写数据

	STB = 1;
	_nop_(); _nop_();

	//设置显示地址及数据
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0xC1+(digital_num-1)*2);

//	TM1629_SendByte((undecode[digital_value]<<4)+(undecode[digital_value]>>4));
	TM1629_SendByte(digital_value); //undecode[digital_value]

	STB = 1;
	_nop_(); _nop_();

	//设置显示控制命令
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0x8F); //显示，最亮

	STB = 1;
	_nop_(); _nop_();

}


/************************************/
//函数名称：TM1629_SendByte()
//函数功能：向TM1629写入一个字节
/************************************/
static void TM1629_SendByte(unsigned char value)
{
	unsigned char idata i;

	for(i=0;i<8;i++)
	{
		CLK = 0;
		DIO = (value>>i)&0x01;
		delay_nops ();
		CLK = 1;
		delay_nops ();
	}
}


/************************************/
//函数名称：TM1629_ReadByte()
//函数功能：从TM1629读入一个字节
/************************************/
void TM1629_ReadByte(void)
{
	unsigned char idata i,key_data=0x00;
	DIO = 1;
	for (i=0;i<8;i++)
	{
		CLK = 0;
		key_data >>= 1;
		if (DIO)
		{
			key_data += 0x80;
		}
		delay_nops ();
		CLK = 1;
		delay_nops ();
	}
	g_key_value = key_data;
}


/************************************/
//函数名称：TM1629_ReadKey()
//函数功能：从TM1629读入键值
/************************************/
void TM1629_ReadKey (void)
{
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0x42); //读键扫数据

	delay_nops ();

	TM1629_ReadByte();
	_nop_(); _nop_();_nop_(); _nop_();
	
	STB = 1;
}


/************************************/
//函数名称：Delay()
//函数功能：延时12*一个机器周期=12*十二个时钟周期
/************************************/
static void delay_nops (void)
{
	_nop_(); _nop_();_nop_(); _nop_();
//	_nop_(); _nop_();_nop_(); _nop_();
//	_nop_(); _nop_();_nop_(); _nop_();	
}


void WriteWord2(unsigned char addr, unsigned char num)
{
	WriteWord (addr, undecode [num] );
}





static unsigned char  i;
/******************************
* *功能：
	(1) 系统开机后进入的状态，
    (2) 0 工作状态 （默认）
******************************/
static void disbase () {
  	
	for (i=2; i<=7; i++) { 
		if (is_heat_now && i==6)
			WriteWord (6, withdp [ *gp ]);
		else
			WriteWord2 (i, *gp);
		gp ++;
	}
}

void DefaultState (unsigned char mode)
{
	WriteWord (1, 0);
	gp = current_time;

	disbase ();
	Delay (7);
	if (mode == 0 && is_work == 0) WriteWord (8, 0);
}

void getGpSetMode () {
	gp = current_time;
	switch (key2_set_mode) {
	case 2:
		gp = set_start_time;
		break;
	case 3:
		gp = set_stop_time;
		break;
	case 4:
		gp = set_start_time2;
		break;
	case 5:
		gp = set_stop_time2;
		break;
	case 6:
		gp = set_start_time3;
		break;
	case 7:
		gp = set_stop_time3;
		break;
	}
}


/************************
* *名称：mode_lighting
* *功能：进入编辑模式时的闪烁
*************************/
void lighting_flash ()
{
	//时间设置为2~5位, 温度设置6~7位
	idata unsigned char addr = 2 + key3_shift_mode;  
	idata unsigned char nn = 0;

	if (key2_set_mode!=0){
		WriteWord(8, work_mode_dis[work_mode]);
	}
  	if (key2_set_mode == 8) {
		Delay (100);
		return;
	}

	getGpSetMode ();
	gp += key3_shift_mode;
		
	WriteWord2 (addr, *gp);
	
	Delay(100);
	WriteWord (addr, 0); 
	Delay(100);		
}

/********************************
 * *功能：

********************************/
void DisplaySetState () {

	WriteWord2 (1, key2_set_mode);
	gp = current_time;

	getGpSetMode ();
 	disbase ();
}	


