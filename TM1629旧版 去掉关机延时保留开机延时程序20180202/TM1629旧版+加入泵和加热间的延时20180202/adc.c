#include "adc.h"
#include "Delay.h"
#include <intrins.h>
#include <math.h>

#include <STC12C52.H>

#define K1 56//5.62
#define K2 57//5.72
#define K3 58//5.85
#define K4 58//5.80

//每段都加上15度，对应576
#define B1 10520//9944//9944.46
#define B2 10513//9937.22
#define B3 10466//9890.91
#define B4 10488//9912.17

//每段都加上10度，对应384
//#define B1 10328//9944//9944.46
//#define B2 10321//9937.22
//#define B3 10274//9890.91
//#define B4 10296//9912.17

//每段都加上5度，对应192
//#define B1 10136//9944//9944.46
//#define B2 10129//9937.22
//#define B3 10082//9890.91
//#define B4 10104//9912.17

//未修正的
//#define B1 9944//9944.46
//#define B2 9937//9937.22
//#define B3 9890//9890.91
//#define B4 9912//9912.17

//38.5对应1度，第一段减5度，对应减去192
//每段都减去5度，对应减去192
//#define B1 9752//9944//9944.46
//#define B2 9745//9937.22
//#define B3 9698//9890.91
//#define B4 9720//9912.17

//每段都减去10度，对应减去384
//#define B1 9560//9944//9944.46
//#define B2 9553//9937.22
//#define B3 9506//9890.91
//#define B4 9528//9912.17

//每段都减去15度，对应减去576
//#define B1 9368//9944//9944.46
//#define B2 9361//9937.22
//#define B3 9314//9890.91
//#define B4 9336//9912.17

//每段都减去20度，对应减去768
//#define B1 9176//9944//9944.46
//#define B2 9169//9937.22
//#define B3 9122//9890.91
//#define B4 9144//9912.17

/********************
* *名称：get_AD_result
* *功能：得到AD转换数据
*********************/
sfr P1ASF     = 0x9D;
sfr ADC_RES   = 0xBD;

unsigned int get_AD_result(unsigned char channel)
{  
    idata int num = 0;
	P1ASF     = 0x03;
	ADC_RES   = 0;

    channel &= 0x07;                //0000,0111 清0高5位

    ADC_CONTR |= channel;           //选择 A/D 当前通道
	_nop_();_nop_();_nop_();_nop_();_nop_();

  				                      //使输入电压达到稳定
    ADC_CONTR |= 0x08;              //0000,1000 令 ADCS = 1, 启动A/D转换,
	_nop_();_nop_();_nop_();_nop_();_nop_();


    while (num++ < 10000)                       //等待A/D转换结束
    {
        if (ADC_CONTR & 0x10)       //0001,0000 测试A/D转换结束否
        	break; 
    }
  
  	ADC_CONTR &= 0xE0;               //1110,0000 清 ADC_FLAG, ADC_START 位和低 3 位
	_nop_();_nop_();_nop_();_nop_();_nop_(); 
//  ADC_CONTR &= 0xE7;              //1111,0111 清 ADC_FLAG 位, 关闭A/D转换, 

    return (ADC_RES); //返回 A/D 8 位转换结果
}


unsigned int AD_Filter2(unsigned char channel_number) {
	unsigned int idata average = 0;
	unsigned char idata i;

	for (i=0; i<6; i++)
		average += get_AD_result(channel_number) ;

	return average/30; //47; // /6:average; /10:convert to temperature
}



