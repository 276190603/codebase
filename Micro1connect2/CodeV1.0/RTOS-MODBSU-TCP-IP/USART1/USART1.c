#include "stm32f10x.h"
#include "stm32f107.h"
#include "USART1.H"
#include "portarch.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "24cxx.h"
#include "includes.h"
volatile unsigned char USART1_REC_Flag = 0;
unsigned char USART1_buff[USART1_REC_BUFF_SIZE];//用于接收数据
unsigned int USART1_rec_counter = 0;//用于USART1接收计数
//static void USART1_Delay(uint32_t nCount);
extern unsigned char get_R_T_flag,get_R_I_flag,get_R_T_flag2,get_R_I_flag2,Reset_Rdata;
unsigned char USART_RX_BUF[30];
unsigned char USART_TX_BUF[200];
extern  CPU_CHAR IP[20];
extern  CPU_CHAR NETMASK[20];
extern  CPU_CHAR GW[20];
unsigned int IP_ADDR;
unsigned int NETMASK_ADDR;
unsigned int GW_ADDR;
unsigned char init_ok;
unsigned short int setRegisterVal(unsigned short int addr,unsigned short int tempData);
unsigned char addard = 1;

unsigned short int getRegisterVal(unsigned short int addr,unsigned short int *tempData);
/******************USART1配置****485接口**********************/

//void USART1_Configuration(void)
//{ 
//  
// GPIO_InitTypeDef GPIO_InitStructure;
//  USART_InitTypeDef USART_InitStructure; 
////
////  
//// 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);    
//	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOD, ENABLE);		
//  
//
////  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//485收发控制IO
////  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
////  GPIO_Init(GPIOC, &GPIO_InitStructure);
//  	
////  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	         
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
////  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
////  GPIO_Init(GPIOD, &GPIO_InitStructure);		   
////
////  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	        
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
////  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
////  GPIO_Init(GPIOD, &GPIO_InitStructure);
//	//GPIO_PinRemapConfig(GPIO_PartialRemap_USART1, ENABLE);
//	
////	USART_InitStructure.USART_BaudRate =9600;
////  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
////  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
////  USART_InitStructure.USART_Parity = USART_Parity_No;//偶校验
////  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
////  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
////  USART_Init(USART1, &USART_InitStructure); 
////  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  
////  USART_Cmd(USART1, ENABLE); 
////  USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断TC位
////  USART1_RX_EN;
// 
//}

/******USART1(485)的中断接收函数******************************************/
void USART1_IRQHandler(void)  
{
  
  unsigned char res;	    
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收到数据
  {	 
    
    res =USART_ReceiveData(USART1); 	//读取接收到的数据
    if(USART1_rec_counter<64)
    {
      USART1_buff[USART1_rec_counter]=res;		//记录接收到的值
      USART1_rec_counter++;						//接收数据增加1 
    } 
  }  
}


/***************USART1(485)发送函数*************************************/
//USART1发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void USART1_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
  
  //USART1_TX_EN;		//485发送使能
  
  for(i = 0;i < length;i ++)
  {			
    
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
    USART_SendData(USART1,send_buff[i]);
  }
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	 	
  USART1_rec_counter = 0;
  //USART1_RX_EN;  		//485接收使能
}

/**************USART1(485)收发延时函数**********************************************/


//USART1查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void USART1_Receive_Data(unsigned char *buf,unsigned char *len)
{
  unsigned char rxlen=USART1_rec_counter;
  unsigned char i=0;
  *len=0;				//默认为0
  OSTimeDlyHMSM(0, 0, 0, 10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
  //delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
  if(rxlen==USART1_rec_counter&&rxlen)//接收到了数据,且接收完成了
  {
    for(i=0;i<rxlen;i++)
    {
      buf[i]=USART1_buff[i];	
    }		
    *len=USART1_rec_counter;	//记录本次数据长度
    USART1_rec_counter=0;		//清零
  }
}
unsigned char	sendCount;		//发送字节个数

/*==================================================================================*/
// 语法格式：void Send_Hex(unsigned char c)
// 功能描述: 发送十六进制数（移植时候是对外的接口函数）
// 入口参数: unsigned char c      
// 出口参数: 无
/*==================================================================================*/
void Send_Hex(unsigned char c)         
{	
  USART1_Send_Data(&c,1);
}
/*==================================================================================*/
// 语法格式：unsigned int crc16(unsigned char *puchMsg, unsigned int usDataLen) 
// 功能描述: 对数据进行CRC校验
// 入口参数: unsigned char *puchMsg, unsigned int usDataLen
//													puchMsg数据缓冲区的起始地址;usDataLen校验数据的长度
// 出口参数: unsigned int类型的校验结果
/*==================================================================================*/
/* CRC 高位字节值表 */ 
const unsigned char auchCRCHi[] = { 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
  0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
  0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
  0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
  0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
} ; 
/* CRC低位字节值表*/ 
const unsigned char auchCRCLo[] = { 
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
  0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
  0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
  0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
  0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
  0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
  0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
  0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
  0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
  0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
  0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
  0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
  0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
  0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
  0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
  0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
  0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
  0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
} ;
unsigned short int crc16(unsigned char *puchMsg, unsigned short int usDataLen) 
{ 
  unsigned char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */ 
  unsigned char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */ 
  unsigned char uIndex ; /* CRC循环中的索引 */ 
  while (usDataLen--) /* 传输消息缓冲区 */ 
  { 
    uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */ 
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
    uchCRCLo = auchCRCLo[uIndex] ; 
  } 
  return (uchCRCHi << 8 | uchCRCLo) ; 
}

/*==================================================================================*/
// 语法格式：void readRegisters(void) 
// 功能描述: 读寄存器，03命令的实现
// 入口参数: 无
// 出口参数: 无
/*==================================================================================*/
void readRegisters(void)
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int crcData;
  unsigned short int readCount;
  unsigned short int byteCount;
  unsigned short int i;
  unsigned short int tempData = 0;	
  
  addr = (USART_RX_BUF[2]<<8) + USART_RX_BUF[3];   //
  //tempAddr = addr & 0xfff;	
  
  tempAddr = addr;
  
  readCount = (USART_RX_BUF[4]<<8) + USART_RX_BUF[5];	//要读的个数
  //readCount = receBuf[5];
  
  byteCount = readCount * 2;
  
  for(i=0;i<byteCount;i+=2,tempAddr++)
  {
    getRegisterVal(tempAddr,&tempData);				
    USART_TX_BUF[i+3] = tempData >> 8;				   		
    USART_TX_BUF[i+4] = tempData & 0xff;			
  }
  
  USART_TX_BUF[0] = addard;
  USART_TX_BUF[1] = 3;
  USART_TX_BUF[2] = byteCount;
  byteCount += 3;
  crcData = crc16(USART_TX_BUF,byteCount);
  USART_TX_BUF[byteCount] = crcData >> 8;
  byteCount++;
  USART_TX_BUF[byteCount] = crcData & 0xff;
  
  sendCount = byteCount + 1;
  for(i=0;i<sendCount;i++)
  {
    Send_Hex(USART_TX_BUF[i]);
  }
}

