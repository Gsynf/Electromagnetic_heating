#ifndef _key_H
#define _key_H

//#include <reg51.H>
#include <STC12C52.H>

///extern volatile unsigned char temp_set [2]; 
extern volatile unsigned char key1_mode, key2_set_mode,	key3_shift_mode, work_mode; 
extern volatile unsigned char current_time [6];   //当前时间值
extern volatile unsigned char set_start_time [6], set_stop_time [6];  //设定结束时间
extern volatile unsigned char idata set_start_time2 [6], set_stop_time2 [6];  
extern volatile unsigned char idata set_start_time3 [6], set_stop_time3 [6]; 
extern volatile unsigned char idata time_init[7];
extern unsigned char idata gSetTmpH, gSetTmpL;

void key_scan ();

//sbit KEY1 = P3^2;   //key1
//sbit KEY2 = P3^7;   //key2
//sbit KEY3 = P3^3;   //key3
//sbit KEY4 = P3^6;   //key4


#endif

