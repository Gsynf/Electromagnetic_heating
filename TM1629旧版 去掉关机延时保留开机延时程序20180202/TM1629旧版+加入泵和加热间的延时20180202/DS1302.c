#include <reg51.h>
#include <absacc.h>
#include <intrins.h>

#include "DS1302.h"

sbit T_CLK=P1^3;
sbit T_IO =P1^4;
sbit T_RST=P1^5;



//********DS1302��д����***************
/********************************************************************
�� �� ����RTInputByteM()
��    �ܣ�ʵʱʱ��д��һ�ֽ�
˵    ������DS1302д��1Byte���� (�ڲ�����)
��ڲ�����d д�������
�� �� ֵ����  
***********************************************************************/
void RTInputByteM(unsigned char d)
{
    unsigned char idata i;
    for(i=8; i>0; i--)
    {
        T_IO = d&0x01; _nop_();_nop_();_nop_();
		T_CLK = 1; _nop_();_nop_();_nop_();
		T_CLK = 0; _nop_();_nop_();_nop_();

	    d = d >> 1;
	}
}
/********************************************************************
�� �� ����RTOutputByteM()
��    �ܣ�ʵʱʱ�Ӷ�ȡһ�ֽ�
˵    ������DS1302��ȡ1Byte���� (�ڲ�����)
��ڲ�������  
�� �� ֵ��ACC
��    �ƣ�zhaojunjie           ��    �ڣ�2002-03-19
��    �ģ�                     ��    �ڣ�
***********************************************************************/
unsigned char RTOutputByteM(void)
{
    unsigned char idata i, DATA=0;
    for(i=8; i>0; i--)
    {
		_nop_();_nop_();_nop_();
        DATA >>= 1; 

		if (T_IO)
			DATA |= 0x80;
		
		_nop_();_nop_();_nop_();
     	T_CLK = 1; 
		
		_nop_();_nop_();_nop_();
        T_CLK = 0; 
	}
    return(DATA);
}
/********************************************************************
�� �� ����Write1302()��    �ܣ���DS1302д������
˵    ������д��ַ����д����/���� (�ڲ�����)
��    �ã�RTInputByteM() , RTOutputByteM()
��ڲ�����ucAddr: DS1302��ַ, ucData: Ҫд������
�� �� ֵ����
***********************************************************************/
void Write1302(unsigned char idata ucAddr, unsigned char idata ucDa)
{
    T_RST = 0; _nop_();_nop_();_nop_();
    T_CLK = 0;_nop_();_nop_();_nop_();
	T_RST = 1;_nop_();_nop_();_nop_();

 	RTInputByteM(ucAddr);   // ��ַ������

	_nop_();_nop_();_nop_();
 	RTInputByteM(ucDa);         // д1Byte����

    T_CLK = 1;_nop_();_nop_();_nop_();
    T_RST = 0;_nop_();_nop_();_nop_();
}							   
/********************************************************************
�� �� ����Read1302()
��    �ܣ���ȡDS1302ĳ��ַ������
˵    ������д��ַ���������/���� (�ڲ�����)
��    �ã�RTInputByteM() , RTOutputByteM()
��ڲ�����ucAddr: DS1302��ַ
�� �� ֵ��ucData :��ȡ������
***********************************************************************/
unsigned char Read1302(unsigned char ucAddr)
{
    unsigned char idata ucData;
    T_RST = 0;_nop_();_nop_();_nop_();
    T_CLK = 0;_nop_();_nop_();_nop_();
    T_RST = 1;_nop_();_nop_();_nop_();
    
	RTInputByteM(ucAddr);             // ��ַ������
	_nop_();_nop_();_nop_();
 	ucData = RTOutputByteM();         // ��1Byte����

    T_CLK = 1;_nop_();_nop_();_nop_();
    T_RST = 0;_nop_();_nop_();_nop_();
    
	return(ucData);
}
/********************************************************************
�� �� ����Set1302M()
��    �ܣ����ó�ʼʱ��
˵    ������д��ַ���������/����(�Ĵ������ֽڷ�ʽ)
��    �ã�Write1302()
��ڲ�����pClock: ����ʱ�����ݵ�ַ ��ʽΪ: �� �� ʱ �� �� ������
                               7Byte (BCD��)1B 1B 1B 1B 1B  1B  1B
�� �� ֵ����
***********************************************************************/
void Set1302M(unsigned char *pClock)
{
    unsigned char idata i;
    unsigned char idata ucAddr = 0x80;
    Write1302(0x8e,0x00);           // ��������,WP=0,д����
    for(i =7; i>0; i--)
    {
     	Write1302(ucAddr,*pClock);  // �� �� ʱ �� �� ���� ��
        pClock++;
        ucAddr +=2;
    }
    
	Write1302(0x8e,0x80);           // ��������,WP=1,д����
}



unsigned char Get1302Minute()
{
	unsigned char idata m;
	m = Read1302(0x083); //Delay11(50000);
	return m;
}
 
unsigned char Get1302Hour()
{
	unsigned char idata h;
	h = Read1302(0x085); //Delay11(50000);
	return h;
}


