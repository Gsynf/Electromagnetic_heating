#ifndef _ENTER_WORK_MODE
#define _ENTER_WORK_MODE

#include <STC12C52.H>




/*********************************************
	Ӱ��Ķ˿�
	��1������Ҳ��ĸ���������������
**********************************************/
#define OPEN_CTL 1
#define CLOSE_CTL 0

/*********************************************
	��1���̵����������
	��2��������
**********************************************/
#define OPEN_MOTOR_SPEAKER 1
#define CLOSE_MOTOR_SPEAKER 0

/*�ұ��ĸ�������Ӧ����*/
sbit CTL1= P2^3;
sbit CTL2= P2^2;
sbit CTL3= P2^1; 
sbit CTL4= P2^6;
/*���������*/
sbit Motor_CTL = P2^5;
/*����������*/
sbit Speaker_CTL = P1^2;

extern volatile unsigned int current_temp;
extern volatile unsigned char key2_set_mode, key3_shift_mode, key1_mode;
extern volatile unsigned char is_heat_now, motor_num, is_work, need_keep_temp;
extern unsigned char idata work_mode_dis[6];
//��������
extern volatile unsigned char idata sample_period;
extern unsigned char idata gSetTmpH, gSetTmpL;

void switch_off (void); 					//�ر�����
void clear_switch (void);  					//�������
void enter_work_mode (void);				//���빤��ģʽ
void enter_protect_mode (void);             //���뱣��ģʽ
void motor_timer (void);                    //��ﶨʱ��Լ1����

void ad_sample ();


#endif
