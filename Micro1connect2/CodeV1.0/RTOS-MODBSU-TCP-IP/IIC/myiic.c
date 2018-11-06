#include "myiic.h"
#include "24cxx.h"
//#include "delay.h"
#include "stm32f107.h"
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
 
//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOE, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_2|GPIO_Pin_3); 	//PB10,PB11 �����
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	GPIO_SetBits(GPIOE, GPIO_Pin_3) ;	  	  
	GPIO_SetBits(GPIOE, GPIO_Pin_2) ;
	delay_us(4);
 	GPIO_ResetBits(GPIOE, GPIO_Pin_3) ;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
	GPIO_ResetBits(GPIOE, GPIO_Pin_3) ;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	GPIO_SetBits(GPIOE, GPIO_Pin_2) ; 
        	delay_us(4);	
	GPIO_SetBits(GPIOE, GPIO_Pin_3) ;//����I2C���߽����ź�
						   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	GPIO_SetBits(GPIOE, GPIO_Pin_3) ;delay_us(1);	   
	GPIO_SetBits(GPIOE, GPIO_Pin_2) ;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
	SDA_OUT();
	GPIO_ResetBits(GPIOE, GPIO_Pin_3) ;
	delay_us(2);
	GPIO_SetBits(GPIOE, GPIO_Pin_2) ;
	delay_us(2);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
	SDA_OUT();
	GPIO_SetBits(GPIOE, GPIO_Pin_3) ;
	delay_us(2);
	GPIO_SetBits(GPIOE, GPIO_Pin_2) ;
	delay_us(2);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 	    
    GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			GPIO_SetBits(GPIOE, GPIO_Pin_3) ;
		else
			GPIO_ResetBits(GPIOE, GPIO_Pin_3) ;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		GPIO_SetBits(GPIOE, GPIO_Pin_2) ;
		delay_us(2); 
		GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        GPIO_ResetBits(GPIOE, GPIO_Pin_2) ; 
        delay_us(2);
		GPIO_SetBits(GPIOE, GPIO_Pin_2) ;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



























