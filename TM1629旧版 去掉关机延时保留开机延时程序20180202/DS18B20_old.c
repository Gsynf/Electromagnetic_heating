#include "DS18B20.h"
#include "delay.h"
#include <REG51.H>


sbit DQ = P3^5; //3.5 Ϊ�¿أ�2.4 Ϊ����

/*******************************************************************************  
*** �� �� ��: void Init1820()  
*** ��������:   
*** ȫ�ֱ���: NO !  
*** ��       ��: NO !  
*** ��       ��: NO !    
*** �� �� �ˣ�                                      
*** ����˵��:   
/******************************************************************************/   
void Init1820()   
{   
	idata int num = 500;
     DQ = 1;    
//     _nop_(); 
	Delay10uS (1);  
     DQ = 0;      //���������ߣ�׼��Reset OneWire Bus��    
//    DLY_us(125);  //��ʱ500us��Reset One-Wire Bus.    
//     DLY_us(125);   
	 Delay10uS (20);
        
     DQ = 1;      //���������ߣ�    
//     DLY_us(15);   //��ʱ35us��    
     Delay10uS (2);
   
//	 while(DQ)    //�ȴ�Slave ����Ack �źţ�    
//     {   
//        _nop_();    
//     } 

     while(num--)    //�ȴ�Slave ����Ack �źţ�    
     {  
	 	if (!DQ) break; 
        _nop_();    
     } 
	 
 // 	Delay10uS (6);
	DQ = 0;

//     DLY_us(60);   //��ʱ125us��     
	Delay10uS (10);
     DQ = 1;      //���������ߣ�׼�����ݴ��䣻    
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
    	DQ = 0; 	// �������ź�
//		_nop_(); _nop_(); _nop_();
    	DATA >>= 1;
		DQ = 1; 	// �������ź�
//		_nop_(); _nop_(); _nop_();

    	if(DQ)
			DATA |= 0x80;
		//delay(4);
		Delay10uS (1);
		
	}

	return DATA;
}


/*******************************************************************************  
*** �� �� ��: unsigned int RD_T();  
*** ��������: ��ȡ�¶ȣ�  
*** ȫ�ֱ���: NO !  
*** ��       ��: NO !  
*** ��       ��: 16bit data��  
*** ����˵��: ��֮ǰ��������д1��  
/******************************************************************************/   
unsigned int RD_T(char s)   
{     
    unsigned int idata tp;   
    unsigned char idata tmbuf[2];  
 
	if (s == 0)
		DQ = P3^5; //3.5 Ϊ�¿أ�2.4 Ϊ����
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

	// 	������λΪ1����ʾ�¶�ֵΪ��������0
	if (tmbuf[1] & 0x80 == 0x01)
		tp = 0;

    return tp;      
}   
