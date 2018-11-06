#include "stm32f10x.h"
#include "stm32f107.h"
#include "gpio_control.h"

void Hardware_Watchdog_init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  //��ʼ��Ӳ�����Ź�-->GPIOF.11�������
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTFʱ��
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PB.13 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.13
  GPIO_SetBits(GPIOB,GPIO_Pin_13);						 //PB.5 �����
}

//��������ʼ������
void buzzer_Init(void) //IO��ʼ��
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  //��ʼ��S2-->GPIOF.10��������
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTFʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PB.5 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
  GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PB.5 �����
}

void relay_Init(void) //IO��ʼ��relay
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  //��ʼ��S2-->GPIOF.10��������
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTFʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PB.5 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
  GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PB.5 �����
}
void Hardware_reset(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}