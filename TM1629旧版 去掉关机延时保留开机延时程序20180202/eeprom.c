#include "eeprom.h"
#include "intrins.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

/**/
sfr IAP_DATA = 0xC2;
sfr IAP_ADDRH = 0xC3;
sfr IAP_ADDRL = 0xC4;
sfr IAP_CMD = 0xC5;
sfr IAP_TRIG = 0xC6;
sfr IAP_CONTR = 0xC7;

#define CMD_IDLE     0
#define CMD_READ     1
#define CMD_PROGRAM  2
#define CMD_ERASE    3

//#define ENABLE_IAP  0x81 // if SYSCLK < 24MHZ
#define ENABLE_IAP  0x82 // if SYSCLK < 20MHZ


#define IAP_ADDRESS 0x0000

void IapIdle ()
{
	IAP_CONTR = 0;
	IAP_CMD = 0;
	IAP_TRIG = 0;
	IAP_ADDRH = 0x80;
	IAP_ADDRL = 0;
}

BYTE ByteRead (WORD addr) {
	BYTE dat;

	IAP_CONTR = ENABLE_IAP;
	IAP_CMD=CMD_READ;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;

	_nop_();

	dat = IAP_DATA;
	IapIdle();

	return dat;
}

void IapProgramByte(WORD addr, BYTE dat) {
	IAP_CONTR=ENABLE_IAP;
	IAP_CMD=CMD_PROGRAM;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_DATA=dat;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
}


void IapEraseSector(WORD addr)
{
	IAP_CONTR=ENABLE_IAP;
	IAP_CMD=CMD_ERASE;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;

	_nop_();
	IapIdle();
}


/*
void ISP_IAP_enable(void)
{
	EA	=	0;								//* ���ж� 
	ISP_CONTR	=	ISP_CONTR & 0xf8;       // 1111,1000 
	ISP_CONTR	=	ISP_CONTR | WAIT_TIME;	// ���õȴ�ʱ�� 
	ISP_CONTR	=	ISP_CONTR | 0x80;       //* 1000,0000 ����ISP/IAP����	
}


void ISP_IAP_disable(void)
{
	ISP_CONTR	=	0x00;	
	ISP_CMD     =   0x00;
	ISP_TRIG	=	0x00;
	ISP_ADDRH   =   0x80;
	ISP_ADDRL   =   0x00;
	EA	=   1;                	//* ���ж� 
}

unsigned char ByteRead(unsigned int addr)   
{
    ISP_IAP_enable();

	ISP_ADDRL=addr;
    ISP_ADDRH=addr>>8;   

	ISP_CMD = ISP_CMD & 0xfc;           //1111,1100	 
    ISP_CMD = ISP_CMD | ISP_BYTE_READ;  //0000,0001 ���ֽڶ�����
	 
    ISP_TRIG = 0x5A;
    ISP_TRIG = 0xA5;

    _nop_();

    ISP_IAP_disable();

    return  ISP_DATA;
}


void ByteProgram(unsigned int addr,unsigned char input_data) 
{	
    ISP_IAP_enable();

    ISP_DATA=input_data;

    ISP_ADDRL=addr;
    ISP_ADDRH=addr>>8;

    ISP_CMD = ISP_CMD & 0xfc;        //* 1111,1100 
    ISP_CMD = ISP_CMD | ISP_BYTE_PROGRAM;	//* 0000,0010 ���ֽ�д����

    ISP_TRIG=0x5A;
    ISP_TRIG=0xA5;

    _nop_();

    ISP_IAP_disable();
}


void SectorErase(unsigned int sector_addr)   //����һ����
{
	unsigned int get_sector_addr = 0;
	ISP_IAP_enable();

    get_sector_addr = (sector_addr & 0xfe00);  // 1111,1110,0000,0000; ȡ������ʼ��ַ 
    ISP_ADDRH = (get_sector_addr >> 8);
    ISP_ADDRL = 0x00;  
    
    ISP_CMD = ISP_CMD & 0xfc;	// 1111,1100 
    ISP_CMD = ISP_CMD | ISP_SECTOR_ERASE;	// 0000,0011 
       
    ISP_TRIG=0x5A;
    ISP_TRIG=0xA5;

    _nop_();

    ISP_IAP_disable();
}
*/

static unsigned char i;
/*************************************
* *���ܣ� ��ʱ��ֵ���¶�ֵд�� eeprom
*************************************/
void write_eeprom_time_temp () {
	//д�趨������¶�ֵ
	//SectorErase(T_S);
	//ByteProgram (T_S, gSetTmpH);
	 
	IapEraseSector(T_S);
	IapProgramByte(T_S, gSetTmpH);
	IapProgramByte(T_G, gSetTmpL);

	IapProgramByte(LAST_WORK_MODE, work_mode);
	IapProgramByte(HAVE_SET_TIME, 1);

	for (i=0; i<2; i++) 
		 IapProgramByte(T_SET + i, set_start_time[4+i]);         
	for (; i<4; i++) 
		IapProgramByte(T_SET + i, set_start_time2[2+i]);        
	for (; i<6; i++) 
		IapProgramByte(T_SET + i, set_start_time3[i]); 


	//д�趨����ʼ�����ʱ��ֵ
	//IapEraseSector(S_HOUR_S);
	for (i=0; i<4; i++)   
		IapProgramByte (S_HOUR_S + i, set_start_time [i]);              
	for (; i<8; i++)     
		IapProgramByte (S_HOUR_S + i, set_stop_time [i-4]); 

	for (; i<12; i++)     
		IapProgramByte (S_HOUR_S + i, set_start_time2 [i-8]); 
	for (; i<16; i++)     
		IapProgramByte (S_HOUR_S + i, set_stop_time2 [i-12]);
		
	for (; i<20; i++)     
		IapProgramByte (S_HOUR_S + i, set_start_time3 [i-16]); 
	for (; i<24; i++)     
		IapProgramByte (S_HOUR_S + i, set_stop_time3 [i-20]);                  
	

}

/*************************************
* *���ܣ� �� eeprom ���� ��ʱ��ֵ���¶�ֵ
*************************************/
void read_eeprom_time_temp () {
	
	work_mode = ByteRead ( LAST_WORK_MODE );
	if ( work_mode >= 7) { // оƬ��һ���ϵ�
		 work_mode = 4;
		 return;
	}
	have_set_time =  ByteRead(HAVE_SET_TIME);
	gSetTmpH = ByteRead (T_S);
	if (gSetTmpH == 0) gSetTmpH = 5;
	gSetTmpL = ByteRead (T_G);

	for (i=0; i<2; i++) 
		set_start_time[4+i] = ByteRead(T_SET + i);         
	for (; i<4; i++) 
		set_start_time2[2+i] = ByteRead(T_SET + i);        
	for (; i<6; i++) 
		set_start_time3[i] = ByteRead(T_SET + i);       

	//��eeprom���趨����ʼ�ͽ���ʱ��ֵ
	for (i=0; i<4; i++)                 
		set_start_time [i] = ByteRead (S_HOUR_S + i);
	for (; i<8; i++)                    
		set_stop_time [i-4] = ByteRead (S_HOUR_S + i);

	for (; i<12; i++)                    
		set_start_time2 [i-8] = ByteRead (S_HOUR_S + i);
	for (; i<16; i++)                    
		set_stop_time2 [i-12] = ByteRead (S_HOUR_S + i);
	
	for (; i<20; i++)                    
		set_start_time3 [i-16] = ByteRead (S_HOUR_S + i);
	for (; i<24; i++)                    
		set_stop_time3 [i-20] = ByteRead (S_HOUR_S + i);
}
