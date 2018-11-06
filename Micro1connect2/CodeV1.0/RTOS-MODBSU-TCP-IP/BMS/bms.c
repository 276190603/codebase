//#include "sys.h"
#include "bms.h"
#include "RS485.h"	
#include "math.h"
#include <includes.h>
//#include "includes.h"
int rerror_flag;
#define TE	0x0080
UCHAR TxBuffer1[10];
UCHAR RxBuffer1[10];
/****************************************************************************
* ��    �ƣ�flaot  FUN_Read_R (unsigned int id)
* ��    �ܣ���ȡ������������
* ��ڲ�����ID��(id);cmd����
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
float  FUN_Read_R (unsigned int id,unsigned int cmd)
{
	float v=0;
	u8 len=4;	
	
	TxBuffer1[0]=id;
	TxBuffer1[1]=cmd;
	TxBuffer1[2]=TxBuffer1[1]^TxBuffer1[0];
	RS485_Send_Data(TxBuffer1,3);//����3���ֽ� 
	OSTimeDlyHMSM(0, 0, 0, 50);
	RS485_Receive_Data(RxBuffer1,&len);
	if(RxBuffer1[0]==TxBuffer1[0])
	{
		if(RxBuffer1[0]^RxBuffer1[1]^RxBuffer1[2]==RxBuffer1[3])
		{
			if(RxBuffer1[1]==0&&RxBuffer1[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((RxBuffer1[1]&0x80)>>7)+1)*powf(2,((RxBuffer1[1]&0x78)>>3)-7)*(1+((RxBuffer1[1]&0x04)>>2)*powf(2,(-1))+((RxBuffer1[1]&0x02)>>1)*powf(2,(-2))+((RxBuffer1[1]&0x01)>>0)*powf(2,(-3))+((RxBuffer1[2]&0x80)>>7)*powf(2,(-4))+((RxBuffer1[2]&0x40)>>6)*powf(2,(-5))+((RxBuffer1[2]&0x20)>>5)*powf(2,(-6))+((RxBuffer1[2]&0x10)>>4)*powf(2,(-7))+((RxBuffer1[2]&0x08)>>3)*powf(2,(-8))+((RxBuffer1[2]&0x04)>>2)*powf(2,(-9))+((RxBuffer1[2]&0x02)>>1)*powf(2,(-10))+((RxBuffer1[2]&0x01)>>0)*powf(2,(-11)));
			rerror_flag=rerror_flag&(~TE);				
			RxBuffer1[0]=0;
			RxBuffer1[1]=0;
			RxBuffer1[2]=0;
			RxBuffer1[3]=0;
			return (1000*v);
		}	
	}
	else
	{
			rerror_flag=rerror_flag|(TE);		
	}	
	return v;
}

//�㲥�봫��������ͬѧ
char  FUN_SEND_CMD_broadcast (unsigned int id)
{
        u8 v=0;
	u8 len=3;
	TxBuffer1[0]=0xff;
	TxBuffer1[1]=0x12;
	TxBuffer1[2]=id;
	RS485_Send_Data(TxBuffer1,3);//����3���ֽ� 
        OSTimeDlyHMSM(0, 0, 0, 50);
	RS485_Receive_Data(RxBuffer1,&len);
	if(RxBuffer1[0]==0x12)
	{
		if(RxBuffer1[0]^RxBuffer1[1]==RxBuffer1[2])
		{
			rerror_flag=rerror_flag&(~TE);				
			RxBuffer1[0]=0;
			RxBuffer1[1]=0;
			RxBuffer1[2]=0;
			RxBuffer1[3]=0;
			return (1);
		}	
	}
	else
	{
			rerror_flag=rerror_flag|(TE);	
                        return v;
	}	
	return v;
}

/****************************************************************************
* ��    �ƣ�flaot  FUN_Read_R (unsigned int id)
* ��    �ܣ���ȡ������������
* ��ڲ�����ID��(id);cmd����
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void  FUN_SEND_CMD (unsigned int id,unsigned int cmd)
{
	
	TxBuffer1[0]=id;
	TxBuffer1[1]=cmd;
	TxBuffer1[2]=TxBuffer1[1]^TxBuffer1[0];
	RS485_Send_Data(TxBuffer1,3);//����3���ֽ� 
}
float FUN_READ_DATA(void)
{
	float v=0;
	u8 len=4;
	RS485_Receive_Data(RxBuffer1,&len);
	if(RxBuffer1[0]==TxBuffer1[0])
	{
		if(RxBuffer1[0]^RxBuffer1[1]^RxBuffer1[2]==RxBuffer1[3])
		{
			if(RxBuffer1[1]==0&&RxBuffer1[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((RxBuffer1[1]&0x80)>>7)+1)*powf(2,((RxBuffer1[1]&0x78)>>3)-7)*(1+((RxBuffer1[1]&0x04)>>2)*powf(2,(-1))+((RxBuffer1[1]&0x02)>>1)*powf(2,(-2))+((RxBuffer1[1]&0x01)>>0)*powf(2,(-3))+((RxBuffer1[2]&0x80)>>7)*powf(2,(-4))+((RxBuffer1[2]&0x40)>>6)*powf(2,(-5))+((RxBuffer1[2]&0x20)>>5)*powf(2,(-6))+((RxBuffer1[2]&0x10)>>4)*powf(2,(-7))+((RxBuffer1[2]&0x08)>>3)*powf(2,(-8))+((RxBuffer1[2]&0x04)>>2)*powf(2,(-9))+((RxBuffer1[2]&0x02)>>1)*powf(2,(-10))+((RxBuffer1[2]&0x01)>>0)*powf(2,(-11)));
			rerror_flag=rerror_flag&(~TE);					
			RxBuffer1[0]=0;
			RxBuffer1[1]=0;
			RxBuffer1[2]=0;
			RxBuffer1[3]=0;
			return (v);
		}
		
	}
	else
	{
			rerror_flag=rerror_flag|(TE);		
	}	
	return v;
}
/****************************************************************************
* ��    �ƣ�flaot  FUN_Read_V (unsigned int id)
* ��    �ܣ���ȡ�������ĵ�ѹ
* ��ڲ�����ID��(id);cmd����
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
float  FUN_Read_V (unsigned int id,unsigned int cmd)
{
	float v=0;
	u8 len=4;
	TxBuffer1[0]=id;
	TxBuffer1[1]=cmd;
	TxBuffer1[2]=TxBuffer1[1]^TxBuffer1[0];
	TxBuffer1[3]=0x00;
	RS485_Send_Data(TxBuffer1,3);//����3���ֽ� 
	OSTimeDlyHMSM(0, 0, 0, 50);
	RS485_Receive_Data(RxBuffer1,&len);
	if(RxBuffer1[0]==TxBuffer1[0])
	{
		if(RxBuffer1[0]^RxBuffer1[1]^RxBuffer1[2]==RxBuffer1[3])
		{
			if(RxBuffer1[1]==0&&RxBuffer1[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((RxBuffer1[1]&0x80)>>7)+1)*powf(2,((RxBuffer1[1]&0x78)>>3)-7)*(1+((RxBuffer1[1]&0x04)>>2)*powf(2,(-1))+((RxBuffer1[1]&0x02)>>1)*powf(2,(-2))+((RxBuffer1[1]&0x01)>>0)*powf(2,(-3))+((RxBuffer1[2]&0x80)>>7)*powf(2,(-4))+((RxBuffer1[2]&0x40)>>6)*powf(2,(-5))+((RxBuffer1[2]&0x20)>>5)*powf(2,(-6))+((RxBuffer1[2]&0x10)>>4)*powf(2,(-7))+((RxBuffer1[2]&0x08)>>3)*powf(2,(-8))+((RxBuffer1[2]&0x04)>>2)*powf(2,(-9))+((RxBuffer1[2]&0x02)>>1)*powf(2,(-10))+((RxBuffer1[2]&0x01)>>0)*powf(2,(-11)));			
			rerror_flag=rerror_flag&(~TE);	
                        RxBuffer1[0]=0;
			RxBuffer1[1]=0;
			RxBuffer1[2]=0;
			RxBuffer1[3]=0;
			return v;
		}
		
	}
	else
	{
			if(TxBuffer1[0]!=0xff)
			{
				rerror_flag=rerror_flag|(TE);	
			}
	}	
	return v;
}
/****************************************************************************
* ��    �ƣ�flaot  FUN_Read_T (unsigned int id)
* ��    �ܣ���ȡ���������¶�
* ��ڲ�����ID��(id);cmd����
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
float  FUN_Read_T (unsigned int id,unsigned int cmd)
{
	float v=0;
	u8 len=4;
	TxBuffer1[0]=id;
	TxBuffer1[1]=cmd;
	TxBuffer1[2]=TxBuffer1[1]^TxBuffer1[0];
	TxBuffer1[3]=0x00;
	RS485_Send_Data(TxBuffer1,3);//����3���ֽ� 
	OSTimeDlyHMSM(0, 0, 0, 50);
	RS485_Receive_Data(RxBuffer1,&len);
	if(RxBuffer1[0]==TxBuffer1[0])
	{
		if(RxBuffer1[0]^RxBuffer1[1]^RxBuffer1[2]==RxBuffer1[3])
		{
	 		if(RxBuffer1[1]==0&&RxBuffer1[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((RxBuffer1[1]&0x80)>>7)+1)*powf(2,((RxBuffer1[1]&0x78)>>3)-7)*(1+((RxBuffer1[1]&0x04)>>2)*powf(2,(-1))+((RxBuffer1[1]&0x02)>>1)*powf(2,(-2))+((RxBuffer1[1]&0x01)>>0)*powf(2,(-3))+((RxBuffer1[2]&0x80)>>7)*powf(2,(-4))+((RxBuffer1[2]&0x40)>>6)*powf(2,(-5))+((RxBuffer1[2]&0x20)>>5)*powf(2,(-6))+((RxBuffer1[2]&0x10)>>4)*powf(2,(-7))+((RxBuffer1[2]&0x08)>>3)*powf(2,(-8))+((RxBuffer1[2]&0x04)>>2)*powf(2,(-9))+((RxBuffer1[2]&0x02)>>1)*powf(2,(-10))+((RxBuffer1[2]&0x01)>>0)*powf(2,(-11)));
			v=(v-32)/1.8; //���϶�ת�������϶�
			rerror_flag=rerror_flag&(~TE);	
			RxBuffer1[0]=0;
			RxBuffer1[1]=0;
			RxBuffer1[2]=0;
			RxBuffer1[3]=0;
			return v;
		}
		
	}
	else
	{
			if(TxBuffer1[0]!=0xff)
			{
					rerror_flag=rerror_flag|(TE);		
			}
	}	
	return v;
}





/****************************************************************************
* ��    �ƣ�void  FUN_SEND_JUNHENG_V (unsigned int id, unsigned int data)
* ��    �ܣ����;����ѹֵ
* ��ڲ�����ID��(id);data�����ѹֵ
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void  FUN_SEND_JUNHENG_V (unsigned int id, unsigned int data)
{
	
	TxBuffer1[0]=id;
	TxBuffer1[1]=(data>>8);
	TxBuffer1[2]=(data&0x00ff);
	TxBuffer1[3]= TxBuffer1[0]^TxBuffer1[1]^TxBuffer1[2];
	RS485_Send_Data(TxBuffer1,4);//����4���ֽ� 
}
