void readRegisters04(void)
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int crcData;
  unsigned short int readCount;
  unsigned short int byteCount;
  unsigned short int i;
  unsigned short int tempData = 0;	
  
  addr = (USART_RX_BUF[2]<<8) + USART_RX_BUF[3];   //
  //tempAddr = addr & 0xfff;	
  
  tempAddr = addr;
  
  readCount = (USART_RX_BUF[4]<<8) + USART_RX_BUF[5];	//要读的个数
  //readCount = receBuf[5];
  
  byteCount = readCount * 2;
  
  for(i=0;i<byteCount;i+=2,tempAddr++)
  {
    //          if((tempAddr<36)||(tempAddr>=416)&&(tempAddr<452)||(tempAddr>=832)&&(tempAddr<868)||
    //             (tempAddr>=1248)&&(tempAddr<1284)||(tempAddr>=1664)&&(tempAddr<1700)||(tempAddr>=2080)&&(tempAddr<2098))
    //          {
    getRegisterVal(tempAddr,&tempData);				
    USART_TX_BUF[i+3] = tempData >> 8;				   		
    USART_TX_BUF[i+4] = tempData & 0xff;		
    //          }
  }
  
  USART_TX_BUF[0] = addard;
  USART_TX_BUF[1] = 4;
  USART_TX_BUF[2] = byteCount;
  byteCount += 3;
  crcData = crc16(USART_TX_BUF,byteCount);
  USART_TX_BUF[byteCount] = crcData >> 8;
  byteCount++;
  USART_TX_BUF[byteCount] = crcData & 0xff;
  
  sendCount = byteCount + 1;
  for(i=0;i<sendCount;i++)
  {
    Send_Hex(USART_TX_BUF[i]);
  }
}

