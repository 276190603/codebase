//#include "sys.h"
#include "bms.h"
#include "UART5.h"	
#include "math.h"
#include <includes.h>
//#include "includes.h"
//int rerror_flag;
#define TE	0x0080
UCHAR UART5_TxBuffer[10];
UCHAR UART5_RxBuffer[10];
/****************************************************************************
* 名    称：flaot  UART5_Read_R (unsigned int id)
* 功    能：读取传感器的内阻
* 入口参数：ID号(id);cmd命令
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
float  UART5_Read_R (unsigned int id,unsigned int cmd)
{
	float v=0;
	u8 len=4;	
	
	UART5_TxBuffer[0]=id;
	UART5_TxBuffer[1]=cmd;
	UART5_TxBuffer[2]=UART5_TxBuffer[1]^UART5_TxBuffer[0];
	UART5_Send_Data(UART5_TxBuffer,3);//发送3个字节 
	OSTimeDlyHMSM(0, 0, 0, 50);
	UART5_Receive_Data(UART5_RxBuffer,&len);
	if(UART5_RxBuffer[0]==UART5_TxBuffer[0])
	{
		if(UART5_RxBuffer[0]^UART5_RxBuffer[1]^UART5_RxBuffer[2]==UART5_RxBuffer[3])
		{
			if(UART5_RxBuffer[1]==0&&UART5_RxBuffer[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((UART5_RxBuffer[1]&0x80)>>7)+1)*powf(2,((UART5_RxBuffer[1]&0x78)>>3)-7)*(1+((UART5_RxBuffer[1]&0x04)>>2)*powf(2,(-1))+((UART5_RxBuffer[1]&0x02)>>1)*powf(2,(-2))+((UART5_RxBuffer[1]&0x01)>>0)*powf(2,(-3))+((UART5_RxBuffer[2]&0x80)>>7)*powf(2,(-4))+((UART5_RxBuffer[2]&0x40)>>6)*powf(2,(-5))+((UART5_RxBuffer[2]&0x20)>>5)*powf(2,(-6))+((UART5_RxBuffer[2]&0x10)>>4)*powf(2,(-7))+((UART5_RxBuffer[2]&0x08)>>3)*powf(2,(-8))+((UART5_RxBuffer[2]&0x04)>>2)*powf(2,(-9))+((UART5_RxBuffer[2]&0x02)>>1)*powf(2,(-10))+((UART5_RxBuffer[2]&0x01)>>0)*powf(2,(-11)));
			rerror_flag=rerror_flag&(~TE);				
			UART5_RxBuffer[0]=0;
			UART5_RxBuffer[1]=0;
			UART5_RxBuffer[2]=0;
			UART5_RxBuffer[3]=0;
			return (1000*v);
		}	
	}
	else
	{
			rerror_flag=rerror_flag|(TE);		
	}	
	return v;
}

//广播与传感器建立同学
char  UART5_SEND_CMD_broadcast (unsigned int id)
{
        u8 v=0;
	u8 len=3;
	UART5_TxBuffer[0]=0xff;
	UART5_TxBuffer[1]=0x12;
	UART5_TxBuffer[2]=id;
	UART5_Send_Data(UART5_TxBuffer,3);//发送3个字节 
        OSTimeDlyHMSM(0, 0, 0, 50);
	UART5_Receive_Data(UART5_RxBuffer,&len);
	if(UART5_RxBuffer[0]==0x12)
	{
		if(UART5_RxBuffer[0]^UART5_RxBuffer[1]==UART5_RxBuffer[2])
		{
			rerror_flag=rerror_flag&(~TE);				
			UART5_RxBuffer[0]=0;
			UART5_RxBuffer[1]=0;
			UART5_RxBuffer[2]=0;
			UART5_RxBuffer[3]=0;
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
* 名    称：flaot  UART5_Read_R (unsigned int id)
* 功    能：读取传感器的内阻
* 入口参数：ID号(id);cmd命令
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void  UART5_SEND_CMD (unsigned int id,unsigned int cmd)
{
	
	UART5_TxBuffer[0]=id;
	UART5_TxBuffer[1]=cmd;
	UART5_TxBuffer[2]=UART5_TxBuffer[1]^UART5_TxBuffer[0];
	UART5_Send_Data(UART5_TxBuffer,3);//发送3个字节 
}
float UART5_READ_DATA(void)
{
	float v=0;
	u8 len=4;
	UART5_Receive_Data(UART5_RxBuffer,&len);
	if(UART5_RxBuffer[0]==UART5_TxBuffer[0])
	{
		if(UART5_RxBuffer[0]^UART5_RxBuffer[1]^UART5_RxBuffer[2]==UART5_RxBuffer[3])
		{
			if(UART5_RxBuffer[1]==0&&UART5_RxBuffer[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((UART5_RxBuffer[1]&0x80)>>7)+1)*powf(2,((UART5_RxBuffer[1]&0x78)>>3)-7)*(1+((UART5_RxBuffer[1]&0x04)>>2)*powf(2,(-1))+((UART5_RxBuffer[1]&0x02)>>1)*powf(2,(-2))+((UART5_RxBuffer[1]&0x01)>>0)*powf(2,(-3))+((UART5_RxBuffer[2]&0x80)>>7)*powf(2,(-4))+((UART5_RxBuffer[2]&0x40)>>6)*powf(2,(-5))+((UART5_RxBuffer[2]&0x20)>>5)*powf(2,(-6))+((UART5_RxBuffer[2]&0x10)>>4)*powf(2,(-7))+((UART5_RxBuffer[2]&0x08)>>3)*powf(2,(-8))+((UART5_RxBuffer[2]&0x04)>>2)*powf(2,(-9))+((UART5_RxBuffer[2]&0x02)>>1)*powf(2,(-10))+((UART5_RxBuffer[2]&0x01)>>0)*powf(2,(-11)));
			rerror_flag=rerror_flag&(~TE);					
			UART5_RxBuffer[0]=0;
			UART5_RxBuffer[1]=0;
			UART5_RxBuffer[2]=0;
			UART5_RxBuffer[3]=0;
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
* 名    称：flaot  UART5_Read_V (unsigned int id)
* 功    能：读取传感器的电压
* 入口参数：ID号(id);cmd命令
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
float  UART5_Read_V (unsigned int id,unsigned int cmd)
{
	float v=0;
	u8 len=4;
	UART5_TxBuffer[0]=id;
	UART5_TxBuffer[1]=cmd;
	UART5_TxBuffer[2]=UART5_TxBuffer[1]^UART5_TxBuffer[0];
	UART5_TxBuffer[3]=0x00;
	UART5_Send_Data(UART5_TxBuffer,3);//发送3个字节 
	OSTimeDlyHMSM(0, 0, 0, 50);
	UART5_Receive_Data(UART5_RxBuffer,&len);
	if(UART5_RxBuffer[0]==UART5_TxBuffer[0])
	{
		if(UART5_RxBuffer[0]^UART5_RxBuffer[1]^UART5_RxBuffer[2]==UART5_RxBuffer[3])
		{
			if(UART5_RxBuffer[1]==0&&UART5_RxBuffer[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((UART5_RxBuffer[1]&0x80)>>7)+1)*powf(2,((UART5_RxBuffer[1]&0x78)>>3)-7)*(1+((UART5_RxBuffer[1]&0x04)>>2)*powf(2,(-1))+((UART5_RxBuffer[1]&0x02)>>1)*powf(2,(-2))+((UART5_RxBuffer[1]&0x01)>>0)*powf(2,(-3))+((UART5_RxBuffer[2]&0x80)>>7)*powf(2,(-4))+((UART5_RxBuffer[2]&0x40)>>6)*powf(2,(-5))+((UART5_RxBuffer[2]&0x20)>>5)*powf(2,(-6))+((UART5_RxBuffer[2]&0x10)>>4)*powf(2,(-7))+((UART5_RxBuffer[2]&0x08)>>3)*powf(2,(-8))+((UART5_RxBuffer[2]&0x04)>>2)*powf(2,(-9))+((UART5_RxBuffer[2]&0x02)>>1)*powf(2,(-10))+((UART5_RxBuffer[2]&0x01)>>0)*powf(2,(-11)));			
			rerror_flag=rerror_flag&(~TE);	
                        UART5_RxBuffer[0]=0;
			UART5_RxBuffer[1]=0;
			UART5_RxBuffer[2]=0;
			UART5_RxBuffer[3]=0;
			return v;
		}
		
	}
	else
	{
			if(UART5_TxBuffer[0]!=0xff)
			{
				rerror_flag=rerror_flag|(TE);	
			}
	}	
	return v;
}
/****************************************************************************
* 名    称：flaot  UART5_Read_T (unsigned int id)
* 功    能：读取传感器的温度
* 入口参数：ID号(id);cmd命令
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
float  UART5_Read_T (unsigned int id,unsigned int cmd)
{
	float v=0;
	u8 len=4;
	UART5_TxBuffer[0]=id;
	UART5_TxBuffer[1]=cmd;
	UART5_TxBuffer[2]=UART5_TxBuffer[1]^UART5_TxBuffer[0];
	UART5_TxBuffer[3]=0x00;
	UART5_Send_Data(UART5_TxBuffer,3);//发送3个字节 
	OSTimeDlyHMSM(0, 0, 0, 50);
	UART5_Receive_Data(UART5_RxBuffer,&len);
	if(UART5_RxBuffer[0]==UART5_TxBuffer[0])
	{
		if(UART5_RxBuffer[0]^UART5_RxBuffer[1]^UART5_RxBuffer[2]==UART5_RxBuffer[3])
		{
	 		if(UART5_RxBuffer[1]==0&&UART5_RxBuffer[2]==0)
			{
				rerror_flag=rerror_flag|(TE);	
				return	0;
			}
			v=(-2*((UART5_RxBuffer[1]&0x80)>>7)+1)*powf(2,((UART5_RxBuffer[1]&0x78)>>3)-7)*(1+((UART5_RxBuffer[1]&0x04)>>2)*powf(2,(-1))+((UART5_RxBuffer[1]&0x02)>>1)*powf(2,(-2))+((UART5_RxBuffer[1]&0x01)>>0)*powf(2,(-3))+((UART5_RxBuffer[2]&0x80)>>7)*powf(2,(-4))+((UART5_RxBuffer[2]&0x40)>>6)*powf(2,(-5))+((UART5_RxBuffer[2]&0x20)>>5)*powf(2,(-6))+((UART5_RxBuffer[2]&0x10)>>4)*powf(2,(-7))+((UART5_RxBuffer[2]&0x08)>>3)*powf(2,(-8))+((UART5_RxBuffer[2]&0x04)>>2)*powf(2,(-9))+((UART5_RxBuffer[2]&0x02)>>1)*powf(2,(-10))+((UART5_RxBuffer[2]&0x01)>>0)*powf(2,(-11)));
			v=(v-32)/1.8; //华氏度转换成摄氏度
			rerror_flag=rerror_flag&(~TE);	
			UART5_RxBuffer[0]=0;
			UART5_RxBuffer[1]=0;
			UART5_RxBuffer[2]=0;
			UART5_RxBuffer[3]=0;
			return v;
		}
		
	}
	else
	{
			if(UART5_TxBuffer[0]!=0xff)
			{
					rerror_flag=rerror_flag|(TE);		
			}
	}	
	return v;
}





/****************************************************************************
* 名    称：void  UART5_SEND_JUNHENG_V (unsigned int id, unsigned int data)
* 功    能：发送均衡电压值
* 入口参数：ID号(id);data均衡电压值
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void  UART5_SEND_JUNHENG_V (unsigned int id, unsigned int data)
{
	
	UART5_TxBuffer[0]=id;
	UART5_TxBuffer[1]=(data>>8);
	UART5_TxBuffer[2]=(data&0x00ff);
	UART5_TxBuffer[3]= UART5_TxBuffer[0]^UART5_TxBuffer[1]^UART5_TxBuffer[2];
	UART5_Send_Data(UART5_TxBuffer,4);//发送4个字节 
}
















