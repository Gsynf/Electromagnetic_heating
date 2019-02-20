#ifndef DS1302_NEW_H_
#define DS1302_NEW_H_

void Set1302M(unsigned char *pClock); 

/* 读取1302当前时间 */
unsigned char   Get1302Minute(); 
unsigned char   Get1302Hour(); 


#endif
