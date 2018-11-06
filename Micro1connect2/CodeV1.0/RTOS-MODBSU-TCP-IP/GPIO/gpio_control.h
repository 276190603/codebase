#ifndef _GPIO_CON_
#define _GPIO_CON_


//#define HWD  PBout(13)


void Hardware_Watchdog_init(void);

void buzzer_Init(void);
void relay_Init(void); //IO≥ı ºªØrelay
void Hardware_reset(void);

#define buzer_on   GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define buzer_off  GPIO_SetBits(GPIOA,GPIO_Pin_0)		

#define relay_on  GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define relay_off GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define Hardware_reset_read  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)


#endif
