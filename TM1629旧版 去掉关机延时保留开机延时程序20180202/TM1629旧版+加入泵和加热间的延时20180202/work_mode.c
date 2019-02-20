#include "work_mode.h"
#include "TM1629.h"
#include "delay.h"
#include "watchdog.h"
#include "DS18B20.h"
#include "DS1302.h"
#include "adc.h"


//温度上限转化值
static unsigned int up_temp; //温度上限

static unsigned char _i;


/********************
* *名称：
* *功能：关闭四个驱动（右侧）
*********************/
void switch_off(void)
{
	clear_switch ();
	Motor_CTL = CLOSE_MOTOR_SPEAKER;	
	need_keep_temp = 0;
}

static unsigned char is_open_or_close_switch = 0;	//是否是打开或者关闭驱动
static void switch_delay () {
	if (is_open_or_close_switch == 1) {
		Delay (200);
	}
}

/***************************
* * 功能：清空驱动
* *
**************************/
void clear_switch () {
 	if (is_heat_now == 1) is_open_or_close_switch = 1;
	
	CTL1 = CLOSE_CTL;
	switch_delay ();
	CTL2 = CLOSE_CTL;
	switch_delay ();
	CTL3 = CLOSE_CTL;
	switch_delay ();
	CTL4 = CLOSE_CTL;

	//停止加热:
	is_heat_now = 0;
	is_open_or_close_switch = 0;
}

static void heat_mode() {
	switch (work_mode / 2) {
	case 0:	//1档加热
		CTL1 = OPEN_CTL;
		switch_delay ();
		CTL2 = OPEN_CTL;
		switch_delay ();
		CTL3 = CLOSE_CTL;
		switch_delay ();
		CTL4 = CLOSE_CTL;	
		break;

	case 1: //2档加热
		CTL1 = OPEN_CTL;
		switch_delay ();
		CTL2 = OPEN_CTL;
		switch_delay ();
		CTL3 = OPEN_CTL;
		switch_delay ();
		CTL4 = CLOSE_CTL;
		break;

	case 2: //3档加热
		CTL1 = OPEN_CTL;
		switch_delay ();
		CTL2 = OPEN_CTL;
		switch_delay ();
		CTL3 = OPEN_CTL;
		switch_delay ();
		CTL4 = OPEN_CTL;
		break;
	}
}
//加热
static void heat ()
{
	//
	if (current_temp > up_temp)  	{ //停止加热
		clear_switch ();
		motor_num = 0xFF;
		need_keep_temp = 1;
	}
	else {
		
		if (current_temp >= (up_temp-7)) {
			if (need_keep_temp) { //判断是否需要保温
				clear_switch ();
				Motor_CTL = CLOSE_MOTOR_SPEAKER;
				return;
			}
		}
		need_keep_temp = 0;

		if (is_heat_now == 0) is_open_or_close_switch = 1;
		if(Motor_CTL == 0)
		{
			Motor_CTL = OPEN_MOTOR_SPEAKER; 	//打开泵
			Delay(5000);
			heat_mode();
		}
		//加热	
		is_heat_now = 1;	//正在加热
		
		is_open_or_close_switch = 0;

    }
}


/*************************************
* *名称：enter_timing_mode
* *功能：定时模式。判断在设定时间之内工作
*************************************/
static void among_work_time () {
	is_work = 1;
	heat ();
}

static void not_among_work_time (unsigned char open)
{
	if (key1_mode == 0) WriteWord (8, 0);
	else WriteWord (8,open+0x20);
	clear_switch ();
	Motor_CTL = CLOSE_MOTOR_SPEAKER;	//不在工作时间内时，关闭泵
	is_work = 0;
}

static unsigned int get_itime (unsigned char* gp)
{
	return ( (*gp++)*10 + (*gp++) ) * 60 + (*gp++)*10 +  (*gp++);
}

static unsigned int idata s_timer, e_timer, 
	s_timer2, e_timer2, 
	s_timer3, e_timer3,
	time;
static unsigned char is_among_time = 0;
static void IsAmongTime (unsigned int st, unsigned int et, unsigned char nn)
{
   	if (et >= st) {
		if ((time >= st)&&(time < et))	
			is_among_time = nn;	
	}
	else {
		if (!((time > et)&&(time < st)))
			is_among_time = nn;
	}

}

static void enter_timing_mode (unsigned char open)
{
	is_among_time = 0;
	s_timer = get_itime (set_start_time);
	e_timer = get_itime (set_stop_time);
	s_timer2 = get_itime (set_start_time2);
	e_timer2 = get_itime (set_stop_time2);
	s_timer3 = get_itime (set_start_time3);
	e_timer3 = get_itime (set_stop_time3);

	time = get_itime (current_time);

	IsAmongTime (s_timer3, e_timer3, 1);
	IsAmongTime (s_timer2, e_timer2, 2);
	IsAmongTime (s_timer, e_timer, 3);

	up_temp = set_start_time[4]*10 + set_start_time[5];
	if (is_among_time == 1) {
		up_temp = set_start_time3[4]*10 + set_start_time3[5];
	} else if (is_among_time == 2) {
		up_temp = set_start_time2[4]*10 + set_start_time2[5];
	}
	if (is_among_time) {
		WriteWord (8, open);
		among_work_time ();
	} else {
		not_among_work_time (open);	
	}
}

