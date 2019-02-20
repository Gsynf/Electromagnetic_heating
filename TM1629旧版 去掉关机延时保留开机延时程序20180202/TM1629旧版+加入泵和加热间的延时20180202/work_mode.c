#include "work_mode.h"
#include "TM1629.h"
#include "delay.h"
#include "watchdog.h"
#include "DS18B20.h"
#include "DS1302.h"
#include "adc.h"


//�¶�����ת��ֵ
static unsigned int up_temp; //�¶�����

static unsigned char _i;


/********************
* *���ƣ�
* *���ܣ��ر��ĸ��������Ҳࣩ
*********************/
void switch_off(void)
{
	clear_switch ();
	Motor_CTL = CLOSE_MOTOR_SPEAKER;	
	need_keep_temp = 0;
}

static unsigned char is_open_or_close_switch = 0;	//�Ƿ��Ǵ򿪻��߹ر�����
static void switch_delay () {
	if (is_open_or_close_switch == 1) {
		Delay (200);
	}
}

/***************************
* * ���ܣ��������
* *
**************************/
void clear_switch () {
 	if (is_heat_now == 1) is_open_or_close_switch = 1;
	
	CTL1 = CLOSE_CTL;
	switch_delay ();
	CTL2 = CLOSE_CTL;
	switch_delay ();
	CTL3 = CLOSE_CTL;
	switch_delay ();
	CTL4 = CLOSE_CTL;

	//ֹͣ����:
	is_heat_now = 0;
	is_open_or_close_switch = 0;
}

static void heat_mode() {
	switch (work_mode / 2) {
	case 0:	//1������
		CTL1 = OPEN_CTL;
		switch_delay ();
		CTL2 = OPEN_CTL;
		switch_delay ();
		CTL3 = CLOSE_CTL;
		switch_delay ();
		CTL4 = CLOSE_CTL;	
		break;

	case 1: //2������
		CTL1 = OPEN_CTL;
		switch_delay ();
		CTL2 = OPEN_CTL;
		switch_delay ();
		CTL3 = OPEN_CTL;
		switch_delay ();
		CTL4 = CLOSE_CTL;
		break;

	case 2: //3������
		CTL1 = OPEN_CTL;
		switch_delay ();
		CTL2 = OPEN_CTL;
		switch_delay ();
		CTL3 = OPEN_CTL;
		switch_delay ();
		CTL4 = OPEN_CTL;
		break;
	}
}
//����
static void heat ()
{
	//
	if (current_temp > up_temp)  	{ //ֹͣ����
		clear_switch ();
		motor_num = 0xFF;
		need_keep_temp = 1;
	}
	else {
		
		if (current_temp >= (up_temp-7)) {
			if (need_keep_temp) { //�ж��Ƿ���Ҫ����
				clear_switch ();
				Motor_CTL = CLOSE_MOTOR_SPEAKER;
				return;
			}
		}
		need_keep_temp = 0;

		if (is_heat_now == 0) is_open_or_close_switch = 1;
		if(Motor_CTL == 0)
		{
			Motor_CTL = OPEN_MOTOR_SPEAKER; 	//�򿪱�
			Delay(5000);
			heat_mode();
		}
		//����	
		is_heat_now = 1;	//���ڼ���
		
		is_open_or_close_switch = 0;

    }
}


/*************************************
* *���ƣ�enter_timing_mode
* *���ܣ���ʱģʽ���ж����趨ʱ��֮�ڹ���
*************************************/
static void among_work_time () {
	is_work = 1;
	heat ();
}

static void not_among_work_time (unsigned char open)
{
	if (key1_mode == 0) WriteWord (8, 0);
	else WriteWord (8,open+0x20);
	clear_switch ();
	Motor_CTL = CLOSE_MOTOR_SPEAKER;	//���ڹ���ʱ����ʱ���رձ�
	is_work = 0;
}

static unsigned int get_itime (unsigned char* gp)
{
	return ( (*gp++)*10 + (*gp++) ) * 60 + (*gp++)*10 +  (*gp++);
}

static unsigned int idata s_timer, e_timer, 
	s_timer2, e_timer2, 
	s_timer3, e_timer3,
	time;
static unsigned char is_among_time = 0;
static void IsAmongTime (unsigned int st, unsigned int et, unsigned char nn)
{
   	if (et >= st) {
		if ((time >= st)&&(time < et))	
			is_among_time = nn;	
	}
	else {
		if (!((time > et)&&(time < st)))
			is_among_time = nn;
	}

}

static void enter_timing_mode (unsigned char open)
{
	is_among_time = 0;
	s_timer = get_itime (set_start_time);
	e_timer = get_itime (set_stop_time);
	s_timer2 = get_itime (set_start_time2);
	e_timer2 = get_itime (set_stop_time2);
	s_timer3 = get_itime (set_start_time3);
	e_timer3 = get_itime (set_stop_time3);

	time = get_itime (current_time);

	IsAmongTime (s_timer3, e_timer3, 1);
	IsAmongTime (s_timer2, e_timer2, 2);
	IsAmongTime (s_timer, e_timer, 3);

	up_temp = set_start_time[4]*10 + set_start_time[5];
	if (is_among_time == 1) {
		up_temp = set_start_time3[4]*10 + set_start_time3[5];
	} else if (is_among_time == 2) {
		up_temp = set_start_time2[4]*10 + set_start_time2[5];
	}
	if (is_among_time) {
		WriteWord (8, open);
		among_work_time ();
	} else {
		not_among_work_time (open);	
	}
}

