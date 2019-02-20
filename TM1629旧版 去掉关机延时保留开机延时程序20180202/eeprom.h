#ifndef __EEPROM_H__
#define __EEPROM_H__

//#include <reg51.H>
#include <STC12C52.H>



#define ISP_BYTE_READ      1   //Byte read
#define ISP_BYTE_PROGRAM   2   //Byte program(write)
#define ISP_SECTOR_ERASE   3   //Sector erase(512 bytes per time)
#define WAIT_TIME          3   //Set wait time: below 30M 0,below 24M 1,below 20M 2,below 12M 3, below 6M 4,below 3M 5,below 2M 6,below 1M 7


/* 15F204EA EEPROM sector space
sector1: 0x0000~0x01FF
sector2: 0x0200~0x03FF
*/
#define T_S 0x1600//0x0000
#define T_G 0x1610//0x0010
#define LAST_WORK_MODE 0x1620//0x0020
#define HAVE_SET_TIME 0x1630//0x0030
#define T_SET 0x1640//0x0040

#define S_HOUR_S 0x1700//0x0200




/********************
* *名称：ISP_IAP_enable
 *功能：打开eeprom
*********************/
void ISP_IAP_enable(void);

/********************
* *名称：ISP_IAP_disable
* *功能：关闭eeprom
*********************/
void ISP_IAP_disable(void);

/********************
* *名称：ByteRead
* *功能：从eeprom中读出一个字节
*********************/
//unsigned char ByteRead(unsigned int addr);

/********************
* *名称：ByteProgram
* *功能：对eeprom中一个字节进行编程
*********************/
//void ByteProgram(unsigned int addr,unsigned char input_data);

/********************
* *名称：SectorErase
* *功能：擦除eeprom中的内容
*********************/
//void SectorErase(unsigned int sector_addr);

extern volatile unsigned char current_time [6];   //当前时间值（设置）
extern volatile unsigned char set_start_time [6], set_stop_time [6];  //设定结束时间
extern volatile  unsigned char idata set_start_time2 [6], set_stop_time2 [6]; 
extern volatile  unsigned char idata set_start_time3 [6], set_stop_time3 [6];   
extern volatile unsigned char work_mode, have_set_time;
extern unsigned char idata gSetTmpH, gSetTmpL;

void read_eeprom_time_temp () ;
void write_eeprom_time_temp () ;

#endif