/*******************************
* *名称：work_mode
* *功能：模式选择为0时，默认工作状态
********************************/
void enter_work_mode ()
{
	// 默认温度上限值
	up_temp = gSetTmpH * 10 + gSetTmpL;

	switch ( work_mode )
	{
	//定时模式
	case 1: //一档定时模式
		enter_timing_mode (0x28);	
		return;
	case 3: //二档定时模式
		enter_timing_mode (0x30);
		return;
	case 5: //三档定时模式
		enter_timing_mode (0x38);
		return;

	 //非定时模式			
	case 0: //一档非定时模式
	case 2: //二档非定时模式
	case 4: //三档非定时模式
		WriteWord(8, work_mode_dis[work_mode]);
		heat ();
		break;								
	}	

}

void protect_displsy ()
{
	ad_sample ();  //采集当前温度值
	if (key1_mode == 0) {
		DefaultState (1);
		return;
	}

 	key2_set_mode = 0;	
	DisplaySetState ();
}

void protect_proc ()
{
	clear_switch ();
	Motor_CTL = CLOSE_MOTOR_SPEAKER;
	Speaker_CTL = OPEN_MOTOR_SPEAKER;
	watch_dog();

	protect_displsy ();
}

void enter_protect_mode () {
//	static unsigned char is_first_log = 0; //第一次上电

	while (AD_Filter2(1)>15)//10)//漏电保护;15对应约XXV
	{
		WriteWord (8,0x01);
		protect_proc ();
	}  
	

//	while (AD_Filter2(1)>85)     //70 //超温保护	对应大约85度
	while (RD_T(1) > 85)
	{
		WriteWord (8,0x80);
		protect_proc ();
	} 

   
//	if (AD_Filter2(2)<16)   	//14)//22)     //防冻保护 
	if (current_temp < 3)
	{
		clear_switch (); //多余	

//		while (AD_Filter2(2)<20)//<18)  	//重新启动，防止温度过低，冻坏电暖气
		while (current_temp < 8) //10)
	    {
			is_heat_now = 1;
            Motor_CTL = OPEN_MOTOR_SPEAKER;              	//打开泵

			heat_mode();

			//如果是第一次上电，则所有档位均打开，以加快加热
//			if (is_first_log == 0) {
//				CTL1=OPEN_CTL; 
//				Delay (100);
//				CTL2=OPEN_CTL; 
//				Delay (100);
//				CTL3=OPEN_CTL; 
//				Delay (100);
//				CTL4=OPEN_CTL;
//				is_first_log = 1;
//			}
//			//如果不是第一次上电，则只开1档位
//			else CTL1=OPEN_CTL;

			Speaker_CTL = OPEN_MOTOR_SPEAKER;
			watch_dog();
			WriteWord (8,0x04);
			protect_displsy ();
		}
	
		Speaker_CTL = CLOSE_MOTOR_SPEAKER;			//关闭蜂鸣器
        Motor_CTL = CLOSE_MOTOR_SPEAKER;      		//关闭泵
		clear_switch ();							//关闭所有加热			

		//当用户重新在5模式设置了加热档次后，才会触发到正常工作模式
		//，否则降温后将再次进入防冻保护状态

		//修改  ----  如果是定时模式，则开始烧起来
		//      ----  如果是非定时模式，则清空工作模式
//		if (work_mode % 2 == 0) {	//非定时模式
//			work_mode = 0xFF;   
//		}
	}  
	
	while (AD_Filter2(0)>=8)//25)//漏水保护
	{
		WriteWord (8,0x02);
		protect_proc ();
	} 
 
	//add:
//	if (work_mode == 0xFF)		//非定时模式
//		WriteWord (8,0x00);	
	if (work_mode % 2 == 1)	{	//定时模式
		if (key1_mode == 0) { 	//关机状态
			enter_work_mode ();
		}
	}
	
}



void motor_timer (void)
{
	if (motor_num == 0xFF)//检测关机标志，等待大约1分钟后停止马达工作
	{
		Delay (5);
		_i += 1;
		if (_i == 200)//218)
		{
			Motor_CTL = CLOSE_MOTOR_SPEAKER;
			_i = 0;
			motor_num = 0;
		}
	}
}


void ad_sample ()
{
	unsigned int idata tmp;
	if (--sample_period == 0 || key2_set_mode) {
		//采集当前温度值
		current_temp = RD_T(0);
		if (current_temp >= 100)	current_temp = 99;

		if (key2_set_mode != 1) {
			current_time [4] = current_temp/10;
			current_time [5] = current_temp%10;
		}
	}
	if (sample_period == 0) {
		// 采集当前时间
		//hour:
		tmp = Get1302Hour();
		current_time [0] = tmp/16;
		current_time [1] = tmp%16;
	
		//minute:
		tmp = Get1302Minute();
		current_time [2] = tmp/16;
		current_time [3] = tmp%16;

		sample_period = 50;
	}

}

