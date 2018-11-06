#ifndef __MYIIC_H
#define __MYIIC_H
//#include "sys.h"
#include "stm32f10x_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO��������
#define SDA_IN()  {GPIOE->CRL&=0XFFFF0FFF;GPIOE->CRL|=8<<12;}
#define SDA_OUT() {GPIOE->CRL&=0XFFFF0FFF;GPIOE->CRL|=3<<12;}

//IO��������	 
//#define IIC_SCL=1;    GPIO_SetBits(GPIOB, GPIO_Pin_6) ;
//PBout(10) //SCL
//#define IIC_SDA    GPIO_SetBits(GPIOB, GPIO_Pin_7)  //SDA	 
#define READ_SDA   GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)
//PBin(11)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(unsigned char txd);			//IIC����һ���ֽ�
unsigned char IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
unsigned char IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(unsigned char daddr,unsigned char addr,unsigned char data);
unsigned char IIC_Read_One_Byte(unsigned char daddr,unsigned char addr);	  
#endif
















