#include "adc.h"
#include "Delay.h"
#include <intrins.h>
#include <math.h>

#include <STC12C52.H>

#define K1 56//5.62
#define K2 57//5.72
#define K3 58//5.85
#define K4 58//5.80

//ÿ�ζ�����15�ȣ���Ӧ576
#define B1 10520//9944//9944.46
#define B2 10513//9937.22
#define B3 10466//9890.91
#define B4 10488//9912.17

//ÿ�ζ�����10�ȣ���Ӧ384
//#define B1 10328//9944//9944.46
//#define B2 10321//9937.22
//#define B3 10274//9890.91
//#define B4 10296//9912.17

//ÿ�ζ�����5�ȣ���Ӧ192
//#define B1 10136//9944//9944.46
//#define B2 10129//9937.22
//#define B3 10082//9890.91
//#define B4 10104//9912.17

//δ������
//#define B1 9944//9944.46
//#define B2 9937//9937.22
//#define B3 9890//9890.91
//#define B4 9912//9912.17

//38.5��Ӧ1�ȣ���һ�μ�5�ȣ���Ӧ��ȥ192
//ÿ�ζ���ȥ5�ȣ���Ӧ��ȥ192
//#define B1 9752//9944//9944.46
//#define B2 9745//9937.22
//#define B3 9698//9890.91
//#define B4 9720//9912.17

//ÿ�ζ���ȥ10�ȣ���Ӧ��ȥ384
//#define B1 9560//9944//9944.46
//#define B2 9553//9937.22
//#define B3 9506//9890.91
//#define B4 9528//9912.17

//ÿ�ζ���ȥ15�ȣ���Ӧ��ȥ576
//#define B1 9368//9944//9944.46
//#define B2 9361//9937.22
//#define B3 9314//9890.91
//#define B4 9336//9912.17

//ÿ�ζ���ȥ20�ȣ���Ӧ��ȥ768
//#define B1 9176//9944//9944.46
//#define B2 9169//9937.22
//#define B3 9122//9890.91
//#define B4 9144//9912.17

/********************
* *���ƣ�get_AD_result
* *���ܣ��õ�ADת������
*********************/
sfr P1ASF     = 0x9D;
sfr ADC_RES   = 0xBD;

unsigned int get_AD_result(unsigned char channel)
{  
    idata int num = 0;
	P1ASF     = 0x03;
	ADC_RES   = 0;

    channel &= 0x07;                //0000,0111 ��0��5λ

    ADC_CONTR |= channel;           //ѡ�� A/D ��ǰͨ��
	_nop_();_nop_();_nop_();_nop_();_nop_();

  				                      //ʹ�����ѹ�ﵽ�ȶ�
    ADC_CONTR |= 0x08;              //0000,1000 �� ADCS = 1, ����A/Dת��,
	_nop_();_nop_();_nop_();_nop_();_nop_();


    while (num++ < 10000)                       //�ȴ�A/Dת������
    {
        if (ADC_CONTR & 0x10)       //0001,0000 ����A/Dת��������
        	break; 
    }
  
  	ADC_CONTR &= 0xE0;               //1110,0000 �� ADC_FLAG, ADC_START λ�͵� 3 λ
	_nop_();_nop_();_nop_();_nop_();_nop_(); 
//  ADC_CONTR &= 0xE7;              //1111,0111 �� ADC_FLAG λ, �ر�A/Dת��, 

    return (ADC_RES); //���� A/D 8 λת�����
}


unsigned int AD_Filter2(unsigned char channel_number) {
	unsigned int idata average = 0;
	unsigned char idata i;

	for (i=0; i<6; i++)
		average += get_AD_result(channel_number) ;

	return average/30; //47; // /6:average; /10:convert to temperature
}



