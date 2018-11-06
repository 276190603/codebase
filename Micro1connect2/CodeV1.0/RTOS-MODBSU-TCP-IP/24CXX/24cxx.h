#ifndef __24CXX_H
#define __24CXX_H
#include "myiic.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//24CXX驱动 代码(适合24C01~24C16,24C32~256未经过测试!有待验证!)		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//Mini STM32开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C16
					  
unsigned char AT24CXX_ReadOneByte(unsigned short ReadAddr);							//指定地址读取一个字节
void AT24CXX_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite);		//指定地址写入一个字节
void AT24CXX_WriteLenByte(unsigned short WriteAddr,unsigned int DataToWrite,unsigned char Len);//指定地址开始写入指定长度的数据
unsigned int AT24CXX_ReadLenByte(unsigned short ReadAddr,unsigned char Len);					//指定地址开始读取指定长度数据
void AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite);	//从指定地址开始写入指定长度的数据
void AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead);   	//从指定地址开始读出指定长度的数据

unsigned char AT24CXX_Check(void);  //检查器件
void AT24CXX_Init(void); //初始化IIC
void delay_us(unsigned int nus);
void delay_ms(unsigned short nms);
u8 AT24CXX_Check_R(u8 i);
u8 AT24CXX_Check_R2(u8 i);
void AT24CXX_Read_Setdata(void);
void AT24CXX_Reset_Alldata(void);
void AT24CXX_Reset_Rdata(void);
#endif
















