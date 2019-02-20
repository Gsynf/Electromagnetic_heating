#ifndef _TM1629_H_
#define _TM1629_H_

#include <intrins.h>
#include <STC12C52.H>

sbit P1_6 = P1^6;
sbit P1_7 = P1^7;
sbit P2_0 = P2^0;

#define DIO	 P1_6
#define STB	 P1_7	
#define CLK  P2_0	

//sbit Time_Point = P3^7; // 时间中间的两点


//extern volatile unsigned char AD_HIGH, AD_LOW; 
extern volatile unsigned char key2_set_mode, key3_shift_mode, work_mode, is_heat_now, is_work;
extern volatile unsigned char current_time [6];   //当前时间值（设置）
extern volatile unsigned char set_start_time [6], set_stop_time [6]; 
extern volatile unsigned char idata set_start_time2 [6], set_stop_time2 [6];
extern volatile unsigned char idata set_start_time3 [6], set_stop_time3 [6];
extern unsigned char idata work_mode_dis[6];

extern unsigned char* gp;

//初始化
void InitDisplay (void);

/***********************************
digital_num:   第几个数码管
digital_value: 要设定的数码管显示的值
************************************/
void WriteWord (unsigned char digital_num, unsigned char digital_value);
void WriteWord2(unsigned char addr, unsigned char num);
void getGpSetMode ();

/**********************
发送读键盘命令，读取键值
***********************/
void TM1629_ReadKey (void);

void DefaultState (unsigned char mode);
void DisplaySetState ();
void lighting_flash ();


extern unsigned char g_key_value;


#endif
