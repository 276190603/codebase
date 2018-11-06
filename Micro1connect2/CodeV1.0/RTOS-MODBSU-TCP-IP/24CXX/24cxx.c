#include "24cxx.h" 
#include "portarch.h"
#include "includes.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//24CXX���� ����(�ʺ�24C01~24C16,24C32~256δ��������!�д���֤!)		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//static void I2C_delay(uint16_t cnt)
//{
//	while(cnt--);
//}
void delay_us(unsigned int nus)
{		
  nus = nus*2;
  while(nus--);
  //	unsigned int ticks;
  //	unsigned int told,tnow,tcnt=0;
  //	unsigned int reload=SysTick->LOAD;	//LOAD��ֵ	    	 
  //	ticks=nus*fac_us; 			//��Ҫ�Ľ�����	  		 
  //	tcnt=0;
  //	told=SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
  //	while(1)
  //	{
  //		tnow=SysTick->VAL;	
  //		if(tnow!=told)
  //		{	    
  //			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
  //			else tcnt+=reload-tnow+told;	    
  //			told=tnow;
  //			if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
  //		}  
  //	}; 									    
}
//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(unsigned short nms)
{	
  //	if(OSRunning==TRUE)//���os�Ѿ�������	    
  //	{		  
  //		if(nms>=fac_ms)//��ʱ��ʱ�����ucos������ʱ������ 
  //		{
  //   			OSTimeDly(nms/fac_ms);//ucos��ʱ
  //		}
  //		nms%=fac_ms;				//ucos�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
  //	}
  delay_us((unsigned int)(nms*1000));	//��ͨ��ʽ��ʱ,��ʱucos�޷���������.
}

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
  IIC_Init();
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
unsigned char AT24CXX_ReadOneByte(unsigned short ReadAddr)
{				  
  unsigned char temp=0;		  	    																 
  IIC_Start();  
  if(EE_TYPE>AT24C16)
  {
    IIC_Send_Byte(0XA0);	   //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
    //IIC_Wait_Ack();		 
  }else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 
  
  IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
  IIC_Wait_Ack();	    
  IIC_Start();  	 	   
  IIC_Send_Byte(0XA1);           //�������ģʽ			   
  IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//����һ��ֹͣ����	    
  return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(unsigned short WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
  IIC_Start();  
  if(EE_TYPE>AT24C16)
  {
    IIC_Send_Byte(0XA0);	    //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
  }else
  {
    IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
  }	 
  IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
  IIC_Wait_Ack(); 	 										  		   
  IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
  IIC_Wait_Ack();  		    	   
  IIC_Stop();//����һ��ֹͣ���� 
  delay_ms(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(unsigned short WriteAddr,unsigned int DataToWrite,unsigned char Len)
{  	
  unsigned char t;
  for(t=0;t<Len;t++)
  {
    AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    //OSTimeDlyHMSM(0, 0, 0, 1);
  }												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
unsigned int AT24CXX_ReadLenByte(unsigned short ReadAddr,unsigned char Len)
{  	
  unsigned char t;
  unsigned int temp=0;
  for(t=0;t<Len;t++)
  {
    temp<<=8;
    temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 
    //OSTimeDlyHMSM(0, 0, 0, 1);
  }
  return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
unsigned char AT24CXX_Check(void)
{
  unsigned char temp;
  //AT24CXX_WriteOneByte(299,0x00);
  temp=AT24CXX_ReadOneByte(299);//����ÿ�ο�����дAT24CXX			   
  if(temp==0X55)return 0;		   
  else//�ų���һ�γ�ʼ�������
  {
    AT24CXX_Reset_Alldata();
    AT24CXX_WriteOneByte(299,0X55);
    OSTimeDlyHMSM(0, 0, 0, 1);
    temp=AT24CXX_ReadOneByte(299);	
    OSTimeDlyHMSM(0, 0, 0, 1);
    if(temp==0X55)return 0;
  }
  return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(unsigned short ReadAddr,unsigned char *pBuffer,unsigned short NumToRead)
{
  while(NumToRead)
  {
    *pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
    NumToRead--;
  }
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short NumToWrite)
{
  while(NumToWrite--)
  {
    AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
    WriteAddr++;
    pBuffer++;
  }
}

//��EEPROM���������
void AT24CXX_Read_Setdata(void)
{
  //usRegInputBuf[2422] = 1;
  u16 i;
  
  usRegInputBuf[2900]=AT24CXX_ReadLenByte(0,2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2901]=AT24CXX_ReadLenByte(2,2);   
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2902]=AT24CXX_ReadLenByte(8,2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2903]=AT24CXX_ReadLenByte(10,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2905]=AT24CXX_ReadLenByte(26,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2906]=AT24CXX_ReadLenByte(28,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2907]=AT24CXX_ReadLenByte(30,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2908]=AT24CXX_ReadLenByte(32,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2909]=AT24CXX_ReadLenByte(34,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2910]=AT24CXX_ReadLenByte(36,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2911]=AT24CXX_ReadLenByte(38,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2912]=AT24CXX_ReadLenByte(40,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2913]=AT24CXX_ReadLenByte(42,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2914]=AT24CXX_ReadLenByte(44,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2915]=AT24CXX_ReadLenByte(46,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2916]=AT24CXX_ReadLenByte(48,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2921]=AT24CXX_ReadLenByte(58,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2922]=AT24CXX_ReadLenByte(60,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2923]=AT24CXX_ReadLenByte(62,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2924]=AT24CXX_ReadLenByte(64,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2925]=AT24CXX_ReadLenByte(66,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2926]=AT24CXX_ReadLenByte(59,2);		
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2927]= 0;//һͨ������������  ����
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2928]=AT24CXX_ReadLenByte(72,2);	//һͨ������/ֹͣ����   ֹͣ		
  OSTimeDlyHMSM(0, 0, 0, 1);
  
  usRegInputBuf[2929]=AT24CXX_ReadLenByte(74,2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2930]=AT24CXX_ReadLenByte(76,2);   
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2931]=AT24CXX_ReadLenByte(78,2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2932]=AT24CXX_ReadLenByte(80,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2933]=AT24CXX_ReadLenByte(82,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2934]=AT24CXX_ReadLenByte(84,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2935]=AT24CXX_ReadLenByte(86,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2936]=AT24CXX_ReadLenByte(88,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2937]=AT24CXX_ReadLenByte(90,2); 
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2938]=AT24CXX_ReadLenByte(92,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2939]=AT24CXX_ReadLenByte(94,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2940]=AT24CXX_ReadLenByte(96,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2945]=AT24CXX_ReadLenByte(106,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2946]=AT24CXX_ReadLenByte(108,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2947]=AT24CXX_ReadLenByte(110,2);		
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2948]=AT24CXX_ReadLenByte(112,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2949]=AT24CXX_ReadLenByte(114,2);		
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2950]=AT24CXX_ReadLenByte(116,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2952]=AT24CXX_ReadLenByte(120,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  
  usRegInputBuf[2951]= 0;//��ͨ������������  ����
  
  usRegInputBuf[2962]=AT24CXX_ReadLenByte(218,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  
  
  
  usRegInputBuf[2963]= 0;//һͨ������ֹͣ/����
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2964]= 0;//��ͨ������ֹͣ/����
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2965]=AT24CXX_ReadLenByte(228,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2966]=AT24CXX_ReadLenByte(230,2);	

  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2967]=AT24CXX_ReadLenByte(232,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2968]=AT24CXX_ReadLenByte(234,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2969]=AT24CXX_ReadLenByte(236,2);		
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2970]=AT24CXX_ReadLenByte(238,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2971]=AT24CXX_ReadLenByte(240,2);		
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2972]=AT24CXX_ReadLenByte(242,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2973]=AT24CXX_ReadLenByte(260,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2974]=AT24CXX_ReadLenByte(262,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2975]=AT24CXX_ReadLenByte(264,2);		
  OSTimeDlyHMSM(0, 0, 0, 1);
  usRegInputBuf[2976]=AT24CXX_ReadLenByte(266,2);	
  OSTimeDlyHMSM(0, 0, 0, 1);
  
  for(i=0;i<256;i++)
  {
     usRegInputBuf[2*i+2048]=AT24CXX_ReadLenByte(300+4*i,2);
     OSTimeDlyHMSM(0, 0, 0, 1);
     usRegInputBuf[2*i+1+2048]=AT24CXX_ReadLenByte(302+4*i,2);		
     OSTimeDlyHMSM(0, 0, 0, 1);    
  }   


}

