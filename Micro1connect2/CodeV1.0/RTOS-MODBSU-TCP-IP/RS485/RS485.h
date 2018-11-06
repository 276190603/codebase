#ifndef  _485_H__H_
#define  _485_H__H_

//#define RS_485_RX_EN 	GPIO_ResetBits(GPIOC , GPIO_Pin_12)	//����ʹ��
//#define RS_485_TX_EN 	GPIO_SetBits(GPIOC , GPIO_Pin_12)  	//����ʹ��
#define RS485_REC_BUFF_SIZE				100
#define UART5_REC_BUFF_SIZE				100
#define RS485_END_FLAG1	'#'			//RS485һ�����ݽ�����־1 
#define RS485_END_FLAG2	'*'			//RS485һ�����ݽ�����־2 

extern volatile unsigned char RS485_REC_Flag ;
extern unsigned char RS485_buff[RS485_REC_BUFF_SIZE] ;//���ڽ�������
extern unsigned int RS485_rec_counter ;//����RS485���ռ���

extern volatile unsigned char UART5_REC_Flag ;
extern unsigned char UART5_buff[UART5_REC_BUFF_SIZE] ;//���ڽ�������
extern unsigned int UART5_rec_counter ;//����RS485���ռ���


void USART_Configuration(void);
void  RS485_Send_Data(unsigned char *send_buff,unsigned int length);
void RS485_Receive_Data(unsigned char *buf,unsigned char *len);
void  UART5_Send_Data(unsigned char *send_buff,unsigned int length);
void UART5_Receive_Data(unsigned char *buf,unsigned char *len);

#endif

