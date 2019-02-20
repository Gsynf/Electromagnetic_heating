#ifndef _ENTER_WORK_MODE
#define _ENTER_WORK_MODE

#include <STC12C52.H>




/*********************************************
	影响的端口
	（1）面板右侧四个加热驱动的引脚
**********************************************/
#define OPEN_CTL 1
#define CLOSE_CTL 0

/*********************************************
	（1）继电器（电机马达）
	（2）扬声器
**********************************************/
#define OPEN_MOTOR_SPEAKER 1
#define CLOSE_MOTOR_SPEAKER 0

/*右边四个驱动对应引脚*/
sbit CTL1= P2^3;
sbit CTL2= P2^2;
sbit CTL3= P2^1; 
sbit CTL4= P2^6;
/*电机马达控制*/
sbit Motor_CTL = P2^5;
/*扬声器控制*/
sbit Speaker_CTL = P1^2;

extern volatile unsigned int current_temp;
extern volatile unsigned char key2_set_mode, key3_shift_mode, key1_mode;
extern volatile unsigned char is_heat_now, motor_num, is_work, need_keep_temp;
extern unsigned char idata work_mode_dis[6];
//采样周期
extern volatile unsigned char idata sample_period;
extern unsigned char idata gSetTmpH, gSetTmpL;

void switch_off (void); 					//关闭驱动
void clear_switch (void);  					//清空驱动
void enter_work_mode (void);				//进入工作模式
void enter_protect_mode (void);             //进入保护模式
void motor_timer (void);                    //马达定时大约1分钟

void ad_sample ();


#endif
