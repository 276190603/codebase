#include "stm32f10x.h"
#include "stm32f107.h"
#include "USART3.H"
#include "portarch.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "24cxx.h"
#include "includes.h"
volatile unsigned char USART3_REC_Flag = 0;
unsigned char USART3_buff[USART3_REC_BUFF_SIZE];//���ڽ�������
unsigned int USART3_rec_counter = 0;//����USART3���ռ���
//static void USART3_Delay(uint32_t nCount);
extern unsigned char get_R_T_flag,get_R_I_flag,get_R_T_flag2,get_R_I_flag2,Reset_Rdata;
unsigned char USART_RX_BUF3[30];
unsigned char USART_TX_BUF3[200];

unsigned short int setRegisterVal3(unsigned short int addr,unsigned short int tempData);
unsigned char localAddr = 1;

unsigned short int getRegisterVal3(unsigned short int addr,unsigned short int *tempData);
/******************USART3����****485�ӿ�**********************/



/******USART3(485)���жϽ��պ���******************************************/
void USART3_IRQHandler(void)  
{
  
  unsigned char res;	    
  
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
  {	 
    
    res =USART_ReceiveData(USART3); 	//��ȡ���յ�������
    if(USART3_rec_counter<64)
    {
      USART3_buff[USART3_rec_counter]=res;		//��¼���յ���ֵ
      USART3_rec_counter++;						//������������1 
    } 
  }  
}


/***************USART3(485)���ͺ���*************************************/
//USART3����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void USART3_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
  
  USART3_TX_EN;		//485����ʹ��
  
  for(i = 0;i < length;i ++)
  {			
    
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
    USART_SendData(USART3,send_buff[i]);
  }
  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	 	
  USART3_rec_counter = 0;
  USART3_RX_EN;  		//485����ʹ��
}

/**************USART3(485)�շ���ʱ����**********************************************/


