/***************************************************************
  4������
  ���� 1�� ���ؼ�
  		  0:  �ػ�״̬
		  1�� �رռ����� (��0״̬һ�£�
  ���� 2��ģʽ�л��� 0 ~ 9��   ��Ϊ�༭ģʽ������λ��˸
          0�� ��ʼ״̬����������״̬
		  1�� �趨��ǰʱ�䣬ͬʱ�趨Ĭ���¶ȣ��Ƕ�ʱ�Ͷ�ʱ�ǹ���������趨�¶ȣ�
		  2�� �趨��ʱ��ʼʱ��1��ͬʱ���õ�һ��ʱ����趨�¶�
		  3�� �趨��ʱ����ʱ��1
		  4:  �趨��ʱ��ʼʱ��2��ͬʱ���õڶ���ʱ����趨�¶ȡ�
		  5:���趨��ʱ����ʱ��2
	  	  6:  �趨��ʱ��ʼʱ��3��ͬʱ���õ�����ʱ����趨�¶ȡ�
		  7:���趨��ʱ����ʱ��3
		  8�� �趨����ģʽ ���������� �� ��ʱģʽ���أ�
		 		//һ��
		  		0: ����1�����Ƕ�ʱģʽ��1 ����
				1������1������ʱģʽ��1 ������34���� 	(�Ƕ�ʱһ�����ȹ���ģʽ)
				//����
				2: ����1��2�����Ƕ�ʱģʽ��2 ����
				3������1��2������ʱģʽ��2 ������34����  (�Ƕ�ʱ�������ȹ���ģʽ)
				//����
				4: ����1~4�����Ƕ�ʱģʽ��1,2 ����
				5������1~4������ʱģʽ��1,2 ���� ��3,4����  (�Ƕ�ʱ�������ȹ���ģʽ)

				3 ��������ʾ��ʱģʽ������ǰʱ���ڶ�ʱ������
				4 ��������ʾ��ʱģʽ�أ���ǰʱ�䲻�ڶ�ʱ������
  ���� 3����λ��
  ���� 4���༭ģʽ��ÿ�ΰ��£�ֵ����1��

==============================================================*/
#include "key.h"
#include "systemInit.h"
#include "TM1629.h"
#include "delay.h"
#include "eeprom.h"
#include "DS1302.h"


extern unsigned char* gp;
static unsigned char limit [6] = {3, 10, 6, 10, 10, 10};

static void key1_scan ();//����
static void key2_scan ();//����
static void key3_scan ();//��λ����
static void key4_scan ();//���Ӱ���

sbit RemoteControl= P3^2;
char openctrl = 0, remoopenctrl=0; //���ƿ��ؼ�
void key_scan ()
{
	TM1629_ReadKey ();
	
	switch (g_key_value)
	{
		case 0x08:
			if (openctrl!=0) break;
			key1_scan ();
			openctrl = 1;
			break;

		case 0x04:
			key2_scan ();
			openctrl = 0;
			break;

		case 0x02:
			key3_scan ();
			openctrl = 0;		
			break;

		case 0x01:
			key4_scan ();
			openctrl = 0;
			break;
		default:
			openctrl = 0;
			break;
	}
	
	// ң����
	if (RemoteControl)
		remoopenctrl = 0;
	if (!RemoteControl && remoopenctrl==0) {
		remoopenctrl = 1;
		key1_scan ();	
	}
	
}

/*************************
	��Ӧ���ذ��� 
*************************/
static void key1_scan () {  //����

	key1_mode ++;
	key1_mode %= 3;	
//	Delay (180);
	Delay (350);
}

/***********************
	��Ӧ ����ģʽѡ�� ����
***********************/
static void key2_scan () {   //����  
	if (key2_set_mode == 8)	{	
		write_eeprom_time_temp () ;
	}

	if (key2_set_mode == 0) {
		current_time [4] = gSetTmpH; //��ʾĬ�ϵ��¶�
		current_time [5] = gSetTmpL;
	}
	if (key2_set_mode == 1) {
		gSetTmpH = current_time [4];
		gSetTmpL = current_time [5];
	}
	if (key2_set_mode == 2)  {
		set_stop_time[4] = set_start_time[4];
		set_stop_time[5] = set_start_time[5];
	}
	if (key2_set_mode == 4)  {
		set_stop_time2[4] = set_start_time2[4];
		set_stop_time2[5] = set_start_time2[5];
	}
	if (key2_set_mode == 6)  {
		set_stop_time3[4] = set_start_time3[4];
		set_stop_time3[5] = set_start_time3[5];
	}

	key2_set_mode ++;
	key2_set_mode %= 9;  
 	key3_shift_mode = 0;

//	if (key2_set_mode == 1)
//		Delay (140);
	if (key2_set_mode == 8 || key2_set_mode == 0)
		Delay (200);
}

/********************************
	** ���ܣ� ��λ���� ѭ��ɨ����ģʽ
********************************/
static void key3_scan () {

	key3_shift_mode ++;

	if (key2_set_mode!=1 && key2_set_mode%2)	key3_shift_mode %= 4;
	else	key3_shift_mode %= 6;
	Delay (50);
}

/********************************
	** ���ܣ� ���Ӱ���
********************************/
static void key4_scan () {
	
    if (key2_set_mode == 0)	return;
    if (key2_set_mode == 8) {
		work_mode ++;
		work_mode %= 6;
		return;
	}

   	getGpSetMode();
	gp += key3_shift_mode;
	*gp = *gp+1;
	if (*gp >= limit [key3_shift_mode] )
		*gp = 0;

	if (key3_shift_mode == 1) { //Сʱ��һλΪ2ʱ����λ���Ϊ3
		gp --;
		if (*gp == 2) {
			gp ++;
			*gp %= 4;
		}
		else gp ++;
	}		

	//���µ�ǰʱ��
	if (key2_set_mode == 1) {
		// �������õ�ǰʱ��
		time_init [0] = 0;
		time_init [1] = current_time[2]*16+current_time[3]; // ����
		time_init [2] = current_time[0]*16+current_time[1]; // Сʱ
		Set1302M(time_init);
	}

//	Delay (20);
}

