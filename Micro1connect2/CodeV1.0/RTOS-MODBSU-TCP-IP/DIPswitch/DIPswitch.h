#ifndef _DIPSWITCH_H_
#define _DIPSWITCH_H_

#define DIPswitch0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define DIPswitch1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define DIPswitch2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
#define DIPswitch3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
#define DIPswitch4  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)
#define DIPswitch5  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define DIPswitch6  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
#define DIPswitch7  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)

void DIPswitch_Init(void);
unsigned char DIPswitch(void);


#endif
