#ifndef  _USART_H_
#define  _USART_H_

//#define USART1_RX_EN 	GPIO_ResetBits(GPIOC , GPIO_Pin_0)	//����ʹ��
//#define USART1_TX_EN 	GPIO_SetBits(GPIOC , GPIO_Pin_0)  	//����ʹ��
#define USART1_REC_BUFF_SIZE				100
#define USART1_END_FLAG1	'#'			//USART1һ�����ݽ�����־1 
#define USART1_END_FLAG2	'*'			//USART1һ�����ݽ�����־2 

extern volatile unsigned char USART1_REC_Flag ;
extern unsigned char USART1_buff[USART1_REC_BUFF_SIZE] ;//���ڽ�������
extern unsigned int USART1_rec_counter ;//����USART1���ռ���

void  App_InitTCPIP (void);
void USART1_Send_Data(unsigned char *send_buff,unsigned int length);
//static void USART1_Delay(uint32_t nCount);
void USART1_Configuration(void);


void  USART1_Send_Data(unsigned char *send_buff,unsigned int length);
void USART1_Receive_Data(unsigned char *buf,unsigned char *len);

void checkComm0Modbus(void);
//void checkComm_TCP(void);
unsigned short int setRegisterVal(unsigned short int addr,unsigned short int tempData);
unsigned short int setRegisterVal_tcpip(unsigned short int addr,unsigned short int tempData);
extern unsigned int IP_ADDR;
extern unsigned int NETMASK_ADDR;
extern unsigned int GW_ADDR;
extern unsigned char init_ok;
#endif