//USART3��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART3_Receive_Data(unsigned char *buf,unsigned char *len)
{
  unsigned char rxlen=USART3_rec_counter;
  unsigned char i=0;
  *len=0;				//Ĭ��Ϊ0
  OSTimeDlyHMSM(0, 0, 0, 10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
  //delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
  if(rxlen==USART3_rec_counter&&rxlen)//���յ�������,�ҽ��������
  {
    for(i=0;i<rxlen;i++)
    {
      buf[i]=USART3_buff[i];	
    }		
    *len=USART3_rec_counter;	//��¼�������ݳ���
    USART3_rec_counter=0;		//����
  }
}
unsigned char	sendCount_USART3;		//�����ֽڸ���

/*==================================================================================*/
// �﷨��ʽ��void Send_Hex3(unsigned char c)
// ��������: ����ʮ������������ֲʱ���Ƕ���Ľӿں�����
// ��ڲ���: unsigned char c      
// ���ڲ���: ��
/*==================================================================================*/
void Send_Hex3(unsigned char c)         
{	
  USART3_Send_Data(&c,1);
}
/*==================================================================================*/
// �﷨��ʽ��unsigned int crc16_USART3(unsigned char *puchMsg, unsigned int usDataLen) 
// ��������: �����ݽ���CRCУ��
// ��ڲ���: unsigned char *puchMsg, unsigned int usDataLen
//													puchMsg���ݻ���������ʼ��ַ;usDataLenУ�����ݵĳ���
// ���ڲ���: unsigned int���͵�У����
/*==================================================================================*/
/* CRC ��λ�ֽ�ֵ�� */ 
const unsigned char auchCRCHi3[] = { 
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
/* CRC��λ�ֽ�ֵ��*/ 
const unsigned char auchCRCLo3[] = { 
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
unsigned short int crc16_USART3(unsigned char *puchMsg, unsigned short int usDataLen) 
{ 
  unsigned char uchCRCHi = 0xFF ; /* ��CRC�ֽڳ�ʼ�� */ 
  unsigned char uchCRCLo = 0xFF ; /* ��CRC �ֽڳ�ʼ�� */ 
  unsigned char uIndex ; /* CRCѭ���е����� */ 
  while (usDataLen--) /* ������Ϣ������ */ 
  { 
    uIndex = uchCRCHi ^ *puchMsg++ ; /* ����CRC */ 
    uchCRCHi = uchCRCLo ^ auchCRCHi3[uIndex] ; 
    uchCRCLo = auchCRCLo3[uIndex] ; 
  } 
  return (uchCRCHi << 8 | uchCRCLo) ; 
}

/*==================================================================================*/
// �﷨��ʽ��void readRegisters(void) 
// ��������: ���Ĵ�����03�����ʵ��
// ��ڲ���: ��
// ���ڲ���: ��
/*==================================================================================*/
void readRegisters3(void)
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int crcData;
  unsigned short int readCount;
  unsigned short int byteCount;
  unsigned short int i;
  unsigned short int tempData = 0;	
  
  addr = (USART_RX_BUF3[2]<<8) + USART_RX_BUF3[3];   //
  //tempAddr = addr & 0xfff;	
  
  tempAddr = addr;
  
  readCount = (USART_RX_BUF3[4]<<8) + USART_RX_BUF3[5];	//Ҫ���ĸ���
  //readCount = receBuf[5];
  
  byteCount = readCount * 2;
  
  for(i=0;i<byteCount;i+=2,tempAddr++)
  {
    getRegisterVal3(tempAddr,&tempData);				
    USART_TX_BUF3[i+3] = tempData >> 8;				   		
    USART_TX_BUF3[i+4] = tempData & 0xff;			
  }
  
  USART_TX_BUF3[0] = localAddr;
  USART_TX_BUF3[1] = 3;
  USART_TX_BUF3[2] = byteCount;
  byteCount += 3;
  crcData = crc16_USART3(USART_TX_BUF3,byteCount);
  USART_TX_BUF3[byteCount] = crcData >> 8;
  byteCount++;
  USART_TX_BUF3[byteCount] = crcData & 0xff;
  
  sendCount_USART3 = byteCount + 1;
  for(i=0;i<sendCount_USART3;i++)
  {
    Send_Hex3(USART_TX_BUF3[i]);
  }
}

void readRegisters04_USART3(void)
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int crcData;
  unsigned short int readCount;
  unsigned short int byteCount;
  unsigned short int i;
  unsigned short int tempData = 0;	
  
  addr = (USART_RX_BUF3[2]<<8) + USART_RX_BUF3[3];   //
  //tempAddr = addr & 0xfff;	
  
  tempAddr = addr;
  
  readCount = (USART_RX_BUF3[4]<<8) + USART_RX_BUF3[5];	//Ҫ���ĸ���
  //readCount = receBuf[5];
  
  byteCount = readCount * 2;
  
  for(i=0;i<byteCount;i+=2,tempAddr++)
  {
    //          if((tempAddr<36)||(tempAddr>=416)&&(tempAddr<452)||(tempAddr>=832)&&(tempAddr<868)||
    //             (tempAddr>=1248)&&(tempAddr<1284)||(tempAddr>=1664)&&(tempAddr<1700)||(tempAddr>=2080)&&(tempAddr<2098))
    //          {
    getRegisterVal3(tempAddr,&tempData);				
    USART_TX_BUF3[i+3] = tempData >> 8;				   		
    USART_TX_BUF3[i+4] = tempData & 0xff;		
    //          }
  }
  
  USART_TX_BUF3[0] = localAddr;
  USART_TX_BUF3[1] = 4;
  USART_TX_BUF3[2] = byteCount;
  byteCount += 3;
  crcData = crc16_USART3(USART_TX_BUF3,byteCount);
  USART_TX_BUF3[byteCount] = crcData >> 8;
  byteCount++;
  USART_TX_BUF3[byteCount] = crcData & 0xff;
  
  sendCount_USART3 = byteCount + 1;
  for(i=0;i<sendCount_USART3;i++)
  {
    Send_Hex3(USART_TX_BUF3[i]);
  }
}

/*==================================================================================*/
// �﷨��ʽ��void presetSingleRegister()
// ��������: ���õ����Ĵ�����06�����ʵ��
// ��ڲ���: ��
// ���ڲ���: ��
/*==================================================================================*/
void presetSingleRegister3()
{
  unsigned short int addr;
  unsigned short int tempAddr;
  //	unsigned char byteCount;
  //	unsigned char setCount;
  unsigned short int crcData;
  unsigned short int tempData;
  unsigned short int i;
  addr = (USART_RX_BUF3[2]<<8) | USART_RX_BUF3[3];
  tempAddr = addr & 0xfff;
  tempData = (USART_RX_BUF3[4]<<8) | USART_RX_BUF3[5];
  
  setRegisterVal3(tempAddr,tempData);			
  
  USART_TX_BUF3[0] = localAddr;
  USART_TX_BUF3[1] = 6;
  USART_TX_BUF3[2] = addr >> 8;
  USART_TX_BUF3[3] = addr & 0xff;
  USART_TX_BUF3[4] = tempData>>8;
  USART_TX_BUF3[5] = tempData&0xff;
  crcData = crc16_USART3(USART_TX_BUF3,6);
  USART_TX_BUF3[6] = crcData >> 8;
  USART_TX_BUF3[7] = crcData & 0xff;
  sendCount_USART3 = 8;
  for(i=0;i<sendCount_USART3;i++)
  {
    Send_Hex3(USART_TX_BUF3[i]);
  }	
}


/*==================================================================================*/
// �﷨��ʽ��void presetSingleRegister()
// ��������: ���ö���Ĵ�����16�����ʵ��
// ��ڲ���: ��
// ���ڲ���: ��
/*==================================================================================*/
void presetMultipleRegisters3(void)
{
  unsigned short int addr;
  unsigned short int tempAddr;
  unsigned short int byteCount;
  unsigned short int setCount;
  unsigned short int crcData;
  unsigned short int tempData;
  
  unsigned short int i;
  
  //	addr = USART_RX_BUF3[3];
  //	tempAddr = addr & 0xff;
  addr = (USART_RX_BUF3[2]<<8) + USART_RX_BUF3[3];
  tempAddr = addr;
  
  setCount = USART_RX_BUF3[5];
  byteCount = USART_RX_BUF3[6];	
  
  for(i=0;i<setCount;i++,tempAddr++)
  {
    tempData = (USART_RX_BUF3[i*2+7]<<8) + USART_RX_BUF3[i*2+8];
    
    setRegisterVal3(tempAddr,tempData);			
  }
  
  USART_TX_BUF3[0] = localAddr;
  USART_TX_BUF3[1] = 16;
  USART_TX_BUF3[2] = addr >> 8;
  USART_TX_BUF3[3] = addr & 0xff;
  USART_TX_BUF3[4] = setCount >> 8;
  USART_TX_BUF3[5] = setCount & 0xff;
  crcData = crc16_USART3(USART_TX_BUF3,6);
  USART_TX_BUF3[6] = crcData >> 8;
  USART_TX_BUF3[7] = crcData & 0xff;
  sendCount_USART3 = 8;
  for(i=0;i<sendCount_USART3;i++)
  {
    Send_Hex3(USART_TX_BUF3[i]);
  }	
}
//���uart0����
void checkComm3Modbus(void)
{
  unsigned short int crcData;
  unsigned short int tempData;
  unsigned char len= 30;
  USART3_Receive_Data(USART_RX_BUF3,&len);
  if(len>7)
  {
    switch(USART_RX_BUF3[1])
    {
      
    case 3:				      //��ȡ���ּĴ���(һ������)
      
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF3[0]==localAddr)
        {
          crcData = crc16_USART3(USART_RX_BUF3,6);	
          if(crcData == USART_RX_BUF3[7]+(USART_RX_BUF3[6]<<8))
          {
            readRegisters3();
            
          }
        }
      }						
      break;
    case 4:				      //��ȡ���ּĴ���(һ������)
      
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF3[0]==localAddr)
        {
          crcData = crc16_USART3(USART_RX_BUF3,6);	
          if(crcData == USART_RX_BUF3[7]+(USART_RX_BUF3[6]<<8))
          {
            readRegisters04_USART3();
            
          }
        }
        
      }						
      break;
    case 6:                   //���õ����Ĵ���
      if(len > 7)
      {						
        len=0;
        if(USART_RX_BUF3[0]==localAddr)
        {
          crcData = crc16_USART3(USART_RX_BUF3,6);	
          if(crcData == USART_RX_BUF3[7]+(USART_RX_BUF3[6]<<8))
          {
            presetSingleRegister3();
          }
        }
      }				
      break;
    case 16://���ö���Ĵ���
      if(USART_RX_BUF3[0]==localAddr)
      {
        if(len>7)
        {
          tempData = (USART_RX_BUF3[4]<<8) + USART_RX_BUF3[5];
          tempData = tempData * 2;	//���ݸ���
          tempData += 9;
          
          if(len== tempData)
          {
            crcData = crc16_USART3(USART_RX_BUF3,len-2);
            if(crcData == (USART_RX_BUF3[len-2]<<8)+ USART_RX_BUF3[len-1])
            {
              presetMultipleRegisters3();
              len=0;
            }
          }
        }					
      }			
      
      break;	
      
      
      
    default:
      break;			
    }
  }
  
  
  
}


