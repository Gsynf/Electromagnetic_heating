#include "DS18B20.h"
#include "delay.h"
#include <REG51.H>


sbit DQ = P3^5; //3.5 为温控，2.4 为高温
sbit DQ2 = P2^4; //3.5 为温控，2.4 为高温

volatile unsigned int last_alarm_temp;
volatile unsigned int last_current_temp;
extern volatile unsigned char first_read_temp;

static void SetDQ (char s) 
{
	if (s == 0) DQ = 1;
	else DQ2 = 1;
}
static void ClearDQ (char s) 
{
	if (s == 0) DQ = 0;
	else DQ2 = 0;
}

/*******************************************************************************  
*** 函 数 名: void Init1820()  
*** 功能描述:   
*** 全局变量: NO !  
*** 输       入: NO !  
*** 输       出: NO !    
*** 修 改 人：                                      
*** 函数说明:   
/******************************************************************************/
idata int num;   
void Init1820(char s)   
{   
	num = 500;

    SetDQ (s);    
	Delay10uS (1);  
	ClearDQ (s);   //拉低数据线，准备Reset OneWire Bus；
	Delay10uS (20);
        
    SetDQ (s);     //提升数据线；        
    Delay10uS (2);
//	while(DQ)    //等待Slave 器件Ack 信号；    
//  {   
//     _nop_();    
//  } 
    while(num--)    //等待Slave 器件Ack 信号；    
    {  
	 	if (!s && !DQ) break; 
		if (s && !DQ2) break;  
        _nop_();    
    } 
	ClearDQ (s);
  
	Delay10uS (10);
    SetDQ (s);       //提升数据线，准备数据传输；    
}  


void WR1820(unsigned char DATA, char s)  
{
	idata unsigned char i = 0;
	for (i=8; i>0; i--)
	{
    	if (s == 0) {
			DQ = 0;
    		DQ = DATA&0x01;
		}else {
			DQ2 = 0;
    		DQ2 = DATA&0x01;
		}
 //   	delay(5);
 		Delay10uS (2);

   		SetDQ (s);

  		DATA >>= 1;
	}
}


unsigned char RD1820(char s)
{
	idata unsigned char i;
	idata unsigned char DATA=0;

	for (i=8; i>0; i--)
	{
    	ClearDQ (s); 	// 给脉冲信号
//		_nop_(); _nop_(); _nop_();
    	DATA >>= 1;
		//DQ = 1; 	// 给脉冲信号
		SetDQ (s); 	// 给脉冲信号
//		_nop_(); _nop_(); _nop_();

    	if( (s==0 && DQ) || (s==1 && DQ2))
			DATA |= 0x80;
		//delay(4);
		Delay10uS (2);
		
	}

	return DATA;
}


/*******************************************************************************  
*** 函 数 名: unsigned int RD_T();  
*** 功能描述: 读取温度；  
*** 全局变量: NO !  
*** 输       入: NO !  
*** 输       出: 16bit data；  
*** 函数说明: 读之前必须首先写1；  
/******************************************************************************/   
unsigned int RD_T(char s)   
{     
    unsigned int idata tp;   
    unsigned char idata tmbuf[2];  
 
//	if (s == 0){
//		//DQ = P3^5; //3.5 为温控，2.4 为高温
//		DQ=P3^5;
//	}
//	else
//		DQ = P2^4;

	Init1820(s); 	  
	WR1820(0xcc, s);     
    WR1820(0x44, s);
	if(first_read_temp == 1)
	{
		Delay(1000);		 //500ms延时
		first_read_temp = 0;
	}
	Init1820(s);	 
	WR1820(0xcc, s);   
    WR1820(0xbe, s); 
    
	tmbuf[0]=RD1820(s);   
	tmbuf[1]=RD1820(s);

    tp = tmbuf[1]*256+tmbuf[0];  
    tp = tp/16; //0.0625  

	// 	如果最高位为1，表示温度值为负，返回0
	if (tmbuf[1] & 0x80 == 0x01)
		tp = 0;

	if(tp <= 99)						   //限幅滤波功能，温度采集值大于99时
	{									   //返回上一次采集的温度有效值
		if(s == 0)
			last_current_temp = tp;
		else if(s == 1)
			last_alarm_temp = tp;
	}
	else 
	{
		if(s == 0)
			tp = last_current_temp;
		else if(s == 1)
			tp =  last_alarm_temp;	
	}

    return tp;      
}  

  