/*==================================================================================*/
// 语法格式：void presetSingleRegister()
// 功能描述: 设置单个寄存器，06命令的实现
// 入口参数: 无
// 出口参数: 无
/*==================================================================================*/
void presetSingleRegister()
{
  unsigned short int addr;
  unsigned short int tempAddr;
  //	unsigned char byteCount;
  //	unsigned char setCount;
  unsigned short int crcData;
  unsigned short int tempData;
  unsigned short int i;
  addr = (USART_RX_BUF[2]<<8) | USART_RX_BUF[3];
  tempAddr = addr & 0xfff;
  tempData = (USART_RX_BUF[4]<<8) | USART_RX_BUF[5];
  
  setRegisterVal(tempAddr,tempData);			
  
  USART_TX_BUF[0] = addard;
  USART_TX_BUF[1] = 6;
  USART_TX_BUF[2] = addr >> 8;
  USART_TX_BUF[3] = addr & 0xff;
  USART_TX_BUF[4] = tempData>>8;
  USART_TX_BUF[5] = tempData&0xff;
  crcData = crc16(USART_TX_BUF,6);
  USART_TX_BUF[6] = crcData >> 8;
  USART_TX_BUF[7] = crcData & 0xff;
  sendCount = 8;
  for(i=0;i<sendCount;i++)
  {
    Send_Hex(USART_TX_BUF[i]);
  }	
}
/*==================================================================================*/
// 语法格式：void presetSingleRegister()
// 功能描述: 设置单个寄存器，07命令的实现
// 入口参数: 无
// 出口参数: 无
/*==================================================================================*/
void presetID()
{
  unsigned short int addr;
  unsigned short int tempAddr;
  //	unsigned char byteCount;
  //	unsigned char setCount;
  unsigned short int crcData;
  unsigned short int tempData;
  unsigned short int i;
  addr = (USART_RX_BUF[2]<<8) | USART_RX_BUF[3];
  tempAddr = addr & 0xfff;
  tempData = (USART_RX_BUF[4]<<8) | USART_RX_BUF[5];
  
  setRegisterVal(tempAddr,tempData);			
  
  USART_TX_BUF[0] = 0x01;
  USART_TX_BUF[1] = 5;
  USART_TX_BUF[2] = addr >> 8;
  USART_TX_BUF[3] = addr & 0xff;
  USART_TX_BUF[4] = tempData>>8;
  USART_TX_BUF[5] = tempData&0xff;
  crcData = crc16(USART_TX_BUF,6);
  USART_TX_BUF[6] = crcData >> 8;
  USART_TX_BUF[7] = crcData & 0xff;
  sendCount = 8;
  for(i=0;i<sendCount;i++)
  {
    Send_Hex(USART_TX_BUF[i]);
  }	
}
void ReadID()
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int crcData;
  unsigned short int readCount;
  unsigned short int byteCount;
  unsigned short int i;
  unsigned short int tempData = 0;	
  
  addr = (USART_RX_BUF[2]<<8) + USART_RX_BUF[3];   //
  tempAddr = addr;
  readCount = (USART_RX_BUF[4]<<8) + USART_RX_BUF[5];	//要读的个数
  byteCount = readCount * 2;
  for(i=0;i<byteCount;i+=2,tempAddr++)
  {
    getRegisterVal(tempAddr,&tempData);				
    USART_TX_BUF[i+3] = tempData >> 8;				   		
    USART_TX_BUF[i+4] = tempData & 0xff;		
  }
  USART_TX_BUF[0] = 0x01;
  USART_TX_BUF[1] = 7;
  USART_TX_BUF[2] = byteCount;
  byteCount += 3;
  crcData = crc16(USART_TX_BUF,byteCount);
  USART_TX_BUF[byteCount] = crcData >> 8;
  byteCount++;
  USART_TX_BUF[byteCount] = crcData & 0xff;
  
  sendCount = byteCount + 1;
  for(i=0;i<sendCount;i++)
  {
    Send_Hex(USART_TX_BUF[i]);
  }
}
/*==================================================================================*/
// 语法格式：void presetSingleRegister()
// 功能描述: 设置多个寄存器，16命令的实现
// 入口参数: 无
// 出口参数: 无
/*==================================================================================*/
void presetMultipleRegisters(void)
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int byteCount;
  unsigned short int setCount;
  unsigned short int crcData;
  unsigned short int tempData;
  
  unsigned short int i;
  
  //	addr = USART_RX_BUF[3];
  //	tempAddr = addr & 0xff;
  addr = (USART_RX_BUF[2]<<8) + USART_RX_BUF[3];
  tempAddr = addr;
  
  setCount = USART_RX_BUF[5];
  byteCount = USART_RX_BUF[6];	
  
  for(i=0;i<setCount;i++,tempAddr++)
  {
    tempData = (USART_RX_BUF[i*2+7]<<8) + USART_RX_BUF[i*2+8];
    
    setRegisterVal(tempAddr,tempData);			
  }
  
  USART_TX_BUF[0] = addard;
  USART_TX_BUF[1] = 16;
  USART_TX_BUF[2] = addr >> 8;
  USART_TX_BUF[3] = addr & 0xff;
  USART_TX_BUF[4] = setCount >> 8;
  USART_TX_BUF[5] = setCount & 0xff;
  crcData = crc16(USART_TX_BUF,6);
  USART_TX_BUF[6] = crcData >> 8;
  USART_TX_BUF[7] = crcData & 0xff;
  sendCount = 8;
  for(i=0;i<sendCount;i++)
  {
    Send_Hex(USART_TX_BUF[i]);
  }	
}
//检查uart0数据
void checkComm0Modbus(void)
{
  unsigned short int crcData;
  unsigned short int tempData;
  unsigned char len= 30;
  unsigned short int i;
  USART1_Receive_Data(USART_RX_BUF,&len);
  if(len>7)
  {
    switch(USART_RX_BUF[1])
    {
      
    case 3:				      //读取保持寄存器(一个或多个)
      
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF[0]==addard)
        {
          crcData = crc16(USART_RX_BUF,6);	
          if(crcData == USART_RX_BUF[7]+(USART_RX_BUF[6]<<8))
          {
            readRegisters();
            
          }
        }
      }						
      break;
    case 4:				      //读取保持寄存器(一个或多个)
      
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF[0]==addard)
        {
          crcData = crc16(USART_RX_BUF,6);	
          if(crcData == USART_RX_BUF[7]+(USART_RX_BUF[6]<<8))
          {
            readRegisters04();
            
          }
        }
        
      }						
      break;
    case 6:                   //设置单个寄存器
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF[0]==addard)
        {
          crcData = crc16(USART_RX_BUF,6);	
          if(crcData == USART_RX_BUF[7]+(USART_RX_BUF[6]<<8))
          {
            presetSingleRegister();
          }
        }
      }				
      break;
    case 16://设置多个寄存器
      if(USART_RX_BUF[0]==addard)
      {
        if(len>7)
        {
          tempData = (USART_RX_BUF[4]<<8) + USART_RX_BUF[5];
          tempData = tempData * 2;	//数据个数
          tempData += 9;
          
          if(len== tempData)
          {
            crcData = crc16(USART_RX_BUF,len-2);
            if(crcData == (USART_RX_BUF[len-2]<<8)+ USART_RX_BUF[len-1])
            {
              presetMultipleRegisters();
              len=0;
            }
          }
        }					
      }			
      
      break;	
    case 5:                   //设置单个寄存器
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF[0]==0x01)
        {             
          crcData = crc16(USART_RX_BUF,6);	
          if(crcData == USART_RX_BUF[7]+(USART_RX_BUF[6]<<8))
          {
            presetID();
          }
        }
      }				
      break; 
    case 7:				      //读取保持寄存器(一个或多个)
      
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF[0]==0x01)
        {
          crcData = crc16(USART_RX_BUF,6);	
          if(crcData == USART_RX_BUF[7]+(USART_RX_BUF[6]<<8))
          {
            ReadID();
            
          }
        }
        
      }						
      break;           
      
    default:
      break;			
    }
  }
  if(len == 6)
  {						
    len=0;
    if(USART_RX_BUF[0]==1)
    {
      if(USART_RX_BUF[5] ==(USART_RX_BUF[0]^USART_RX_BUF[1] ^USART_RX_BUF[2] ^USART_RX_BUF[3] ^USART_RX_BUF[4]))
      {
        sprintf((char*)IP,"%d.%d.%d.%d",USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4]);	
        //qw = AT24CXX_ReadOneByte(0x00);
        AT24CXX_WriteOneByte(0x118,USART_RX_BUF[1]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x119,USART_RX_BUF[2]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x120,USART_RX_BUF[3]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x121,USART_RX_BUF[4]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        USART_TX_BUF[0] = 0x79;
        USART_TX_BUF[1] = 0x75;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
      else
      {
        USART_TX_BUF[0] = 0x69;
        USART_TX_BUF[1] = 0x82;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
    }
    if(USART_RX_BUF[0]==2)
    {
      if(USART_RX_BUF[5] ==(USART_RX_BUF[0]^USART_RX_BUF[1] ^USART_RX_BUF[2] ^USART_RX_BUF[3] ^USART_RX_BUF[4]))
      {
        sprintf((char*)NETMASK,"%d.%d.%d.%d",USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4]);	
        AT24CXX_WriteOneByte(0x122,USART_RX_BUF[1]);
        
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x123,USART_RX_BUF[2]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x124,USART_RX_BUF[3]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x125,USART_RX_BUF[4]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        USART_TX_BUF[0] = 0x79;
        USART_TX_BUF[1] = 0x75;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
      else
      {
        USART_TX_BUF[0] = 0x69;
        USART_TX_BUF[1] = 0x82;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
    }
    if(USART_RX_BUF[0]==3)
    {	
      if(USART_RX_BUF[5] ==(USART_RX_BUF[0]^USART_RX_BUF[1] ^USART_RX_BUF[2] ^USART_RX_BUF[3] ^USART_RX_BUF[4]))
      {
        sprintf((char*)GW,"%d.%d.%d.%d",USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4]);	
        AT24CXX_WriteOneByte(0x126,USART_RX_BUF[1]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x127,USART_RX_BUF[2]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x128,USART_RX_BUF[3]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x129,USART_RX_BUF[4]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        USART_TX_BUF[0] = 0x79;
        USART_TX_BUF[1] = 0x75;
        
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
      else
      {
        USART_TX_BUF[0] = 0x69;
        USART_TX_BUF[1] = 0x82;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
    }
    
  }
  if(len == 1)
  {
    len = 0;
    if(USART_RX_BUF[0]==4)
    {	
      USART_TX_BUF[0] = 0x04;
      USART_TX_BUF[1] = IP_ADDR/256/256/256;
      USART_TX_BUF[2] = (IP_ADDR-256*256*256*USART_TX_BUF[1])/256/256;
      USART_TX_BUF[3] = (IP_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2])/256;
      USART_TX_BUF[4] = (IP_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2]-256*USART_TX_BUF[3]);
      USART_TX_BUF[5] =(USART_TX_BUF[0]^USART_TX_BUF[1] ^USART_TX_BUF[2] ^USART_TX_BUF[3] ^USART_TX_BUF[4]);
      for(i=0;i<6;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
      
    }
    if(USART_RX_BUF[0]==5)
    {	
      USART_TX_BUF[0] = 0x05;
      USART_TX_BUF[1] = NETMASK_ADDR/256/256/256;
      USART_TX_BUF[2] = (NETMASK_ADDR-256*256*256*USART_TX_BUF[1])/256/256;
      USART_TX_BUF[3] = (NETMASK_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2])/256;
      USART_TX_BUF[4] = (NETMASK_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2]-256*USART_TX_BUF[3]);
      USART_TX_BUF[5] =(USART_TX_BUF[0]^USART_TX_BUF[1] ^USART_TX_BUF[2] ^USART_TX_BUF[3] ^USART_TX_BUF[4]);
      for(i=0;i<6;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
    }
    if(USART_RX_BUF[0]==6)
    {	
      USART_TX_BUF[0] = 0x06;
      USART_TX_BUF[1] = GW_ADDR/256/256/256;
      USART_TX_BUF[2] = (GW_ADDR-256*256*256*USART_TX_BUF[1])/256/256;
      USART_TX_BUF[3] = (GW_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2])/256;
      USART_TX_BUF[4] = (GW_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2]-256*USART_TX_BUF[3]);
      USART_TX_BUF[5] =(USART_TX_BUF[0]^USART_TX_BUF[1] ^USART_TX_BUF[2] ^USART_TX_BUF[3] ^USART_TX_BUF[4]);
      for(i=0;i<6;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
    }
    if(USART_RX_BUF[0]==7)
    {	
      USART_TX_BUF[0] = 0x07;
      USART_TX_BUF[1] = init_ok;
      for(i=0;i<2;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
    }
    
  }
  
}
char qw = 0;
//检查uart0数据
void checkComm_TCP(void)
{
  unsigned char len= 30;
  USART1_Receive_Data(USART_RX_BUF,&len);
  unsigned short int i;
  if(len == 6)
  {						
    len=0;
    if(USART_RX_BUF[0]==1)
    {
      if(USART_RX_BUF[5] ==(USART_RX_BUF[0]^USART_RX_BUF[1] ^USART_RX_BUF[2] ^USART_RX_BUF[3] ^USART_RX_BUF[4]))
      {
        sprintf((char*)IP,"%d.%d.%d.%d",USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4]);	
        //qw = AT24CXX_ReadOneByte(0x00);
        AT24CXX_WriteOneByte(0x00,USART_RX_BUF[1]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x01,USART_RX_BUF[2]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x02,USART_RX_BUF[3]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x03,USART_RX_BUF[4]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        USART_TX_BUF[0] = 0x79;
        USART_TX_BUF[1] = 0x75;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
      else
      {
        USART_TX_BUF[0] = 0x69;
        USART_TX_BUF[1] = 0x82;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
    }
    if(USART_RX_BUF[0]==2)
    {
      if(USART_RX_BUF[5] ==(USART_RX_BUF[0]^USART_RX_BUF[1] ^USART_RX_BUF[2] ^USART_RX_BUF[3] ^USART_RX_BUF[4]))
      {
        sprintf((char*)NETMASK,"%d.%d.%d.%d",USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4]);	
        AT24CXX_WriteOneByte(0x04,USART_RX_BUF[1]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x05,USART_RX_BUF[2]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x06,USART_RX_BUF[3]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x07,USART_RX_BUF[4]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        USART_TX_BUF[0] = 0x79;
        USART_TX_BUF[1] = 0x75;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
      else
      {
        USART_TX_BUF[0] = 0x69;
        USART_TX_BUF[1] = 0x82;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
    }
    if(USART_RX_BUF[0]==3)
    {	
      if(USART_RX_BUF[5] ==(USART_RX_BUF[0]^USART_RX_BUF[1] ^USART_RX_BUF[2] ^USART_RX_BUF[3] ^USART_RX_BUF[4]))
      {
        sprintf((char*)GW,"%d.%d.%d.%d",USART_RX_BUF[1],USART_RX_BUF[2],USART_RX_BUF[3],USART_RX_BUF[4]);	
        AT24CXX_WriteOneByte(0x08,USART_RX_BUF[1]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x09,USART_RX_BUF[2]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x0A,USART_RX_BUF[3]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        AT24CXX_WriteOneByte(0x0B,USART_RX_BUF[4]);
        OSTimeDlyHMSM(0, 0, 0, 1);
        USART_TX_BUF[0] = 0x79;
        USART_TX_BUF[1] = 0x75;
        
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
      else
      {
        USART_TX_BUF[0] = 0x69;
        USART_TX_BUF[1] = 0x82;
        for(i=0;i<2;i++)
        {
          Send_Hex(USART_TX_BUF[i]);
        }	
      }
    }
    
  }
  if(len == 1)
  {
    len = 0;
    if(USART_RX_BUF[0]==4)
    {	
      USART_TX_BUF[0] = 0x04;
      USART_TX_BUF[1] = IP_ADDR/256/256/256;
      USART_TX_BUF[2] = (IP_ADDR-256*256*256*USART_TX_BUF[1])/256/256;
      USART_TX_BUF[3] = (IP_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2])/256;
      USART_TX_BUF[4] = (IP_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2]-256*USART_TX_BUF[3]);
      USART_TX_BUF[5] =(USART_TX_BUF[0]^USART_TX_BUF[1] ^USART_TX_BUF[2] ^USART_TX_BUF[3] ^USART_TX_BUF[4]);
      for(i=0;i<6;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
      
    }
    if(USART_RX_BUF[0]==5)
    {	
      USART_TX_BUF[0] = 0x05;
      USART_TX_BUF[1] = NETMASK_ADDR/256/256/256;
      USART_TX_BUF[2] = (NETMASK_ADDR-256*256*256*USART_TX_BUF[1])/256/256;
      USART_TX_BUF[3] = (NETMASK_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2])/256;
      USART_TX_BUF[4] = (NETMASK_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2]-256*USART_TX_BUF[3]);
      USART_TX_BUF[5] =(USART_TX_BUF[0]^USART_TX_BUF[1] ^USART_TX_BUF[2] ^USART_TX_BUF[3] ^USART_TX_BUF[4]);
      for(i=0;i<6;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
    }
    if(USART_RX_BUF[0]==6)
    {	
      USART_TX_BUF[0] = 0x06;
      USART_TX_BUF[1] = GW_ADDR/256/256/256;
      USART_TX_BUF[2] = (GW_ADDR-256*256*256*USART_TX_BUF[1])/256/256;
      USART_TX_BUF[3] = (GW_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2])/256;
      USART_TX_BUF[4] = (GW_ADDR-256*256*256*USART_TX_BUF[1]-256*256*USART_TX_BUF[2]-256*USART_TX_BUF[3]);
      USART_TX_BUF[5] =(USART_TX_BUF[0]^USART_TX_BUF[1] ^USART_TX_BUF[2] ^USART_TX_BUF[3] ^USART_TX_BUF[4]);
      for(i=0;i<6;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
    }
    if(USART_RX_BUF[0]==7)
    {	
      USART_TX_BUF[0] = 0x07;
      USART_TX_BUF[1] = init_ok;
      for(i=0;i<2;i++)
      {
        Send_Hex(USART_TX_BUF[i]);
      }	
    }
    
  }
  
  
}
//取寄存器值 返回0表示成功
unsigned short int getRegisterVal(unsigned short int addr,unsigned short int *tempData)
{
  unsigned short int result = 0;
  unsigned short int tempAddr;
  
  tempAddr = addr & 0xfff;
  tempAddr = addr & 0xfff;
  *tempData = usRegInputBuf[tempAddr & 0xfff];
  return result;
}
//设置寄存器值 返回0表示成功
unsigned short int setRegisterVal(unsigned short int addr,unsigned short int tempData)
{
  
  unsigned short int result = 0;
  unsigned short int tempAddr;
  union float_to_u16 data_buffer1;
  union float_to_u16 data_buffer4;
  
  tempAddr = addr & 0xfff;
  switch(tempAddr & 0xfff)
  {
  case 2900://一通道单体数 6
    usRegInputBuf[2900]= tempData;
    AT24CXX_WriteLenByte(0,usRegInputBuf[2900],2);
    break;
  case 2901://二通道单体数 6
    usRegInputBuf[2901]= tempData;
    AT24CXX_WriteLenByte(2,usRegInputBuf[2901],2);
    break;    
  case 2902://一通道只数 0
    usRegInputBuf[2902]= tempData;
    AT24CXX_WriteLenByte(8,usRegInputBuf[2902],2);
    break;
  case 2903://二通道只数 0
    usRegInputBuf[2903]= tempData;
    AT24CXX_WriteLenByte(10,usRegInputBuf[2903],2);
    break;
    
    //	
    //		
    
    //  case 2306://设备ID 
    //    usRegInputBuf[2306]= tempData;
    //    AT24CXX_WriteLenByte(24,usRegInputBuf[2306],2);
    //    addard = usRegInputBuf[2306];
    //    break;
    
  case 2905://一通道单体电压高限 2.5
    usRegInputBuf[2905]= tempData;
    AT24CXX_WriteLenByte(26,usRegInputBuf[2905],2);
    break;
  case 2906://一通道单体电压高限
    usRegInputBuf[2906]= tempData;
    AT24CXX_WriteLenByte(28,usRegInputBuf[2906],2);
    //取一通道单体电压高限
    data_buffer1.data16[1]=usRegInputBuf[2905];
    data_buffer1.data16[0]=usRegInputBuf[2906];
    //计算一通道组电压高限
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2902] * usRegInputBuf[2900];
    usRegInputBuf[2917]=data_buffer4.data16[1];
    usRegInputBuf[2918]=data_buffer4.data16[0];		
    break;
    
  case 2907://一通道单体电压低限 1.75
    usRegInputBuf[2907]= tempData;
    AT24CXX_WriteLenByte(30,usRegInputBuf[2907],2);
    break;
  case 2908://一通道单体电压低限
    usRegInputBuf[2908]= tempData;
    AT24CXX_WriteLenByte(32,usRegInputBuf[2908],2);
    //取一通道单体电压低限
    data_buffer1.data16[1]=usRegInputBuf[2907];
    data_buffer1.data16[0]=usRegInputBuf[2908];
    //计算一通道组电压低限
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2902] * usRegInputBuf[2900];
    usRegInputBuf[2919]=data_buffer4.data16[1];
    usRegInputBuf[2920]=data_buffer4.data16[0];			
    break;
    
  case 2909://一通道单体温度高限 45
    usRegInputBuf[2909]= tempData;
    AT24CXX_WriteLenByte(34,usRegInputBuf[2909],2);
    break;
  case 2910://一通道单体温度高限
    usRegInputBuf[2910]= tempData;
    AT24CXX_WriteLenByte(36,usRegInputBuf[2910],2);
    break;
    
    
  case 2911://一通道单体温度低限 0
    usRegInputBuf[2911]= tempData;
    AT24CXX_WriteLenByte(38,usRegInputBuf[2911],2);
    break;
  case 2912://一通道单体温度低限
    usRegInputBuf[2912]= tempData;
    AT24CXX_WriteLenByte(40,usRegInputBuf[2912],2);
    break;
    
  case 2913://一通道单体内阻高限 1.5
    usRegInputBuf[2913]= tempData;
    AT24CXX_WriteLenByte(42,usRegInputBuf[2913],2);
    break;
  case 2914://一通道单体内阻高限
    usRegInputBuf[2914]= tempData;
    AT24CXX_WriteLenByte(44,usRegInputBuf[2914],2);
    break;
    
  case 2915://一通道单体内阻低限 0.8
    usRegInputBuf[2915]= tempData;
    AT24CXX_WriteLenByte(46,usRegInputBuf[2915],2);
    break;
  case 2916://一通道单体内阻低限
    usRegInputBuf[2916]= tempData;
    AT24CXX_WriteLenByte(48,usRegInputBuf[2916],2);
    break;
    
  case 2921://一通道单体容量低限 80
    usRegInputBuf[2921]= tempData;
    AT24CXX_WriteLenByte(58,usRegInputBuf[2921],2);
    break;		
  case 2922://一通道单体容量低限
    usRegInputBuf[2922]= tempData;
    AT24CXX_WriteLenByte(60,usRegInputBuf[2922],2);
    break;		
    
    
  case 2925://一通道内阻扫描电流  -0.3   0.3+
    usRegInputBuf[2925]= tempData;
    AT24CXX_WriteLenByte(66,usRegInputBuf[2925],2);
    break;		
  case 2926://一通道内阻扫描电流
    usRegInputBuf[2926]= tempData;
    AT24CXX_WriteLenByte(68,usRegInputBuf[2926],2);
    break;
  case 2927://一通道立即测内阻  不测
    usRegInputBuf[2927]= tempData;
    get_R_T_flag = 1;
    break;
  case 2928://一通道启动/停止测试   启动
    usRegInputBuf[2928]= tempData;
    AT24CXX_WriteLenByte(72,usRegInputBuf[2928],2);
    break;
  case 2923://一通道内阻扫描间隔 168
    usRegInputBuf[2923]= tempData;
    AT24CXX_WriteLenByte(62,usRegInputBuf[2923],2);
    break;	
  case 2924://一通道内阻扫描间隔
    usRegInputBuf[2924]= tempData;
    AT24CXX_WriteLenByte(64,usRegInputBuf[2924],2);
    break;
    
  case 2929://二通道单体电压高限
    usRegInputBuf[2929]= tempData;
    AT24CXX_WriteLenByte(74,usRegInputBuf[2929],2);
    break;
  case 2930://二通道单体电压高限
    usRegInputBuf[2930]= tempData;
    AT24CXX_WriteLenByte(76,usRegInputBuf[2930],2);
    //取二通道单体电压高限
    data_buffer1.data16[1]=usRegInputBuf[2941];
    data_buffer1.data16[0]=usRegInputBuf[2942];
    //计算二通道组电压高限
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2903] * usRegInputBuf[2901];
    usRegInputBuf[2941]=data_buffer4.data16[1];
    usRegInputBuf[2942]=data_buffer4.data16[0];		
    break;
    
  case 2931://二通道单体电压低限
    usRegInputBuf[2931]= tempData;
    AT24CXX_WriteLenByte(78,usRegInputBuf[2931],2);
    break;
  case 2932://一通道单体电压低限
    usRegInputBuf[2932]= tempData;
    AT24CXX_WriteLenByte(80,usRegInputBuf[2932],2);
    //取一通道单体电压低限
    data_buffer1.data16[1]=usRegInputBuf[2943];
    data_buffer1.data16[0]=usRegInputBuf[2944];
    //计算一通道组电压低限
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2903] * usRegInputBuf[2901];
    usRegInputBuf[2943]=data_buffer4.data16[1];
    usRegInputBuf[2944]=data_buffer4.data16[0];			
    break;
    
  case 2933://二通道单体温度高限
    usRegInputBuf[2933]= tempData;
    AT24CXX_WriteLenByte(82,usRegInputBuf[2933],2);
    break;
  case 2934://二通道单体温度高限
    usRegInputBuf[2934]= tempData;
    AT24CXX_WriteLenByte(84,usRegInputBuf[2934],2);
    break;
    
    
  case 2935://二通道单体温度低限
    usRegInputBuf[2935]= tempData;
    AT24CXX_WriteLenByte(86,usRegInputBuf[2935],2);
    break;
  case 2936://二通道单体温度低限
    usRegInputBuf[2936]= tempData;
    AT24CXX_WriteLenByte(88,usRegInputBuf[2936],2);
    break;
    
  case 2937://二通道单体内阻高限
    usRegInputBuf[2937]= tempData;
    AT24CXX_WriteLenByte(90,usRegInputBuf[2937],2);
    break;
  case 2938://二通道单体内阻高限
    usRegInputBuf[2938]= tempData;
    AT24CXX_WriteLenByte(92,usRegInputBuf[2938],2);
    break;
    
  case 2939://二通道单体内阻低限
    usRegInputBuf[2939]= tempData;
    AT24CXX_WriteLenByte(94,usRegInputBuf[2939],2);
    break;
  case 2940://二通道单体内阻低限
    usRegInputBuf[2940]= tempData;
    AT24CXX_WriteLenByte(96,usRegInputBuf[2940],2);
    break;
    
  case 2945://二通道单体容量低限
    usRegInputBuf[2945]= tempData;
    AT24CXX_WriteLenByte(106,usRegInputBuf[2945],2);
    break;		
  case 2946://二通道单体容量低限
    usRegInputBuf[2946]= tempData;
    AT24CXX_WriteLenByte(108,usRegInputBuf[2946],2);
    break;		
    
  case 2947://二通道内阻扫描间隔
    usRegInputBuf[2947]= tempData;
    AT24CXX_WriteLenByte(110,usRegInputBuf[2947],2);
    break;		
  case 2948://二通道内阻扫描间隔
    usRegInputBuf[2948]= tempData;
    AT24CXX_WriteLenByte(112,usRegInputBuf[2948],2);
    break;
    
  case 2949://二通道内阻扫描电流
    usRegInputBuf[2949]= tempData;
    AT24CXX_WriteLenByte(114,usRegInputBuf[2949],2);
    break;		
  case 2950://二通道内阻扫描电流
    usRegInputBuf[2950]= tempData;
    AT24CXX_WriteLenByte(116,usRegInputBuf[2950],2);
    break;
  case 2951://二通道立即测内阻
    //							usRegInputBuf[2353]= tempData;
    get_R_T_flag2 = 1;
    break;
  case 2952://二通道启动/停止测试
    usRegInputBuf[2952]= tempData;
    AT24CXX_WriteLenByte(120,usRegInputBuf[2952],2);
    break;
  case 2961://蜂鸣器
    //usRegInputBuf[2961]= tempData;
    AT24CXX_WriteOneByte(299,0X00);
    break;
  case 2962://蜂鸣器
    usRegInputBuf[2962]= tempData;
    AT24CXX_WriteLenByte(218,usRegInputBuf[2962],2);
    break;
    
  case 2963://1均衡启动停止
    usRegInputBuf[2963]= tempData;
    AT24CXX_WriteLenByte(220,usRegInputBuf[2963],2);
    break;
  case 2964://2均衡启动停止
    usRegInputBuf[2964]= tempData;
    AT24CXX_WriteLenByte(222,usRegInputBuf[2964],2);
    break;  
  case 2965://第一通道均衡电流高限
    usRegInputBuf[2965]= tempData;
    AT24CXX_WriteLenByte(228,usRegInputBuf[2965],2);
    break;
  case 2966://第一通道均衡电流高限
    usRegInputBuf[2966]= tempData;
    AT24CXX_WriteLenByte(230,usRegInputBuf[2966],2);
    break;
  case 2967://第一通道均衡电流低限
    usRegInputBuf[2967]= tempData;
    AT24CXX_WriteLenByte(232,usRegInputBuf[2967],2);
    break;
  case 2968://第一通道均衡电流低限
    usRegInputBuf[2968]= tempData;
    AT24CXX_WriteLenByte(234,usRegInputBuf[2968],2);
    break;	
  case 2969://第二通道均衡电流高限
    usRegInputBuf[2969]= tempData;
    AT24CXX_WriteLenByte(236,usRegInputBuf[2969],2);
    break;
  case 2970://第二通道均衡电流高限
    usRegInputBuf[2970]= tempData;
    AT24CXX_WriteLenByte(238,usRegInputBuf[2970],2);
    break;
  case 2971://第二通道均衡电流低限
    usRegInputBuf[2971]= tempData;
    AT24CXX_WriteLenByte(240,usRegInputBuf[2971],2);
    break;
  case 2972://第二通道均衡电流低限
    usRegInputBuf[2972]= tempData;
    AT24CXX_WriteLenByte(242,usRegInputBuf[2972],2);
    break;   
  case 2973://第一通道浮充电压
    usRegInputBuf[2973]= tempData;
    AT24CXX_WriteLenByte(260,usRegInputBuf[2973],2);
    break;
  case 2974://第一通道浮充电压
    usRegInputBuf[2974]= tempData;
    AT24CXX_WriteLenByte(262,usRegInputBuf[2974],2);
    break;	
  case 2975://第二通道浮充电压
    usRegInputBuf[2975]= tempData;
    AT24CXX_WriteLenByte(264,usRegInputBuf[2975],2);
    break;
  case 2976://第二通道浮充电压
    usRegInputBuf[2976]= tempData;
    AT24CXX_WriteLenByte(266,usRegInputBuf[2976],2);
    break;   
 case 2977:
    Reset_Rdata = 1;
    //AT24CXX_WriteOneByte(299,0X00);
    break;
    
  case 2978://清初始容量
    Start_Rdata1 = 1;
    Get_Rdata = tempData-1-1000;
    break; 
   case 2979://蜂鸣器
    Start_Rdata2 = 1;
    Get_Rdata = tempData-1-2000;
    break;  
   case 2980://蜂鸣器
    
    AT24CXX_WriteOneByte(300+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(301+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(302+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(303+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);   
    break; 
   case 2981://蜂鸣器
    
    AT24CXX_WriteOneByte(300+4*(tempData+127-2000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(301+4*(tempData+127-2000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(302+4*(tempData+127-2000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(303+4*(tempData+127-2000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);   
    break;   
    
  default:
    break;		
  }
  
  return result;
}



unsigned short int setRegisterVal_tcpip(unsigned short int addr,unsigned short int tempData)
{
  unsigned short int result;
  
  return result;
}