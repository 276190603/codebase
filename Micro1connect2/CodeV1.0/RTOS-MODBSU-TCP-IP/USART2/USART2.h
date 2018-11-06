#ifndef  _USART2_H_
#define  _USART2_H_

#define USART2_RX_EN 	GPIO_ResetBits(GPIOA , GPIO_Pin_1)	//����ʹ��
#define USART2_TX_EN 	GPIO_SetBits(GPIOA , GPIO_Pin_1)  	//����ʹ��
#define USART2_REC_BUFF_SIZE				100
#define USART2_END_FLAG1	'#'			//USART2һ�����ݽ�����־1 
#define USART2_END_FLAG2	'*'			//USART2һ�����ݽ�����־2 

extern volatile unsigned char USART2_REC_Flag ;
extern unsigned char USART2_buff[USART2_REC_BUFF_SIZE] ;//���ڽ�������
extern unsigned int USART2_rec_counter ;//����USART2���ռ���
extern unsigned char localAddr ;

void USART2_Send_Data(unsigned char *send_buff,unsigned int length);
//static void USART2_Delay(uint32_t nCount);



void  USART2_Send_Data(unsigned char *send_buff,unsigned int length);
void USART2_Receive_Data(unsigned char *buf,unsigned char *len);

void checkComm2Modbus(void);
unsigned short int setRegisterVal2(unsigned short int addr,unsigned short int tempData);


#endif