//ȡ�Ĵ���ֵ ����0��ʾ�ɹ�
unsigned short int getRegisterVal3(unsigned short int addr,unsigned short int *tempData)
{
  unsigned short int result = 0;
  unsigned short int tempAddr;
  
  tempAddr = addr & 0xfff;
  tempAddr = addr & 0xfff;
  *tempData = usRegInputBuf[tempAddr & 0xfff];
  return result;
}
//���üĴ���ֵ ����0��ʾ�ɹ�
unsigned short int setRegisterVal3(unsigned short int addr,unsigned short int tempData)
{
  
  unsigned short int result = 0;
  unsigned short int tempAddr;
  union float_to_u16 data_buffer1;
  union float_to_u16 data_buffer4;
  
  tempAddr = addr & 0xfff;
  switch(tempAddr & 0xfff)
  {
  
  case 2900://һͨ�������� 6
    usRegInputBuf[2900]= tempData;
    AT24CXX_WriteLenByte(0,usRegInputBuf[2900],2);
    break;
  case 2901://��ͨ�������� 6
    usRegInputBuf[2901]= tempData;
    AT24CXX_WriteLenByte(2,usRegInputBuf[2901],2);
    break;    
  case 2902://һͨ��ֻ�� 0
    usRegInputBuf[2902]= tempData;
    AT24CXX_WriteLenByte(8,usRegInputBuf[2902],2);
    break;
  case 2903://��ͨ��ֻ�� 0
    usRegInputBuf[2903]= tempData;
    AT24CXX_WriteLenByte(10,usRegInputBuf[2903],2);
    break;
    
    //	
    //		
    
    //  case 2306://�豸ID 
    //    usRegInputBuf[2306]= tempData;
    //    AT24CXX_WriteLenByte(24,usRegInputBuf[2306],2);
    //    addard = usRegInputBuf[2306];
    //    break;
    
  case 2905://һͨ�������ѹ���� 2.5
    usRegInputBuf[2905]= tempData;
    AT24CXX_WriteLenByte(26,usRegInputBuf[2905],2);
    break;
  case 2906://һͨ�������ѹ����
    usRegInputBuf[2906]= tempData;
    AT24CXX_WriteLenByte(28,usRegInputBuf[2906],2);
    //ȡһͨ�������ѹ����
    data_buffer1.data16[1]=usRegInputBuf[2905];
    data_buffer1.data16[0]=usRegInputBuf[2906];
    //����һͨ�����ѹ����
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2902] * usRegInputBuf[2900];
    usRegInputBuf[2917]=data_buffer4.data16[1];
    usRegInputBuf[2918]=data_buffer4.data16[0];		
    break;
    
  case 2907://һͨ�������ѹ���� 1.75
    usRegInputBuf[2907]= tempData;
    AT24CXX_WriteLenByte(30,usRegInputBuf[2907],2);
    break;
  case 2908://һͨ�������ѹ����
    usRegInputBuf[2908]= tempData;
    AT24CXX_WriteLenByte(32,usRegInputBuf[2908],2);
    //ȡһͨ�������ѹ����
    data_buffer1.data16[1]=usRegInputBuf[2907];
    data_buffer1.data16[0]=usRegInputBuf[2908];
    //����һͨ�����ѹ����
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2902] * usRegInputBuf[2900];
    usRegInputBuf[2919]=data_buffer4.data16[1];
    usRegInputBuf[2920]=data_buffer4.data16[0];			
    break;
    
  case 2909://һͨ�������¶ȸ��� 45
    usRegInputBuf[2909]= tempData;
    AT24CXX_WriteLenByte(34,usRegInputBuf[2909],2);
    break;
  case 2910://һͨ�������¶ȸ���
    usRegInputBuf[2910]= tempData;
    AT24CXX_WriteLenByte(36,usRegInputBuf[2910],2);
    break;
    
    
  case 2911://һͨ�������¶ȵ��� 0
    usRegInputBuf[2911]= tempData;
    AT24CXX_WriteLenByte(38,usRegInputBuf[2911],2);
    break;
  case 2912://һͨ�������¶ȵ���
    usRegInputBuf[2912]= tempData;
    AT24CXX_WriteLenByte(40,usRegInputBuf[2912],2);
    break;
    
  case 2913://һͨ������������� 1.5
    usRegInputBuf[2913]= tempData;
    AT24CXX_WriteLenByte(42,usRegInputBuf[2913],2);
    break;
  case 2914://һͨ�������������
    usRegInputBuf[2914]= tempData;
    AT24CXX_WriteLenByte(44,usRegInputBuf[2914],2);
    break;
    
  case 2915://һͨ������������� 0.8
    usRegInputBuf[2915]= tempData;
    AT24CXX_WriteLenByte(46,usRegInputBuf[2915],2);
    break;
  case 2916://һͨ�������������
    usRegInputBuf[2916]= tempData;
    AT24CXX_WriteLenByte(48,usRegInputBuf[2916],2);
    break;
    
  case 2921://һͨ�������������� 80
    usRegInputBuf[2921]= tempData;
    AT24CXX_WriteLenByte(58,usRegInputBuf[2921],2);
    break;		
  case 2922://һͨ��������������
    usRegInputBuf[2922]= tempData;
    AT24CXX_WriteLenByte(60,usRegInputBuf[2922],2);
    break;		
    
    
  case 2925://һͨ������ɨ�����  -0.3   0.3+
    usRegInputBuf[2925]= tempData;
    AT24CXX_WriteLenByte(66,usRegInputBuf[2925],2);
    break;		
  case 2926://һͨ������ɨ�����
    usRegInputBuf[2926]= tempData;
    AT24CXX_WriteLenByte(68,usRegInputBuf[2926],2);
    break;
  case 2927://һͨ������������  ����
    usRegInputBuf[2927]= tempData;
    get_R_T_flag = 1;
    break;
  case 2928://һͨ������/ֹͣ����   ����
    usRegInputBuf[2928]= tempData;
    AT24CXX_WriteLenByte(72,usRegInputBuf[2928],2);
    break;
  case 2923://һͨ������ɨ���� 168
    usRegInputBuf[2923]= tempData;
    AT24CXX_WriteLenByte(62,usRegInputBuf[2923],2);
    break;	
  case 2924://һͨ������ɨ����
    usRegInputBuf[2924]= tempData;
    AT24CXX_WriteLenByte(64,usRegInputBuf[2924],2);
    break;
    
  case 2929://��ͨ�������ѹ����
    usRegInputBuf[2929]= tempData;
    AT24CXX_WriteLenByte(74,usRegInputBuf[2929],2);
    break;
  case 2930://��ͨ�������ѹ����
    usRegInputBuf[2930]= tempData;
    AT24CXX_WriteLenByte(76,usRegInputBuf[2930],2);
    //ȡ��ͨ�������ѹ����
    data_buffer1.data16[1]=usRegInputBuf[2941];
    data_buffer1.data16[0]=usRegInputBuf[2942];
    //�����ͨ�����ѹ����
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2903] * usRegInputBuf[2901];
    usRegInputBuf[2941]=data_buffer4.data16[1];
    usRegInputBuf[2942]=data_buffer4.data16[0];		
    break;
    
  case 2931://��ͨ�������ѹ����
    usRegInputBuf[2931]= tempData;
    AT24CXX_WriteLenByte(78,usRegInputBuf[2931],2);
    break;
  case 2932://һͨ�������ѹ����
    usRegInputBuf[2932]= tempData;
    AT24CXX_WriteLenByte(80,usRegInputBuf[2932],2);
    //ȡһͨ�������ѹ����
    data_buffer1.data16[1]=usRegInputBuf[2943];
    data_buffer1.data16[0]=usRegInputBuf[2944];
    //����һͨ�����ѹ����
    data_buffer4.data32 = data_buffer1.data32 * usRegInputBuf[2903] * usRegInputBuf[2901];
    usRegInputBuf[2943]=data_buffer4.data16[1];
    usRegInputBuf[2944]=data_buffer4.data16[0];			
    break;
    
  case 2933://��ͨ�������¶ȸ���
    usRegInputBuf[2933]= tempData;
    AT24CXX_WriteLenByte(82,usRegInputBuf[2933],2);
    break;
  case 2934://��ͨ�������¶ȸ���
    usRegInputBuf[2934]= tempData;
    AT24CXX_WriteLenByte(84,usRegInputBuf[2934],2);
    break;
    
    
  case 2935://��ͨ�������¶ȵ���
    usRegInputBuf[2935]= tempData;
    AT24CXX_WriteLenByte(86,usRegInputBuf[2935],2);
    break;
  case 2936://��ͨ�������¶ȵ���
    usRegInputBuf[2936]= tempData;
    AT24CXX_WriteLenByte(88,usRegInputBuf[2936],2);
    break;
    
  case 2937://��ͨ�������������
    usRegInputBuf[2937]= tempData;
    AT24CXX_WriteLenByte(90,usRegInputBuf[2937],2);
    break;
  case 2938://��ͨ�������������
    usRegInputBuf[2938]= tempData;
    AT24CXX_WriteLenByte(92,usRegInputBuf[2938],2);
    break;
    
  case 2939://��ͨ�������������
    usRegInputBuf[2939]= tempData;
    AT24CXX_WriteLenByte(94,usRegInputBuf[2939],2);
    break;
  case 2940://��ͨ�������������
    usRegInputBuf[2940]= tempData;
    AT24CXX_WriteLenByte(96,usRegInputBuf[2940],2);
    break;
    
  case 2945://��ͨ��������������
    usRegInputBuf[2945]= tempData;
    AT24CXX_WriteLenByte(106,usRegInputBuf[2945],2);
    break;		
  case 2946://��ͨ��������������
    usRegInputBuf[2946]= tempData;
    AT24CXX_WriteLenByte(108,usRegInputBuf[2946],2);
    break;		
    
  case 2947://��ͨ������ɨ����
    usRegInputBuf[2947]= tempData;
    AT24CXX_WriteLenByte(110,usRegInputBuf[2947],2);
    break;		
  case 2948://��ͨ������ɨ����
    usRegInputBuf[2948]= tempData;
    AT24CXX_WriteLenByte(112,usRegInputBuf[2948],2);
    break;
    
  case 2949://��ͨ������ɨ�����
    usRegInputBuf[2949]= tempData;
    AT24CXX_WriteLenByte(114,usRegInputBuf[2949],2);
    break;		
  case 2950://��ͨ������ɨ�����
    usRegInputBuf[2950]= tempData;
    AT24CXX_WriteLenByte(116,usRegInputBuf[2950],2);
    break;
  case 2951://��ͨ������������
    //							usRegInputBuf[2353]= tempData;
    get_R_T_flag2 = 1;
    break;
  case 2952://��ͨ������/ֹͣ����
    usRegInputBuf[2952]= tempData;
    AT24CXX_WriteLenByte(120,usRegInputBuf[2952],2);
    break;
  case 2961://������
    //usRegInputBuf[2961]= tempData;
    AT24CXX_WriteOneByte(299,0X00);
    break;
  case 2962://������
    usRegInputBuf[2962]= tempData;
    AT24CXX_WriteLenByte(218,usRegInputBuf[2962],2);
    break;
    
  case 2963://1��������ֹͣ
    usRegInputBuf[2963]= tempData;
    AT24CXX_WriteLenByte(220,usRegInputBuf[2963],2);
    break;
  case 2964://2��������ֹͣ
    usRegInputBuf[2964]= tempData;
    AT24CXX_WriteLenByte(222,usRegInputBuf[2964],2);
    break;  
  case 2965://��һͨ�������������
    usRegInputBuf[2965]= tempData;
    AT24CXX_WriteLenByte(228,usRegInputBuf[2965],2);
    break;
  case 2966://��һͨ�������������
    usRegInputBuf[2966]= tempData;
    AT24CXX_WriteLenByte(230,usRegInputBuf[2966],2);
    break;
  case 2967://��һͨ�������������
    usRegInputBuf[2967]= tempData;
    AT24CXX_WriteLenByte(232,usRegInputBuf[2967],2);
    break;
  case 2968://��һͨ�������������
    usRegInputBuf[2968]= tempData;
    AT24CXX_WriteLenByte(234,usRegInputBuf[2968],2);
    break;	
  case 2969://�ڶ�ͨ�������������
    usRegInputBuf[2969]= tempData;
    AT24CXX_WriteLenByte(236,usRegInputBuf[2969],2);
    break;
  case 2970://�ڶ�ͨ�������������
    usRegInputBuf[2970]= tempData;
    AT24CXX_WriteLenByte(238,usRegInputBuf[2970],2);
    break;
  case 2971://�ڶ�ͨ�������������
    usRegInputBuf[2971]= tempData;
    AT24CXX_WriteLenByte(240,usRegInputBuf[2971],2);
    break;
  case 2972://�ڶ�ͨ�������������
    usRegInputBuf[2972]= tempData;
    AT24CXX_WriteLenByte(242,usRegInputBuf[2972],2);
    break;   
  case 2973://��һͨ�������ѹ
    usRegInputBuf[2973]= tempData;
    AT24CXX_WriteLenByte(260,usRegInputBuf[2973],2);
    break;
  case 2974://��һͨ�������ѹ
    usRegInputBuf[2974]= tempData;
    AT24CXX_WriteLenByte(262,usRegInputBuf[2974],2);
    break;	
  case 2975://�ڶ�ͨ�������ѹ
    usRegInputBuf[2975]= tempData;
    AT24CXX_WriteLenByte(264,usRegInputBuf[2975],2);
    break;
  case 2976://�ڶ�ͨ�������ѹ
    usRegInputBuf[2976]= tempData;
    AT24CXX_WriteLenByte(266,usRegInputBuf[2976],2);
    break;   
 case 2977:
    Reset_Rdata = 1;
    //AT24CXX_WriteOneByte(299,0X00);
    break;
    
  case 2978://���ʼ����
    Start_Rdata1 = 1;
    Get_Rdata = tempData-1-1000;
    break; 
   case 2979://������
    Start_Rdata2 = 1;
    Get_Rdata = tempData-1-2000;
    break;  
   case 2980://������
    
    AT24CXX_WriteOneByte(300+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(301+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(302+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(303+4*(tempData-1-1000),0);
    OSTimeDlyHMSM(0, 0, 0, 1);   
    break; 
   case 2981://������
    
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



