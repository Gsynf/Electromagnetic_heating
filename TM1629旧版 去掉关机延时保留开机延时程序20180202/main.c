#include "Delay.h"
#include "intrins.h"
#include "key.h"
#include "TM1629.h"
#include "eeprom.h"
#include "systemInit.h"
#include "work_mode.h"
#include "watchdog.h"
#include "DS1302.h"


#include <STC12C52.H>

volatile unsigned char first_read_temp = 1;

/**** AD 转换全局变量 ************************/
// AD 采样 对应显示2位温度值， 
volatile unsigned int current_temp = 0; //当前温度值
unsigned char idata gSetTmpH = 5,
	gSetTmpL = 0;

/**** 按键控制 全局变量 ************************/
volatile unsigned char key1_mode = 0,         //按键1，开关键
			key2_set_mode = 0,   //按键2，设置模式： 0 ~ 5
			key3_shift_mode = 0; //按键3，移位模式
volatile unsigned char work_mode = 4;       //设定模式5，对应的工作模式值;
//0:非定时模式I档;1:定时模式I档;2:非定时模式II档;3:定时模式II档;4:非定时模式III档;5:定时模式III档;
volatile unsigned char is_heat_now = 0;		//是否处在加热状态，控制右上角的LED是否点亮
volatile unsigned char need_keep_temp = 0;		//是否需要保温（第一次升温不需要保温，第二次温度下降中要保温）
volatile unsigned char is_work = 0;				//是否处在工作状态（控制定时模式下的显示问题）
			
			
/********* 时间 *************************
	0: 小时高位；
	1：小时低位；
	2：分钟高位；
	3：分钟低位；
	4: 当前温度或设定温度的高位
	5：当前温度或设定温度的低位 
    ===================================*/
volatile unsigned char current_time [6]= {0,0,0,0,5};   //当前时间值（设置）
volatile unsigned char set_start_time [6]= {0,0,0,0,5}; //设定起始时间
volatile unsigned char set_stop_time [6];  //设定结束时间
volatile unsigned char idata set_start_time2 [6] = {0,0,0,0,5}; //设定起始时间2
volatile unsigned char idata set_stop_time2 [6] = {0,0,0,0};  //设定结束时间2
volatile unsigned char idata set_start_time3 [6] = {0,0,0,0,5}; //设定起始时间3
volatile unsigned char idata set_stop_time3 [6] = {0,0,0,0};  //设定结束时间3
unsigned char idata work_mode_dis [6] = {0x08, 0x68, 0x10, 0x70, 0x18, 0x78};
				

/************马达停止标志*****************/
/***********************************************
motor_num: 
0x00: 正常工作，
0xFF: 驱动板停止工作，但是电机仍然工作，一分钟后停止
***********************************************/
volatile unsigned char motor_num = 0;


//采样周期
volatile unsigned char idata sample_period = 50;

//时钟数据地址 格式为: 秒 分 时 日 月 星期 年 控制   
unsigned char idata time_init[7]= {0x00,0x00,0x18,0x01,0x01,0x07,0x12};
volatile unsigned char have_set_time = 0;  //是否设置了初始化时间


static unsigned int idata num = 100,
	num2 = 1000;
void detect () {
	if (num -- == 0)	{
		enter_protect_mode ();     //异常报警检测
		num = 100;
	}
	if (num2 -- == 0)	{
		InitDisplay();             //初始化tm1629
		num2 = 1000;
	}
}

/********************
* *名称：main
* *功能：
*********************/
void main()
{
	// sleep ...
	Delay (100); //	 100*0.1ms

	//初始化
	system_init ();								//初始化端口 
	InitDisplay();                  			//初始化TM1629
	clear_switch ();                			//清空驱动状态信息
	Motor_CTL = CLOSE_MOTOR_SPEAKER;      		//关闭泵
	
	//从eeprom中读取设定的温度与时间值
	read_eeprom_time_temp();
	 
	if (have_set_time != 1) {  //第一写程序时要对时间芯片进行初始化，以后上电便不必在初始化
		Set1302M(time_init); 
		have_set_time = 1;
	}
	Delay (100);
	while(1)
	{
		
		Speaker_CTL = CLOSE_MOTOR_SPEAKER;
		watch_dog();
		ad_sample ();

		if (key1_mode == 0) { //关机状态，等待开机。 0:表示关机状态;1:表示工作状态;2:表示关机操作
			key_scan ();
			key2_set_mode = 0;
			DefaultState (0);
			
        	detect();
			
			//超过温度上限，motor_num = 0xFF
			//重新加热的时候，启动泵；超过温度上限时，等待一分钟停止泵
			motor_timer ();	

			continue;
		}

		if ( key1_mode == 2) { //关机操作
			//关机时写入时间和温度的设定值
			write_eeprom_time_temp () ; //此句没有必要，可删去
			switch_off ();
			Speaker_CTL = CLOSE_MOTOR_SPEAKER;
			motor_num = 0xFF;  //关机标志

			key1_mode = 0;   

			continue;
		}
		
		key_scan ();	//key1_mode == 1时，扫描开关键
        motor_timer ();	//判断是否关泵
		
		detect();
		
		switch ( key2_set_mode )
		{
		case 0://空模式
			DisplaySetState ();
			enter_work_mode ();      //进入工作模式(第一个LED显示0的模式)
			break;
			
		case 1:		//设置当前时间和默认温度
		case 2: 	//设定起始定时时间1		
		case 3: 	//设定终止定时时间1
		case 4:		//设定起始定时时间2
		case 5: 	//设定终止定时时间2
		case 6:		//设定起始定时时间3 	
		case 7: 	//设定终止定时时间3
		case 8:	    //设定工作模式
		case 9:		//
			lighting_flash ();	//闪烁
			DisplaySetState ();	
			break;
		}		

	}
}