//�ָ�����Ĭ������
void AT24CXX_Reset_Alldata(void)
{
  union float_to_u16 test_data;	
  u16 i;
  //    for(i=0;i<2000;i++)
  //    {
  //      //if(i<230||i>233)
  //      AT24CXX_WriteOneByte(i,0x00);
  //      OSTimeDlyHMSM(0, 0, 0, 1);
  //    }
  
  //usRegInputBuf[2294] = 6;	//һͨ�������� 6	
  usRegInputBuf[2900] = 6;	//һͨ�������� 6		 
      AT24CXX_WriteLenByte(0,usRegInputBuf[2900],2);
  usRegInputBuf[2901] = 6;	//��ͨ�������� 6		
      AT24CXX_WriteLenByte(2,usRegInputBuf[2901],2);
  usRegInputBuf[2902] = 0;	//һͨ��ֻ�� 0
      AT24CXX_WriteLenByte(8,usRegInputBuf[2902],2);
  usRegInputBuf[2903] = 0;	//��ͨ��ֻ�� 0
  AT24CXX_WriteLenByte(10,usRegInputBuf[2903],2);
//  usRegInputBuf[2300] = 0;	//һͨ��ֻ�� 0   
//  usRegInputBuf[2306] = 64;	//һͨ��ֻ�� 0
  
  
  test_data.data32 = 2.5;		//һͨ�������ѹ���� 2.5	
  usRegInputBuf[2905] = test_data.data16[1];
  AT24CXX_WriteLenByte(26,usRegInputBuf[2905],2);
  usRegInputBuf[2906] = test_data.data16[0];
 
  AT24CXX_WriteLenByte(28,usRegInputBuf[2906],2);
 
  test_data.data32 = 1.75;	//һͨ�������ѹ���� 1.75	
  usRegInputBuf[2907] = test_data.data16[1];

  AT24CXX_WriteLenByte(30,usRegInputBuf[2907],2);
  usRegInputBuf[2908] = test_data.data16[0];
   AT24CXX_WriteLenByte(32,usRegInputBuf[2908],2);
  test_data.data32 = 45;	//һͨ�������¶ȸ��� 45
  usRegInputBuf[2909] = test_data.data16[1];
  AT24CXX_WriteLenByte(34,usRegInputBuf[2909],2);
  usRegInputBuf[2910] = test_data.data16[0];
   AT24CXX_WriteLenByte(36,usRegInputBuf[2910],2);
  test_data.data32 = 0;	//һͨ�������¶ȵ��� 0
  usRegInputBuf[2911] = test_data.data16[1];
   AT24CXX_WriteLenByte(38,usRegInputBuf[2911],2);
  usRegInputBuf[2912] = test_data.data16[0];
   AT24CXX_WriteLenByte(40,usRegInputBuf[2912],2);
  test_data.data32 = 1.5;	//һͨ������������� 1.5
  usRegInputBuf[2913] = test_data.data16[1];
  AT24CXX_WriteLenByte(42,usRegInputBuf[2913],2);
  usRegInputBuf[2914] = test_data.data16[0];	
   AT24CXX_WriteLenByte(44,usRegInputBuf[2914],2);
  test_data.data32 = 0.8;	//һͨ������������� 0.8
  usRegInputBuf[2915] = test_data.data16[1];
  AT24CXX_WriteLenByte(46,usRegInputBuf[2915],2);
  usRegInputBuf[2916] = test_data.data16[0];	
  AT24CXX_WriteLenByte(48,usRegInputBuf[2916],2);
  
  test_data.data32 = 80;	//һͨ�������������� 80
  usRegInputBuf[2921] = test_data.data16[1];
  AT24CXX_WriteLenByte(58,usRegInputBuf[2921],2);
  usRegInputBuf[2922] = test_data.data16[0];	
  AT24CXX_WriteLenByte(60,usRegInputBuf[2922],2);
  test_data.data32 = 168;	//һͨ������ɨ���� 168
  usRegInputBuf[2923] = test_data.data16[1];
  AT24CXX_WriteLenByte(62,usRegInputBuf[2923],2);
  usRegInputBuf[2924] = test_data.data16[0];	
  AT24CXX_WriteLenByte(64,usRegInputBuf[2924],2);
  test_data.data32 = 0.3;	//һͨ������ɨ�����  -0.3   0.3+
  usRegInputBuf[2925] = test_data.data16[1];
   AT24CXX_WriteLenByte(66,usRegInputBuf[2925],2);
  usRegInputBuf[2926] = test_data.data16[0];	
   AT24CXX_WriteLenByte(68,usRegInputBuf[2926],2);
  usRegInputBuf[2927]= 0;//һͨ������������  ����
  usRegInputBuf[2928]= 0;//һͨ������/ֹͣ����   ����
  AT24CXX_WriteLenByte(72,usRegInputBuf[2928],2);
  //usRegInputBuf[2422] = 1;
  
  test_data.data32 = 2.5;	//��ͨ�������ѹ���� 2.5
  usRegInputBuf[2929] = test_data.data16[1];
    AT24CXX_WriteLenByte(74,usRegInputBuf[2929],2);
  usRegInputBuf[2930] = test_data.data16[0];		
  AT24CXX_WriteLenByte(76,usRegInputBuf[2930],2);
  
  test_data.data32 = 1.75;	//��ͨ�������ѹ���� 1.75
  usRegInputBuf[2931] = test_data.data16[1];
  AT24CXX_WriteLenByte(78,usRegInputBuf[2931],2);
  usRegInputBuf[2932] = test_data.data16[0];		
  AT24CXX_WriteLenByte(80,usRegInputBuf[2932],2);
  test_data.data32 = 45;	//��ͨ�������¶ȸ��� 40
  usRegInputBuf[2933] = test_data.data16[1];
  AT24CXX_WriteLenByte(82,usRegInputBuf[2933],2);
  usRegInputBuf[2934] = test_data.data16[0];		
  AT24CXX_WriteLenByte(84,usRegInputBuf[2934],2);
  test_data.data32 = 0;	//��ͨ�������¶ȵ��� 0
  usRegInputBuf[2935] = test_data.data16[1];
      AT24CXX_WriteLenByte(86,usRegInputBuf[2935],2);
  usRegInputBuf[2936] = test_data.data16[0];	
   AT24CXX_WriteLenByte(88,usRegInputBuf[2936],2);
  test_data.data32 = 1.5;	//��ͨ������������� 1.5
  usRegInputBuf[2937] = test_data.data16[1];
   AT24CXX_WriteLenByte(90,usRegInputBuf[2937],2);
  usRegInputBuf[2938] = test_data.data16[0];		
    AT24CXX_WriteLenByte(92,usRegInputBuf[2938],2);
  test_data.data32 = 0.8;	//��ͨ������������� 0.8
  usRegInputBuf[2939] = test_data.data16[1];
  AT24CXX_WriteLenByte(94,usRegInputBuf[2939],2);
  usRegInputBuf[2940] = test_data.data16[0];		
  AT24CXX_WriteLenByte(96,usRegInputBuf[2940],2);
  test_data.data32 = 80;	//��ͨ�������������� 80
  usRegInputBuf[2945] = test_data.data16[1]; 
    AT24CXX_WriteLenByte(106,usRegInputBuf[2945],2);
  usRegInputBuf[2946] = test_data.data16[0];		
  AT24CXX_WriteLenByte(108,usRegInputBuf[2946],2);
  test_data.data32 = 168;	//��ͨ������ɨ���� 168
  usRegInputBuf[2947] = test_data.data16[1];
  AT24CXX_WriteLenByte(110,usRegInputBuf[2947],2);
  usRegInputBuf[2948] = test_data.data16[0];		
   AT24CXX_WriteLenByte(112,usRegInputBuf[2948],2);
  test_data.data32 = 0.3;	//��ͨ������ɨ����� 0.3
  usRegInputBuf[2949] = test_data.data16[1];
  AT24CXX_WriteLenByte(114,usRegInputBuf[2949],2);
  usRegInputBuf[2950] = test_data.data16[0];		
   AT24CXX_WriteLenByte(116,usRegInputBuf[2950],2);
  usRegInputBuf[2951]= 0;//��ͨ������������
  usRegInputBuf[2952]= 0;//��ͨ������/ֹͣ����
   AT24CXX_WriteLenByte(120,usRegInputBuf[2952],2);
  
  usRegInputBuf[2963]= 0;//һͨ������ֹͣ/����
     AT24CXX_WriteLenByte(220,usRegInputBuf[2963],2);
  usRegInputBuf[2964]= 0;//��ͨ������ֹͣ/����
  AT24CXX_WriteLenByte(222,usRegInputBuf[2964],2);

  test_data.data32 = 0.0;	//һͨ������������� 
  usRegInputBuf[2965] = test_data.data16[1];
  AT24CXX_WriteLenByte(228,usRegInputBuf[2965],2);
  usRegInputBuf[2966] = test_data.data16[0];	
    AT24CXX_WriteLenByte(230,usRegInputBuf[2966],2);
  test_data.data32 = 0.0;	//һͨ������������� 
  usRegInputBuf[2967] = test_data.data16[1];
  AT24CXX_WriteLenByte(232,usRegInputBuf[2967],2);
  usRegInputBuf[2968] = test_data.data16[0];	
   AT24CXX_WriteLenByte(234,usRegInputBuf[2968],2);
  
  test_data.data32 = 0.0;	//��ͨ������������� 
  usRegInputBuf[2969] = test_data.data16[1];
  AT24CXX_WriteLenByte(236,usRegInputBuf[2969],2);
  usRegInputBuf[2970] = test_data.data16[0];	
  AT24CXX_WriteLenByte(238,usRegInputBuf[2970],2);
  
  test_data.data32 = 0.0;	//��ͨ������������� 
  usRegInputBuf[2971] = test_data.data16[1];
  AT24CXX_WriteLenByte(240,usRegInputBuf[2971],2);
  usRegInputBuf[2972] = test_data.data16[0];	
     AT24CXX_WriteLenByte(242,usRegInputBuf[2972],2);
  
  test_data.data32 = 13.5;	//һͨ�������ѹ
  usRegInputBuf[2973] = test_data.data16[1];
    AT24CXX_WriteLenByte(260,usRegInputBuf[2973],2);
  usRegInputBuf[2974] = test_data.data16[0];	
  AT24CXX_WriteLenByte(262,usRegInputBuf[2974],2);
  
  test_data.data32 = 13.5;	//��ͨ�������ѹ
  usRegInputBuf[2975] = test_data.data16[1];
  AT24CXX_WriteLenByte(264,usRegInputBuf[2975],2);
  usRegInputBuf[2976] = test_data.data16[0];	
  AT24CXX_WriteLenByte(266,usRegInputBuf[2976],2);
  
  

  
  for(i=0;i<256;i++)
  {
    AT24CXX_WriteOneByte(300+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(301+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(302+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(303+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);      
  }
  
  
}

u8 AT24CXX_Check_R(u8 i)
{
  union float_to_u16 test_data;
  test_data.data16[1]=AT24CXX_ReadLenByte(300+4*i,2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  test_data.data16[0]=AT24CXX_ReadLenByte(302+4*i,2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  //����ÿ�ο�����дAT24CXX			   
  if((test_data.data32>0.0)&&(test_data.data32<255.937))return 0;		   
  else//�ų���һ�γ�ʼ�������
  {		
    AT24CXX_WriteLenByte(300+4*i,usRegInputBuf[2*i+1024],2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteLenByte(302+4*i,usRegInputBuf[2*i+1+1024],2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    test_data.data16[1]=AT24CXX_ReadLenByte(300+4*i,2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    test_data.data16[0]=AT24CXX_ReadLenByte(302+4*i,2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    if((test_data.data32>0.0)&&(test_data.data32<255.937))return 0;
  }
  return 1;											  
}

u8 AT24CXX_Check_R2(u8 i)
{
  union float_to_u16 test_data;
  test_data.data16[1]=AT24CXX_ReadLenByte(300+4*(i+128),2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  test_data.data16[0]=AT24CXX_ReadLenByte(302+4*(i+128),2);
  OSTimeDlyHMSM(0, 0, 0, 1);
  //����ÿ�ο�����дAT24CXX			   
  if((test_data.data32>0.0)&&(test_data.data32<255.937))return 0;		   
  else//�ų���һ�γ�ʼ�������
  {		
    AT24CXX_WriteLenByte(300+4*(i+128),usRegInputBuf[2*i+1280],2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteLenByte(302+4*(i+128),usRegInputBuf[2*i+1+1280],2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    test_data.data16[1]=AT24CXX_ReadLenByte(300+4*(i+128),2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    test_data.data16[0]=AT24CXX_ReadLenByte(302+4*(i+128),2);
    OSTimeDlyHMSM(0, 0, 0, 1);
    if((test_data.data32>0.0)&&(test_data.data32<255.937))return 0;
  }
  return 1;											  
}

void AT24CXX_Reset_Rdata(void)
{
  u16 i;
  for(i=0;i<256;i++)
  {
    AT24CXX_WriteOneByte(300+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(301+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(302+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(303+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);      
  }
}

void AT24CXX_Reset_ORdata(u8 i)
{
    AT24CXX_WriteOneByte(300+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(301+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(302+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);
    AT24CXX_WriteOneByte(303+4*i,0);
    OSTimeDlyHMSM(0, 0, 0, 1);      
}
