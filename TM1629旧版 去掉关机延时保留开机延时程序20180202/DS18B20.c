#include "DS18B20.h"
#include "delay.h"
#include <REG51.H>


sbit DQ = P3^5; //3.5 Ϊ�¿أ�2.4 Ϊ����
sbit DQ2 = P2^4; //3.5 Ϊ�¿أ�2.4 Ϊ����

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
*** �� �� ��: void Init1820()  
*** ��������:   
*** ȫ�ֱ���: NO !  
*** ��       ��: NO !  
*** ��       ��: NO !    
*** �� �� �ˣ�                                      
*** ����˵��:   
/******************************************************************************/
idata int num;   
void Init1820(char s)   
{   
	num = 500;

    SetDQ (s);    
	Delay10uS (1);  
	ClearDQ (s);   //���������ߣ�׼��Reset OneWire Bus��
	Delay10uS (20);
        
    SetDQ (s);     //���������ߣ�        
    Delay10uS (2);
//	while(DQ)    //�ȴ�Slave ����Ack �źţ�    
//  {   
//     _nop_();    
//  } 
    while(num--)    //�ȴ�Slave ����Ack �źţ�    
    {  
	 	if (!s && !DQ) break; 
		if (s && !DQ2) break;  
        _nop_();    
    } 
	ClearDQ (s);
  
	Delay10uS (10);
    SetDQ (s);       //���������ߣ�׼�����ݴ��䣻    
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
    	ClearDQ (s); 	// �������ź�
//		_nop_(); _nop_(); _nop_();
    	DATA >>= 1;
		//DQ = 1; 	// �������ź�
		SetDQ (s); 	// �������ź�
//		_nop_(); _nop_(); _nop_();

    	if( (s==0 && DQ) || (s==1 && DQ2))
			DATA |= 0x80;
		//delay(4);
		Delay10uS (2);
		
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
 
//	if (s == 0){
//		//DQ = P3^5; //3.5 Ϊ�¿أ�2.4 Ϊ����
//		DQ=P3^5;
//	}
//	else
//		DQ = P2^4;

	Init1820(s); 	  
	WR1820(0xcc, s);     
    WR1820(0x44, s);
	if(first_read_temp == 1)
	{
		Delay(1000);		 //500ms��ʱ
		first_read_temp = 0;
	}
	Init1820(s);	 
	WR1820(0xcc, s);   
    WR1820(0xbe, s); 
    
	tmbuf[0]=RD1820(s);   
	tmbuf[1]=RD1820(s);

    tp = tmbuf[1]*256+tmbuf[0];  
    tp = tp/16; //0.0625  

	// 	������λΪ1����ʾ�¶�ֵΪ��������0
	if (tmbuf[1] & 0x80 == 0x01)
		tp = 0;

	if(tp <= 99)						   //�޷��˲����ܣ��¶Ȳɼ�ֵ����99ʱ
	{									   //������һ�βɼ����¶���Чֵ
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

  
