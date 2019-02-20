#include "eeprom.h"
#include "intrins.h"

/********************
* *���ƣ�ISP_IAP_enable
* *���ܣ���eeprom
*********************/
void ISP_IAP_enable(void)
{
	EA	=	0;								/* ���ж� */
	ISP_CONTR	=	ISP_CONTR & 0xf8;       /* 1111,1000 */
	ISP_CONTR	=	ISP_CONTR | WAIT_TIME;	/* ���õȴ�ʱ�� */
	ISP_CONTR	=	ISP_CONTR | 0x80;       /* 1000,0000 ����ISP/IAP����*/	
}

/********************
* *���ƣ�ISP_IAP_disable
* *���ܣ��ر�eeprom
*********************/
void ISP_IAP_disable(void)
{
	ISP_CONTR	=	0x00;	
	ISP_CMD     =   0x00;
	ISP_TRIG	=	0x00;
	ISP_ADDRH   =   0x80;
	ISP_ADDRL   =   0x00;
	EA	=   1;                	/* ���ж� */
}

/********************
* *���ƣ�ByteRead
* *���ܣ���eeprom�ж���һ���ֽ�
*********************/
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


/********************
* *���ƣ�ByteProgram
* *���ܣ���eeprom��һ���ֽڽ��б��
*********************/
void ByteProgram(unsigned int addr,unsigned char input_data) 
{	
    ISP_IAP_enable();

    ISP_DATA=input_data;

    ISP_ADDRL=addr;
    ISP_ADDRH=addr>>8;

    ISP_CMD = ISP_CMD & 0xfc;        /* 1111,1100 */
    ISP_CMD = ISP_CMD | ISP_BYTE_PROGRAM;	/* 0000,0010 ���ֽ�д����*/

    ISP_TRIG=0x5A;
    ISP_TRIG=0xA5;

    _nop_();

    ISP_IAP_disable();
}


/********************
* *���ƣ�SectorErase
* *���ܣ�����eeprom�е�����
*********************/
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
static unsigned char i;
/*************************************
* *���ܣ� ��ʱ��ֵ���¶�ֵд�� eeprom
*************************************/
void write_eeprom_time_temp () {
	//д�趨������¶�ֵ
	SectorErase(T_S); 
	ByteProgram (T_S, gSetTmpH);
	ByteProgram (T_G, gSetTmpL);

	ByteProgram(LAST_WORK_MODE, work_mode);
	ByteProgram(HAVE_SET_TIME, 1);

	for (i=0; i<2; i++) 
		 ByteProgram(T_SET + i*16, set_start_time[4+i]);         
	for (; i<4; i++) 
		ByteProgram(T_SET + i*16, set_start_time2[2+i]);        
	for (; i<6; i++) 
		ByteProgram(T_SET + i*16, set_start_time3[i]); 


	//д�趨����ʼ�����ʱ��ֵ
	SectorErase(S_HOUR_S);
	for (i=0; i<4; i++)   
		ByteProgram (S_HOUR_S + i* 16, set_start_time [i]);              
	for (; i<8; i++)     
		ByteProgram (S_HOUR_S + i* 16, set_stop_time [i-4]); 

	for (; i<12; i++)     
		ByteProgram (S_HOUR_S + i* 16, set_start_time2 [i-8]); 
	for (; i<16; i++)     
		ByteProgram (S_HOUR_S + i* 16, set_stop_time2 [i-12]);
		
	for (; i<20; i++)     
		ByteProgram (S_HOUR_S + i* 16, set_start_time3 [i-16]); 
	for (; i<24; i++)     
		ByteProgram (S_HOUR_S + i* 16, set_stop_time3 [i-20]);                  
	

}

/*************************************
* *���ܣ� �� eeprom ���� ��ʱ��ֵ���¶�ֵ
*************************************/
void read_eeprom_time_temp () {
	work_mode = ByteRead ( LAST_WORK_MODE );
	have_set_time =  ByteRead(HAVE_SET_TIME);
	if ( have_set_time != 1) { // оƬ��һ���ϵ�
	//	 have_set_time = 0;
		 return;
	}

	gSetTmpH = ByteRead (T_S);
	gSetTmpL = ByteRead (T_G);
	// ��ȡ�¶��趨ֵ
	for (i=0; i<2; i++) 
		set_start_time[4+i] = ByteRead(T_SET + i*16);         
	for (; i<4; i++) 
		set_start_time2[2+i] = ByteRead(T_SET + i*16);        
	for (; i<6; i++) 
		set_start_time3[i] = ByteRead(T_SET + i*16);       

	//��eeprom���趨����ʼ�ͽ���ʱ��ֵ
	for (i=0; i<4; i++)                 
		set_start_time [i] = ByteRead (S_HOUR_S + i* 16);
	for (; i<8; i++)                    
		set_stop_time [i-4] = ByteRead (S_HOUR_S + i* 16);

	for (; i<12; i++)                    
		set_start_time2 [i-8] = ByteRead (S_HOUR_S + i* 16);
	for (; i<16; i++)                    
		set_stop_time2 [i-12] = ByteRead (S_HOUR_S + i* 16);
	
	for (; i<20; i++)                    
		set_start_time3 [i-16] = ByteRead (S_HOUR_S + i* 16);
	for (; i<24; i++)                    
		set_stop_time3 [i-20] = ByteRead (S_HOUR_S + i* 16);
}
