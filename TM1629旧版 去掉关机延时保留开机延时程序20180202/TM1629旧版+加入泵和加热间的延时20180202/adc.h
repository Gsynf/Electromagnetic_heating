#ifndef _adc_H
#define _adc_H

//#include <reg51.H>
#include <STC12C52.H>


/********************
* *名称：AD_Filter
* *功能：AD数字滤波器
*********************/


//其他保护AD通道采样
unsigned int AD_Filter2(unsigned char channel_number);

#endif