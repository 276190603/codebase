#include "stm32f10x.h"
#include "stm32f107.h"
#include "RS485.H"
#include "portarch.h"
#include "USART1.H"
#include "USART2.H"
#include "USART3.H"
volatile unsigned char RS485_REC_Flag = 0;
unsigned char RS485_buff[RS485_REC_BUFF_SIZE];//���ڽ�������
unsigned int RS485_rec_counter = 0;//����RS485���ռ���
volatile unsigned char UART5_REC_Flag = 0;
unsigned char UART5_buff[UART5_REC_BUFF_SIZE];//���ڽ�������
unsigned int UART5_rec_counter = 0;//����RS485���ռ���

//static void RS485_Delay(uint32_t nCount);

/******************UART4����****485�ӿ�**********************/

void USART_Configuration(void)
{ 
  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 
  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);    
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);    
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);      
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOE, ENABLE);		
 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);		   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 // GPIO_PinRemapConfig(GPIO_PartialRemap_UART4, ENABLE);
  
  USART_InitStructure.USART_BaudRate =9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(UART4, &USART_InitStructure); 
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  
  USART_Cmd(UART4, ENABLE); 
  USART_ClearITPendingBit(UART4, USART_IT_TC);//����ж�TCλ
 // RS_485_RX_EN;
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 // GPIO_PinRemapConfig(GPIO_PartialRemap_UART4, ENABLE);
  
  USART_InitStructure.USART_BaudRate =9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(USART2, &USART_InitStructure); 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
  USART_Cmd(USART2, ENABLE); 
  USART_ClearITPendingBit(USART2, USART_IT_TC);//����ж�TCλ
  USART2_RX_EN;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);	
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);		   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 // GPIO_PinRemapConfig(GPIO_PartialRemap_UART4, ENABLE);
  
  USART_InitStructure.USART_BaudRate =9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(USART3, &USART_InitStructure); 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
  USART_Cmd(USART3, ENABLE); 
  USART_ClearITPendingBit(USART3, USART_IT_TC);//����ж�TCλ
  USART3_RX_EN;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //GPIO_PinRemapConfig(GPIO_PartialRemap_USART1, ENABLE);
  
  USART_InitStructure.USART_BaudRate =9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  
  USART_Cmd(USART1, ENABLE); 
  USART_ClearITPendingBit(USART1, USART_IT_TC);//����ж�TCλ
  //USART1_RX_EN;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);		   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 // GPIO_PinRemapConfig(GPIO_PartialRemap_UART4, ENABLE);
  
  USART_InitStructure.USART_BaudRate =9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(UART5, &USART_InitStructure); 
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);  
  USART_Cmd(UART5, ENABLE); 
  USART_ClearITPendingBit(UART5, USART_IT_TC);//����ж�TCλ
}

/******UART4(485)���жϽ��պ���******************************************/
void UART4_IRQHandler(void)  
{
  unsigned char res;	    
  
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //���յ�����
  {	 
    
    res =USART_ReceiveData(UART4); 	//��ȡ���յ�������
    if(RS485_rec_counter<64)
    {
      RS485_buff[RS485_rec_counter]=res;		//��¼���յ���ֵ
      RS485_rec_counter++;						//������������1 
    } 
  }  
}


/***************UART4(485)���ͺ���*************************************/
//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void RS485_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
  //RS_485_TX_EN;		//485����ʹ��	
  for(i = 0;i < length;i ++)
  {			
    while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
    USART_SendData(UART4,send_buff[i]);
  }
  while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	 	
  RS485_rec_counter = 0;
 // RS_485_RX_EN;  		//485����ʹ��
}

/**************UART4(485)�շ���ʱ����**********************************************/

//static void RS485_Delay(uint32_t nCount)
//{ 
  //while(nCount > 0)
 // { 
  //	  nCount --;   
//  }
//}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(unsigned char *buf,unsigned char *len)
{
  unsigned char rxlen=RS485_rec_counter;
  unsigned char i=0;
  *len=0;				//Ĭ��Ϊ0
  //delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
  OSTimeDlyHMSM(0, 0, 0, 10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
  if(rxlen==RS485_rec_counter&&rxlen)//���յ�������,�ҽ��������
  {
    for(i=0;i<rxlen;i++)
    {
      buf[i]=RS485_buff[i];	
    }		
    *len=RS485_rec_counter;	//��¼�������ݳ���
    RS485_rec_counter=0;		//����
  }
}


/******UART4(485)���жϽ��պ���******************************************/
void UART5_IRQHandler(void)  
{
  unsigned char res;	    
  
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //���յ�����
  {	 
    
    res =USART_ReceiveData(UART5); 	//��ȡ���յ�������
    if(UART5_rec_counter<64)
    {
      UART5_buff[UART5_rec_counter]=res;		//��¼���յ���ֵ
      UART5_rec_counter++;						//������������1 
    } 
  }  
}


/***************UART5(485)���ͺ���*************************************/
//RS485����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void UART5_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
  //RS_485_TX_EN;		//485����ʹ��	
  for(i = 0;i < length;i ++)
  {			
    while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
    USART_SendData(UART5,send_buff[i]);
  }
  while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	 	
  UART5_rec_counter = 0;
 // RS_485_RX_EN;  		//485����ʹ��
}

/**************UART4(485)�շ���ʱ����**********************************************/

//static void RS485_Delay(uint32_t nCount)
//{ 
  //while(nCount > 0)
 // { 
  //	  nCount --;   
//  }
//}
//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void UART5_Receive_Data(unsigned char *buf,unsigned char *len)
{
  unsigned char rxlen=UART5_rec_counter;
  unsigned char i=0;
  *len=0;				//Ĭ��Ϊ0
  //delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
  OSTimeDlyHMSM(0, 0, 0, 10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
  if(rxlen==UART5_rec_counter&&rxlen)//���յ�������,�ҽ��������
  {
    for(i=0;i<rxlen;i++)
    {
      buf[i]=UART5_buff[i];	
    }		
    *len=UART5_rec_counter;	//��¼�������ݳ���
    UART5_rec_counter=0;		//����
  }
}