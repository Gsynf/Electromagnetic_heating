#include "Delay.h"
#include "intrins.h"
#include "key.h"
#include "TM1629.h"
#include "eeprom.h"
#include "systemInit.h"
#include "work_mode.h"
#include "watchdog.h"
#include "DS1302.h"


#include <STC12C52.H>

volatile unsigned char first_read_temp = 1;

/**** AD ת��ȫ�ֱ��� ************************/
// AD ���� ��Ӧ��ʾ2λ�¶�ֵ�� 
volatile unsigned int current_temp = 0; //��ǰ�¶�ֵ
unsigned char idata gSetTmpH = 5,
	gSetTmpL = 0;

/**** �������� ȫ�ֱ��� ************************/
volatile unsigned char key1_mode = 0,         //����1�����ؼ�
			key2_set_mode = 0,   //����2������ģʽ�� 0 ~ 5
			key3_shift_mode = 0; //����3����λģʽ
volatile unsigned char work_mode = 4;       //�趨ģʽ5����Ӧ�Ĺ���ģʽֵ;
//0:�Ƕ�ʱģʽI��;1:��ʱģʽI��;2:�Ƕ�ʱģʽII��;3:��ʱģʽII��;4:�Ƕ�ʱģʽIII��;5:��ʱģʽIII��;
volatile unsigned char is_heat_now = 0;		//�Ƿ��ڼ���״̬���������Ͻǵ�LED�Ƿ����
volatile unsigned char need_keep_temp = 0;		//�Ƿ���Ҫ���£���һ�����²���Ҫ���£��ڶ����¶��½���Ҫ���£�
volatile unsigned char is_work = 0;				//�Ƿ��ڹ���״̬�����ƶ�ʱģʽ�µ���ʾ���⣩
			
			
/********* ʱ�� *************************
	0: Сʱ��λ��
	1��Сʱ��λ��
	2�����Ӹ�λ��
	3�����ӵ�λ��
	4: ��ǰ�¶Ȼ��趨�¶ȵĸ�λ
	5����ǰ�¶Ȼ��趨�¶ȵĵ�λ 
    ===================================*/
volatile unsigned char current_time [6]= {0,0,0,0,5};   //��ǰʱ��ֵ�����ã�
volatile unsigned char set_start_time [6]= {0,0,0,0,5}; //�趨��ʼʱ��
volatile unsigned char set_stop_time [6];  //�趨����ʱ��
volatile unsigned char idata set_start_time2 [6] = {0,0,0,0,5}; //�趨��ʼʱ��2
volatile unsigned char idata set_stop_time2 [6] = {0,0,0,0};  //�趨����ʱ��2
volatile unsigned char idata set_start_time3 [6] = {0,0,0,0,5}; //�趨��ʼʱ��3
volatile unsigned char idata set_stop_time3 [6] = {0,0,0,0};  //�趨����ʱ��3
unsigned char idata work_mode_dis [6] = {0x08, 0x68, 0x10, 0x70, 0x18, 0x78};
				

/************���ֹͣ��־*****************/
/***********************************************
motor_num: 
0x00: ����������
0xFF: ������ֹͣ���������ǵ����Ȼ������һ���Ӻ�ֹͣ
***********************************************/
volatile unsigned char motor_num = 0;


//��������
volatile unsigned char idata sample_period = 50;

//ʱ�����ݵ�ַ ��ʽΪ: �� �� ʱ �� �� ���� �� ����   
unsigned char idata time_init[7]= {0x00,0x00,0x18,0x01,0x01,0x07,0x12};
volatile unsigned char have_set_time = 0;  //�Ƿ������˳�ʼ��ʱ��


static unsigned int idata num = 100,
	num2 = 1000;
void detect () {
	if (num -- == 0)	{
		enter_protect_mode ();     //�쳣�������
		num = 100;
	}
	if (num2 -- == 0)	{
		InitDisplay();             //��ʼ��tm1629
		num2 = 1000;
	}
}

/********************
* *���ƣ�main
* *���ܣ�
*********************/
void main()
{
	// sleep ...
	Delay (100); //	 100*0.1ms

	//��ʼ��
	system_init ();								//��ʼ���˿� 
	InitDisplay();                  			//��ʼ��TM1629
	clear_switch ();                			//�������״̬��Ϣ
	Motor_CTL = CLOSE_MOTOR_SPEAKER;      		//�رձ�
	
	//��eeprom�ж�ȡ�趨���¶���ʱ��ֵ
	read_eeprom_time_temp();
	 
	if (have_set_time != 1) {  //��һд����ʱҪ��ʱ��оƬ���г�ʼ�����Ժ��ϵ�㲻���ڳ�ʼ��
		Set1302M(time_init); 
		have_set_time = 1;
	}
	Delay (100);
	while(1)
	{
		
		Speaker_CTL = CLOSE_MOTOR_SPEAKER;
		watch_dog();
		ad_sample ();

		if (key1_mode == 0) { //�ػ�״̬���ȴ������� 0:��ʾ�ػ�״̬;1:��ʾ����״̬;2:��ʾ�ػ�����
			key_scan ();
			key2_set_mode = 0;
			DefaultState (0);
			
        	detect();
			
			//�����¶����ޣ�motor_num = 0xFF
			//���¼��ȵ�ʱ�������ã������¶�����ʱ���ȴ�һ����ֹͣ��
			motor_timer ();	

			continue;
		}

		if ( key1_mode == 2) { //�ػ�����
			//�ػ�ʱд��ʱ����¶ȵ��趨ֵ
			write_eeprom_time_temp () ; //�˾�û�б�Ҫ����ɾȥ
			switch_off ();
			Speaker_CTL = CLOSE_MOTOR_SPEAKER;
			motor_num = 0xFF;  //�ػ���־

			key1_mode = 0;   

			continue;
		}
		
		key_scan ();	//key1_mode == 1ʱ��ɨ�迪�ؼ�
        motor_timer ();	//�ж��Ƿ�ر�
		
		detect();
		
		switch ( key2_set_mode )
		{
		case 0://��ģʽ
			DisplaySetState ();
			enter_work_mode ();      //���빤��ģʽ(��һ��LED��ʾ0��ģʽ)
			break;
			
		case 1:		//���õ�ǰʱ���Ĭ���¶�
		case 2: 	//�趨��ʼ��ʱʱ��1		
		case 3: 	//�趨��ֹ��ʱʱ��1
		case 4:		//�趨��ʼ��ʱʱ��2
		case 5: 	//�趨��ֹ��ʱʱ��2
		case 6:		//�趨��ʼ��ʱʱ��3 	
		case 7: 	//�趨��ֹ��ʱʱ��3
		case 8:	    //�趨����ģʽ
		case 9:		//
			lighting_flash ();	//��˸
			DisplaySetState ();	
			break;
		}		

	}
}


