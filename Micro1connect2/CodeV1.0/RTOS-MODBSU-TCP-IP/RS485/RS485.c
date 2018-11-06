#include "stm32f10x.h"
#include "stm32f107.h"
#include "RS485.H"
#include "portarch.h"
#include "USART1.H"
#include "USART2.H"
#include "USART3.H"
volatile unsigned char RS485_REC_Flag = 0;
unsigned char RS485_buff[RS485_REC_BUFF_SIZE];//用于接收数据
unsigned int RS485_rec_counter = 0;//用于RS485接收计数
volatile unsigned char UART5_REC_Flag = 0;
unsigned char UART5_buff[UART5_REC_BUFF_SIZE];//用于接收数据
unsigned int UART5_rec_counter = 0;//用于RS485接收计数

//static void RS485_Delay(uint32_t nCount);

/******************UART4配置****485接口**********************/

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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//偶校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
  USART_Init(UART4, &USART_InitStructure); 
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  
  USART_Cmd(UART4, ENABLE); 
  USART_ClearITPendingBit(UART4, USART_IT_TC);//清除中断TC位
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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//偶校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
  USART_Init(USART2, &USART_InitStructure); 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
  USART_Cmd(USART2, ENABLE); 
  USART_ClearITPendingBit(USART2, USART_IT_TC);//清除中断TC位
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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//偶校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
  USART_Init(USART3, &USART_InitStructure); 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
  USART_Cmd(USART3, ENABLE); 
  USART_ClearITPendingBit(USART3, USART_IT_TC);//清除中断TC位
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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//偶校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  
  USART_Cmd(USART1, ENABLE); 
  USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断TC位
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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//偶校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
  USART_Init(UART5, &USART_InitStructure); 
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);  
  USART_Cmd(UART5, ENABLE); 
  USART_ClearITPendingBit(UART5, USART_IT_TC);//清除中断TC位
}

/******UART4(485)的中断接收函数******************************************/
void UART4_IRQHandler(void)  
{
  unsigned char res;	    
  
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //接收到数据
  {	 
    
    res =USART_ReceiveData(UART4); 	//读取接收到的数据
    if(RS485_rec_counter<64)
    {
      RS485_buff[RS485_rec_counter]=res;		//记录接收到的值
      RS485_rec_counter++;						//接收数据增加1 
    } 
  }  
}


/***************UART4(485)发送函数*************************************/
//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
  //RS_485_TX_EN;		//485发送使能	
  for(i = 0;i < length;i ++)
  {			
    while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
    USART_SendData(UART4,send_buff[i]);
  }
  while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	 	
  RS485_rec_counter = 0;
 // RS_485_RX_EN;  		//485接收使能
}

/**************UART4(485)收发延时函数**********************************************/

//static void RS485_Delay(uint32_t nCount)
//{ 
  //while(nCount > 0)
 // { 
  //	  nCount --;   
//  }
//}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(unsigned char *buf,unsigned char *len)
{
  unsigned char rxlen=RS485_rec_counter;
  unsigned char i=0;
  *len=0;				//默认为0
  //delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
  OSTimeDlyHMSM(0, 0, 0, 10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
  if(rxlen==RS485_rec_counter&&rxlen)//接收到了数据,且接收完成了
  {
    for(i=0;i<rxlen;i++)
    {
      buf[i]=RS485_buff[i];	
    }		
    *len=RS485_rec_counter;	//记录本次数据长度
    RS485_rec_counter=0;		//清零
  }
}


/******UART4(485)的中断接收函数******************************************/
void UART5_IRQHandler(void)  
{
  unsigned char res;	    
  
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //接收到数据
  {	 
    
    res =USART_ReceiveData(UART5); 	//读取接收到的数据
    if(UART5_rec_counter<64)
    {
      UART5_buff[UART5_rec_counter]=res;		//记录接收到的值
      UART5_rec_counter++;						//接收数据增加1 
    } 
  }  
}


/***************UART5(485)发送函数*************************************/
//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void UART5_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
  //RS_485_TX_EN;		//485发送使能	
  for(i = 0;i < length;i ++)
  {			
    while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
    USART_SendData(UART5,send_buff[i]);
  }
  while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	 	
  UART5_rec_counter = 0;
 // RS_485_RX_EN;  		//485接收使能
}

/**************UART4(485)收发延时函数**********************************************/

//static void RS485_Delay(uint32_t nCount)
//{ 
  //while(nCount > 0)
 // { 
  //	  nCount --;   
//  }
//}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void UART5_Receive_Data(unsigned char *buf,unsigned char *len)
{
  unsigned char rxlen=UART5_rec_counter;
  unsigned char i=0;
  *len=0;				//默认为0
  //delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
  OSTimeDlyHMSM(0, 0, 0, 10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
  if(rxlen==UART5_rec_counter&&rxlen)//接收到了数据,且接收完成了
  {
    for(i=0;i<rxlen;i++)
    {
      buf[i]=UART5_buff[i];	
    }		
    *len=UART5_rec_counter;	//记录本次数据长度
    UART5_rec_counter=0;		//清零
  }
}