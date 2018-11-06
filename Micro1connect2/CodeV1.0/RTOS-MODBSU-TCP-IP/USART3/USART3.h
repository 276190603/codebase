#ifndef  _USART3_H_
#define  _USART3_H_

#define USART3_RX_EN 	GPIO_ResetBits(GPIOE , GPIO_Pin_15)	//接收使能
#define USART3_TX_EN 	GPIO_SetBits(GPIOE , GPIO_Pin_15)  	//发送使能
#define USART3_REC_BUFF_SIZE				100
#define USART3_END_FLAG1	'#'			//USART3一桢数据结束标志1 
#define USART3_END_FLAG2	'*'			//USART3一桢数据结束标志2 

extern volatile unsigned char USART3_REC_Flag ;
extern unsigned char USART3_buff[USART3_REC_BUFF_SIZE] ;//用于接收数据
extern unsigned int USART3_rec_counter ;//用于USART3接收计数
extern unsigned char localAddr ;

void USART3_Send_Data(unsigned char *send_buff,unsigned int length);
//static void USART3_Delay(uint32_t nCount);



void  USART3_Send_Data(unsigned char *send_buff,unsigned int length);
void USART3_Receive_Data(unsigned char *buf,unsigned char *len);

void checkComm3Modbus(void);
unsigned short int setRegisterVal3(unsigned short int addr,unsigned short int tempData);


#endif

