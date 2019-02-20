#include "TM1629.h"
#include "intrins.h"
#include "delay.h"
#include "watchdog.h"

#include <STC12C52.H>


/***********��С����0-9����***********/
static unsigned char undecode[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
/***********��С����0-9����***********/
static unsigned char withdp[10] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xDF};
/**********�����̵�ֵ**********/
unsigned char g_key_value;

unsigned char* gp;

static void TM1629_SendByte(unsigned char value);
static void delay_nops ();

/************************************/
//�������ƣ�Init_TM1629����
//�������ܣ�TM1629��ʼ��
/************************************/
void InitDisplay (void)
{
	unsigned char idata i;
	
	STB = 1;
	CLK = 1;
	DIO = 0;

	delay_nops ();

	//������ʾ��������
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0x80); //��ʾ������

	STB = 1;
	delay_nops ();

	//������������
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0x40); //�̶���ַд����

	STB = 1;
	delay_nops ();

	//������ʾ��ַ������
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0xC0);

	delay_nops ();

	for(i=0;i<16;i++)
	{
		TM1629_SendByte(0x00);

		delay_nops ();
	}

	STB = 1;
	delay_nops ();

	//������ʾ��������
	STB = 0;
	delay_nops ();

	TM1629_SendByte(0x8a); //��ʾ������

	STB = 1;
	delay_nops ();

}


/************************************/
//�������ƣ�Set_TM1629()
//�������ܣ���ʾ����(digital_numΪλ�룬
//  digital_valueΪ����ʾ������)
/************************************/
void WriteWord (unsigned char digital_num, unsigned char digital_value)
{
	//������������
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0x44); //�̶���ַд����

	STB = 1;
	_nop_(); _nop_();

	//������ʾ��ַ������
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0xC1+(digital_num-1)*2);

//	TM1629_SendByte((undecode[digital_value]<<4)+(undecode[digital_value]>>4));
	TM1629_SendByte(digital_value); //undecode[digital_value]

	STB = 1;
	_nop_(); _nop_();

	//������ʾ��������
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0x8F); //��ʾ������

	STB = 1;
	_nop_(); _nop_();

}


/************************************/
//�������ƣ�TM1629_SendByte()
//�������ܣ���TM1629д��һ���ֽ�
/************************************/
static void TM1629_SendByte(unsigned char value)
{
	unsigned char idata i;

	for(i=0;i<8;i++)
	{
		CLK = 0;
		DIO = (value>>i)&0x01;
		delay_nops ();
		CLK = 1;
		delay_nops ();
	}
}


/************************************/
//�������ƣ�TM1629_ReadByte()
//�������ܣ���TM1629����һ���ֽ�
/************************************/
void TM1629_ReadByte(void)
{
	unsigned char idata i,key_data=0x00;
	DIO = 1;
	for (i=0;i<8;i++)
	{
		CLK = 0;
		key_data >>= 1;
		if (DIO)
		{
			key_data += 0x80;
		}
		delay_nops ();
		CLK = 1;
		delay_nops ();
	}
	g_key_value = key_data;
}


/************************************/
//�������ƣ�TM1629_ReadKey()
//�������ܣ���TM1629�����ֵ
/************************************/
void TM1629_ReadKey (void)
{
	STB = 0;
	_nop_(); _nop_();

	TM1629_SendByte(0x42); //����ɨ����

	delay_nops ();

	TM1629_ReadByte();
	_nop_(); _nop_();_nop_(); _nop_();
	
	STB = 1;
}


/************************************/
//�������ƣ�Delay()
//�������ܣ���ʱ12*һ����������=12*ʮ����ʱ������
/************************************/
static void delay_nops (void)
{
	_nop_(); _nop_();_nop_(); _nop_();
//	_nop_(); _nop_();_nop_(); _nop_();
//	_nop_(); _nop_();_nop_(); _nop_();	
}


void WriteWord2(unsigned char addr, unsigned char num)
{
	WriteWord (addr, undecode [num] );
}





static unsigned char  i;
/******************************
* *���ܣ�
	(1) ϵͳ����������״̬��
    (2) 0 ����״̬ ��Ĭ�ϣ�
******************************/
static void disbase () {
  	
	for (i=2; i<=7; i++) { 
		if (is_heat_now && i==6)
			WriteWord (6, withdp [ *gp ]);
		else
			WriteWord2 (i, *gp);
		gp ++;
	}
}

void DefaultState (unsigned char mode)
{
	WriteWord (1, 0);
	gp = current_time;

	disbase ();
	Delay (7);
	if (mode == 0 && is_work == 0) WriteWord (8, 0);
}

void getGpSetMode () {
	gp = current_time;
	switch (key2_set_mode) {
	case 2:
		gp = set_start_time;
		break;
	case 3:
		gp = set_stop_time;
		break;
	case 4:
		gp = set_start_time2;
		break;
	case 5:
		gp = set_stop_time2;
		break;
	case 6:
		gp = set_start_time3;
		break;
	case 7:
		gp = set_stop_time3;
		break;
	}
}


/************************
* *���ƣ�mode_lighting
* *���ܣ�����༭ģʽʱ����˸
*************************/
void lighting_flash ()
{
	//ʱ������Ϊ2~5λ, �¶�����6~7λ
	idata unsigned char addr = 2 + key3_shift_mode;  
	idata unsigned char nn = 0;

	if (key2_set_mode!=0){
		WriteWord(8, work_mode_dis[work_mode]);
	}
  	if (key2_set_mode == 8) {
		Delay (100);
		return;
	}

	getGpSetMode ();
	gp += key3_shift_mode;
		
	WriteWord2 (addr, *gp);
	
	Delay(100);
	WriteWord (addr, 0); 
	Delay(100);		
}

/********************************
 * *���ܣ�

********************************/
void DisplaySetState () {

	WriteWord2 (1, key2_set_mode);
	gp = current_time;

	getGpSetMode ();
 	disbase ();
}	


