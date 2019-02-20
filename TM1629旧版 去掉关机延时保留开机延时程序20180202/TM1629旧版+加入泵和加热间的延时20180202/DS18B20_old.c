#include "DS18B20.h"
#include "delay.h"
#include <REG51.H>


sbit DQ = P3^5; //3.5 为温控，2.4 为高温

/*******************************************************************************  
*** 函 数 名: void Init1820()  
*** 功能描述:   
*** 全局变量: NO !  
*** 输       入: NO !  
*** 输       出: NO !    
*** 修 改 人：                                      
*** 函数说明:   
/******************************************************************************/   
void Init1820()   
{   
	idata int num = 500;
     DQ = 1;    
//     _nop_(); 
	Delay10uS (1);  
     DQ = 0;      //拉低数据线，准备Reset OneWire Bus；    
//    DLY_us(125);  //延时500us，Reset One-Wire Bus.    
//     DLY_us(125);   
	 Delay10uS (20);
        
     DQ = 1;      //提升数据线；    
//     DLY_us(15);   //延时35us；    
     Delay10uS (2);
   
//	 while(DQ)    //等待Slave 器件Ack 信号；    
//     {   
//        _nop_();    
//     } 

     while(num--)    //等待Slave 器件Ack 信号；    
     {  
	 	if (!DQ) break; 
        _nop_();    
     } 
	 
 // 	Delay10uS (6);
	DQ = 0;

//     DLY_us(60);   //延时125us；     
	Delay10uS (10);
     DQ = 1;      //提升数据线，准备数据传输；    
}  


void WR1820(unsigned char DATA)  
{
	unsigned char i = 0;
	for (i=8; i>0; i--)
	{
    	DQ = 0;
    	DQ = DATA&0x01;
 //   	delay(5);
 		Delay10uS (1);

   		DQ = 1;
  		DATA >>= 1;
	}
}


unsigned char RD1820()
{
	unsigned char i;
	unsigned char DATA=0;

	for (i=8; i>0; i--)
	{
    	DQ = 0; 	// 给脉冲信号
//		_nop_(); _nop_(); _nop_();
    	DATA >>= 1;
		DQ = 1; 	// 给脉冲信号
//		_nop_(); _nop_(); _nop_();

    	if(DQ)
			DATA |= 0x80;
		//delay(4);
		Delay10uS (1);
		
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
 
	if (s == 0)
		DQ = P3^5; //3.5 为温控，2.4 为高温
	else
		DQ = P2^4;

    Init1820();   
    WR1820(0xcc);  
	   
    WR1820(0x44);     
 
    Init1820();   
    WR1820(0xcc);   
    WR1820(0xbe);   
    tmbuf[0]=RD1820();   
    tmbuf[1]=RD1820();    

    tp = tmbuf[1]*256+tmbuf[0];  
    tp = tp/16; //0.0625  

	// 	如果最高位为1，表示温度值为负，返回0
	if (tmbuf[1] & 0x80 == 0x01)
		tp = 0;

    return tp;      
}   
