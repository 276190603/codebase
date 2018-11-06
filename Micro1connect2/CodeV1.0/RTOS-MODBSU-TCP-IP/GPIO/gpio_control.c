#include "stm32f10x.h"
#include "stm32f107.h"
#include "gpio_control.h"

void Hardware_Watchdog_init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  //初始化硬件看门狗-->GPIOF.11推挽输出
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTF时钟
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PB.13 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.13
  GPIO_SetBits(GPIOB,GPIO_Pin_13);						 //PB.5 输出高
}

//蜂鸣器初始化函数
void buzzer_Init(void) //IO初始化
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  //初始化S2-->GPIOF.10上拉输入
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTF时钟
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PB.5 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
  GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PB.5 输出高
}

void relay_Init(void) //IO初始化relay
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  //初始化S2-->GPIOF.10上拉输入
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTF时钟
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PB.5 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
  GPIO_SetBits(GPIOB,GPIO_Pin_12);						 //PB.5 输出高
}
void Hardware_reset(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}