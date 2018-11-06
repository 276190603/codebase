//-------------------------------------------------------------
// 文件名： "ComStruct.h"
//-------------------------------------------------------------
//功能：	1.定义通讯模块所有的结构
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2017年1月12日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------
#ifndef COMSTRUCT_H_
#define COMSTRUCT_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "GlobalDefine.h"

/*------------------------SCI模块定义-----------------------------------*/

//-------------------------------------------------------------
// 宏定义
//-------------------------------------------------------------
#define SCI_TXBUFFER_SIZE 12		//定义SCI发送缓冲区大小
#define SCI_RXBUFFER_SIZE 28		//定义SCI接收缓冲区大小

//-------------------------------------------------------------
//枚举类型、 结构体声明
//-------------------------------------------------------------
typedef enum Parity
{
	NoP = 0
	,OddP
	,EvenP
}Parity;
typedef struct UartFlagBits
{								//	bit	description
	Uint16 RecData:1;			//	0	接收到数据置位，用于字节接收延时
	Uint16 SendEnd:1;			//	1	发送完毕标志位，用于发送完成延迟
	Uint16 RecFrameFlag:1;		//	2	接收到一针数据标志
	Uint16 SendEmptyFlag:1;		//	3	发送缓冲区空标志
	Uint16 RecOverflow:1;		//	4	接收数据溢出
}UartFlagBits;
typedef union UartFlag
{
	UartFlagBits bit;
	Uint16 all;
}UartFlag;

typedef struct SCICom
{
	//属性
	Uint16 BaudRate;		//波特率(2000~40000 bps)
	Uint16 DataBits;		//数据位(1~8)
	Uint16 StopBits;		//停止位(1或者2位)
	Parity Parity;			//校验位
	Uint16 SendBuffer[SCI_TXBUFFER_SIZE];	//发送缓冲区
	Uint16 RecBuffer[SCI_RXBUFFER_SIZE];	//接收缓冲区
	Uint16 RecNum;			//接收数据的个数
	UartFlag Flag;			//标志位
	//方法
	Uint16 (*Init)(void);						//初始化
	Uint16 (*ChangeState)(State state);			//改变状态
	Uint16 (*SendData)(Uint16 num);				//发送缓冲区中的前num个数据
	Uint16 (*MB03Return)(float _data);			//使用Modbus 03命令发送一个浮点数
	Uint16 (*Scan)(void);						//RS232扫描函数
}SCICom;

/*------------------------CAN模块定义-----------------------------------*/

//-------------------------------------------------------------
//枚举类型、 结构体声明
//-------------------------------------------------------------
typedef enum CanBaudRate
{
	br125k = 0
	,br250k
	,br500k
	,br750k
	,br1M
}CanBaudRate;

typedef struct CanId
{					//	bit		description
	Uint32 Id:29;	//	0-28	id
	Uint32 rsv:3;	//	29-31	保留
}CanId;

typedef struct CanFlagBit
{									//	bit		description
	Uint16 TxBufEmpty:1;			//	0		发送缓冲区空
	Uint16 AutoTxBufEmpty:1;		//	1		自动发送缓冲区空
	Uint16 RecFrame:1;				//	2		接收到数据帧标志位
	Uint16 AutoRecFrame:1;			//	3		自动接收到数据帧标志位
}CanFlagBit;

typedef union CanFlag
{
	Uint16 all;
	CanFlagBit bit;
}CanFlag;

typedef enum MailBoxType
{
	SendMailBox = 0
	,AutoSendMailBox
	,RecMailBox
	,AutoRecMailBox
}MailBoxType;

typedef struct CANCom
{
	//属性
	CanBaudRate BaudRate;			//波特率
	CanId SendId;					//发送邮箱ID
	CanId RecId;					//接收邮箱ID
	CanId RecIdMask;				//接收邮箱掩码
	CanId AutoSendId;				//自动发送邮箱ID
	CanId AutoRecId;				//自动接收邮箱ID
	Uint16 SendBuf[8];				//发送数据缓冲区
	Uint16 RecBuf[8];				//接收数据缓冲区
	Uint16 RecNum;					//接收数据个数
	Uint16 AutoRecBuf[8];			//自动接收数据缓冲区
	Uint16 AutoRecNum;				//自动接收数据个数
	CanFlag Flag;					//标志位

	//方法
	Uint16 (*Init)(void);														//初始化
	Uint16 (*ChangeState)(State state);											//改变状态
	Uint16 (*ChangeAutoSendState)(State state);									//改变自动发送状态
	Uint16 (*SendData)(Uint16 num,MailBoxType mbType);			//发送缓冲区前num个数据
	Uint16 (*SendDataWithPtc)(Uint16 FunCode,float data1,float data2,float data3,float data4,float data5,Uint16 data6,MailBoxType mbType) ;   //采用通讯协议发送数据					//使用协议发送浮点数
	Uint16 (*AutoRecData)(void);												//发送一个远程帧并自动接收数据
	Uint16 (*Scan)(void);														//循环扫描有无中断事件需要处理
}CANCom;

#endif
