#include "stm32f10x.h"
#include "stm32f107.h"
#include "DIPswitch.h"


void DIPswitch_Init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

u8 DIPswitch(void)
{
  u8 DIP_SW_KV=0;
  DIP_SW_KV = (DIPswitch7<<7)|(DIPswitch6<<6)|(DIPswitch5<<5)|(DIPswitch4<<4)|(DIPswitch3<<3)|(DIPswitch2<<2)|(DIPswitch1<<1)|(DIPswitch0<<0);
  
  return ~DIP_SW_KV;
}