/*******************************
* *���ƣ�work_mode
* *���ܣ�ģʽѡ��Ϊ0ʱ��Ĭ�Ϲ���״̬
********************************/
void enter_work_mode ()
{
	// Ĭ���¶�����ֵ
	up_temp = gSetTmpH * 10 + gSetTmpL;

	switch ( work_mode )
	{
	//��ʱģʽ
	case 1: //һ����ʱģʽ
		enter_timing_mode (0x28);	
		return;
	case 3: //������ʱģʽ
		enter_timing_mode (0x30);
		return;
	case 5: //������ʱģʽ
		enter_timing_mode (0x38);
		return;

	 //�Ƕ�ʱģʽ			
	case 0: //һ���Ƕ�ʱģʽ
	case 2: //�����Ƕ�ʱģʽ
	case 4: //�����Ƕ�ʱģʽ
		WriteWord(8, work_mode_dis[work_mode]);
		heat ();
		break;								
	}	

}

void protect_displsy ()
{
	ad_sample ();  //�ɼ���ǰ�¶�ֵ
	if (key1_mode == 0) {
		DefaultState (1);
		return;
	}

 	key2_set_mode = 0;	
	DisplaySetState ();
}

void protect_proc ()
{
	clear_switch ();
	Motor_CTL = CLOSE_MOTOR_SPEAKER;
	Speaker_CTL = OPEN_MOTOR_SPEAKER;
	watch_dog();

	protect_displsy ();
}

void enter_protect_mode () {
//	static unsigned char is_first_log = 0; //��һ���ϵ�

	while (AD_Filter2(1)>15)//10)//©�籣��;15��ӦԼXXV
	{
		WriteWord (8,0x01);
		protect_proc ();
	}  
	

//	while (AD_Filter2(1)>85)     //70 //���±���	��Ӧ��Լ85��
	while (RD_T(1) > 85)
	{
		WriteWord (8,0x80);
		protect_proc ();
	} 

   
//	if (AD_Filter2(2)<16)   	//14)//22)     //�������� 
	if (current_temp < 3)
	{
		clear_switch (); //����	

//		while (AD_Filter2(2)<20)//<18)  	//������������ֹ�¶ȹ��ͣ�������ů��
		while (current_temp < 8) //10)
	    {
			is_heat_now = 1;
            Motor_CTL = OPEN_MOTOR_SPEAKER;              	//�򿪱�

			heat_mode();

			//����ǵ�һ���ϵ磬�����е�λ���򿪣��Լӿ����
//			if (is_first_log == 0) {
//				CTL1=OPEN_CTL; 
//				Delay (100);
//				CTL2=OPEN_CTL; 
//				Delay (100);
//				CTL3=OPEN_CTL; 
//				Delay (100);
//				CTL4=OPEN_CTL;
//				is_first_log = 1;
//			}
//			//������ǵ�һ���ϵ磬��ֻ��1��λ
//			else CTL1=OPEN_CTL;

			Speaker_CTL = OPEN_MOTOR_SPEAKER;
			watch_dog();
			WriteWord (8,0x04);
			protect_displsy ();
		}
	
		Speaker_CTL = CLOSE_MOTOR_SPEAKER;			//�رշ�����
        Motor_CTL = CLOSE_MOTOR_SPEAKER;      		//�رձ�
		clear_switch ();							//�ر����м���			

		//���û�������5ģʽ�����˼��ȵ��κ󣬲Żᴥ������������ģʽ
		//�������º��ٴν����������״̬

		//�޸�  ----  ����Ƕ�ʱģʽ����ʼ������
		//      ----  ����ǷǶ�ʱģʽ������չ���ģʽ
//		if (work_mode % 2 == 0) {	//�Ƕ�ʱģʽ
//			work_mode = 0xFF;   
//		}
	}  
	
	while (AD_Filter2(0)>=8)//25)//©ˮ����
	{
		WriteWord (8,0x02);
		protect_proc ();
	} 
 
	//add:
//	if (work_mode == 0xFF)		//�Ƕ�ʱģʽ
//		WriteWord (8,0x00);	
	if (work_mode % 2 == 1)	{	//��ʱģʽ
		if (key1_mode == 0) { 	//�ػ�״̬
			enter_work_mode ();
		}
	}
	
}



void motor_timer (void)
{
	if (motor_num == 0xFF)//���ػ���־���ȴ���Լ1���Ӻ�ֹͣ��﹤��
	{
		Delay (5);
		_i += 1;
		if (_i == 200)//218)
		{
			Motor_CTL = CLOSE_MOTOR_SPEAKER;
			_i = 0;
			motor_num = 0;
		}
	}
}


void ad_sample ()
{
	unsigned int idata tmp;
	if (--sample_period == 0 || key2_set_mode) {
		//�ɼ���ǰ�¶�ֵ
		current_temp = RD_T(0);
		if (current_temp >= 100)	current_temp = 99;

		if (key2_set_mode != 1) {
			current_time [4] = current_temp/10;
			current_time [5] = current_temp%10;
		}
	}
	if (sample_period == 0) {
		// �ɼ���ǰʱ��
		//hour:
		tmp = Get1302Hour();
		current_time [0] = tmp/16;
		current_time [1] = tmp%16;
	
		//minute:
		tmp = Get1302Minute();
		current_time [2] = tmp/16;
		current_time [3] = tmp%16;

		sample_period = 50;
	}

}

