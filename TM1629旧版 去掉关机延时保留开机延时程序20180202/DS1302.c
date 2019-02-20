#include <reg51.h>
#include <absacc.h>
#include <intrins.h>

#include "DS1302.h"

sbit T_CLK=P1^3;
sbit T_IO =P1^4;
sbit T_RST=P1^5;



//********DS1302读写程序***************
/********************************************************************
函 数 名：RTInputByteM()
功    能：实时时钟写入一字节
说    明：往DS1302写入1Byte数据 (内部函数)
入口参数：d 写入的数据
返 回 值：无  
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
函 数 名：RTOutputByteM()
功    能：实时时钟读取一字节
说    明：从DS1302读取1Byte数据 (内部函数)
入口参数：无  
返 回 值：ACC
设    计：zhaojunjie           日    期：2002-03-19
修    改：                     日    期：
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
函 数 名：Write1302()功    能：往DS1302写入数据
说    明：先写地址，后写命令/数据 (内部函数)
调    用：RTInputByteM() , RTOutputByteM()
入口参数：ucAddr: DS1302地址, ucData: 要写的数据
返 回 值：无
***********************************************************************/
void Write1302(unsigned char idata ucAddr, unsigned char idata ucDa)
{
    T_RST = 0; _nop_();_nop_();_nop_();
    T_CLK = 0;_nop_();_nop_();_nop_();
	T_RST = 1;_nop_();_nop_();_nop_();

 	RTInputByteM(ucAddr);   // 地址，命令

	_nop_();_nop_();_nop_();
 	RTInputByteM(ucDa);         // 写1Byte数据

    T_CLK = 1;_nop_();_nop_();_nop_();
    T_RST = 0;_nop_();_nop_();_nop_();
}							   
/********************************************************************
函 数 名：Read1302()
功    能：读取DS1302某地址的数据
说    明：先写地址，后读命令/数据 (内部函数)
调    用：RTInputByteM() , RTOutputByteM()
入口参数：ucAddr: DS1302地址
返 回 值：ucData :读取的数据
***********************************************************************/
unsigned char Read1302(unsigned char ucAddr)
{
    unsigned char idata ucData;
    T_RST = 0;_nop_();_nop_();_nop_();
    T_CLK = 0;_nop_();_nop_();_nop_();
    T_RST = 1;_nop_();_nop_();_nop_();
    
	RTInputByteM(ucAddr);             // 地址，命令
	_nop_();_nop_();_nop_();
 	ucData = RTOutputByteM();         // 读1Byte数据

    T_CLK = 1;_nop_();_nop_();_nop_();
    T_RST = 0;_nop_();_nop_();_nop_();
    
	return(ucData);
}
/********************************************************************
函 数 名：Set1302M()
功    能：设置初始时间
说    明：先写地址，后读命令/数据(寄存器多字节方式)
调    用：Write1302()
入口参数：pClock: 设置时钟数据地址 格式为: 秒 分 时 日 月 星期年
                               7Byte (BCD码)1B 1B 1B 1B 1B  1B  1B
返 回 值：无
***********************************************************************/
void Set1302M(unsigned char *pClock)
{
    unsigned char idata i;
    unsigned char idata ucAddr = 0x80;
    Write1302(0x8e,0x00);           // 控制命令,WP=0,写操作
    for(i =7; i>0; i--)
    {
     	Write1302(ucAddr,*pClock);  // 秒 分 时 日 月 星期 年
        pClock++;
        ucAddr +=2;
    }
    
	Write1302(0x8e,0x80);           // 控制命令,WP=1,写保护
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


