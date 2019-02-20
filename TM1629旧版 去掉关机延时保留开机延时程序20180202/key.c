/***************************************************************
  4个按键
  按键 1： 开关键
  		  0:  关机状态
		  1： 关闭加热器 (和0状态一致）
  按键 2：模式切换键 0 ~ 9；   如为编辑模式，则首位闪烁
          0： 初始状态和正常工作状态
		  1： 设定当前时间，同时设定默认温度（非定时和定时非工作区间的设定温度）
		  2： 设定定时起始时间1，同时设置第一段时间的设定温度
		  3： 设定定时结束时间1
		  4:  设定定时起始时间2，同时设置第二段时间的设定温度　
		  5:　设定定时结束时间2
	  	  6:  设定定时起始时间3，同时设置第三段时间的设定温度　
		  7:　设定定时结束时间3
		  8： 设定工作模式 （驱动开关 和 定时模式开关）
		 		//一档
		  		0: 驱动1开，非定时模式，1 灯亮
				1：驱动1开，定时模式，1 灯亮，34灯亮 	(非定时一档加热工作模式)
				//二档
				2: 驱动1，2开，非定时模式，2 灯亮
				3：驱动1，2开，定时模式，2 灯亮，34灯亮  (非定时二档加热工作模式)
				//三档
				4: 驱动1~4开，非定时模式，1,2 灯亮
				5：驱动1~4开，定时模式，1,2 灯亮 ，3,4灯亮  (非定时三档加热工作模式)

				3 灯亮：表示定时模式开，当前时间在定时区间内
				4 灯亮：表示定时模式关，当前时间不在定时区间内
  按键 3：移位键
  按键 4：编辑模式（每次按下，值增加1）

==============================================================*/
#include "key.h"
#include "systemInit.h"
#include "TM1629.h"
#include "delay.h"
#include "eeprom.h"
#include "DS1302.h"


extern unsigned char* gp;
static unsigned char limit [6] = {3, 10, 6, 10, 10, 10};

static void key1_scan ();//开关
static void key2_scan ();//设置
static void key3_scan ();//移位按键
static void key4_scan ();//增加按键

sbit RemoteControl= P3^2;
char openctrl = 0, remoopenctrl=0; //控制开关键
void key_scan ()
{
	TM1629_ReadKey ();
	
	switch (g_key_value)
	{
		case 0x08:
			if (openctrl!=0) break;
			key1_scan ();
			openctrl = 1;
			break;

		case 0x04:
			key2_scan ();
			openctrl = 0;
			break;

		case 0x02:
			key3_scan ();
			openctrl = 0;		
			break;

		case 0x01:
			key4_scan ();
			openctrl = 0;
			break;
		default:
			openctrl = 0;
			break;
	}
	
	// 遥控器
	if (RemoteControl)
		remoopenctrl = 0;
	if (!RemoteControl && remoopenctrl==0) {
		remoopenctrl = 1;
		key1_scan ();	
	}
	
}

/*************************
	对应开关按键 
*************************/
static void key1_scan () {  //开关

	key1_mode ++;
	key1_mode %= 3;	
//	Delay (180);
	Delay (350);
}

/***********************
	对应 设置模式选择 按键
***********************/
static void key2_scan () {   //设置  
	if (key2_set_mode == 8)	{	
		write_eeprom_time_temp () ;
	}

	if (key2_set_mode == 0) {
		current_time [4] = gSetTmpH; //显示默认的温度
		current_time [5] = gSetTmpL;
	}
	if (key2_set_mode == 1) {
		gSetTmpH = current_time [4];
		gSetTmpL = current_time [5];
	}
	if (key2_set_mode == 2)  {
		set_stop_time[4] = set_start_time[4];
		set_stop_time[5] = set_start_time[5];
	}
	if (key2_set_mode == 4)  {
		set_stop_time2[4] = set_start_time2[4];
		set_stop_time2[5] = set_start_time2[5];
	}
	if (key2_set_mode == 6)  {
		set_stop_time3[4] = set_start_time3[4];
		set_stop_time3[5] = set_start_time3[5];
	}

	key2_set_mode ++;
	key2_set_mode %= 9;  
 	key3_shift_mode = 0;

//	if (key2_set_mode == 1)
//		Delay (140);
	if (key2_set_mode == 8 || key2_set_mode == 0)
		Delay (200);
}

/********************************
	** 功能： 移位按键 循环扫描检测模式
********************************/
static void key3_scan () {

	key3_shift_mode ++;

	if (key2_set_mode!=1 && key2_set_mode%2)	key3_shift_mode %= 4;
	else	key3_shift_mode %= 6;
	Delay (50);
}

/********************************
	** 功能： 增加按键
********************************/
static void key4_scan () {
	
    if (key2_set_mode == 0)	return;
    if (key2_set_mode == 8) {
		work_mode ++;
		work_mode %= 6;
		return;
	}

   	getGpSetMode();
	gp += key3_shift_mode;
	*gp = *gp+1;
	if (*gp >= limit [key3_shift_mode] )
		*gp = 0;

	if (key3_shift_mode == 1) { //小时第一位为2时，低位最高为3
		gp --;
		if (*gp == 2) {
			gp ++;
			*gp %= 4;
		}
		else gp ++;
	}		

	//更新当前时间
	if (key2_set_mode == 1) {
		// 重新设置当前时间
		time_init [0] = 0;
		time_init [1] = current_time[2]*16+current_time[3]; // 分钟
		time_init [2] = current_time[0]*16+current_time[1]; // 小时
		Set1302M(time_init);
	}

//	Delay (20);
}

