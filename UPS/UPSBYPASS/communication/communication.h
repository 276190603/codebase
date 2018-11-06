//-------------------------------------------------------------
// 文件名： "Communication.h"
//-------------------------------------------------------------
//功能：	1.包含Communication.c所用头文件
//		2.进行相关宏定义
//      3.声明全局变量
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2017年1月9日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------

#ifndef _COMMUNICATION
#define _COMMUNICATION

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "ComStruct.h"
#include "RS232.h"
#include "Scr.h"
#include "GlobalDefine.h"
#include "RS485.h"
#include "CAN.h"

//-------------------------------------------------------------
// 宏定义
//-------------------------------------------------------------

//Modbus中各个数据、命令的寄存器地址(功能码)
#define SYS_START       0      //整流启动/停止

#define UA_SCROUT_ADD	1			//整流A相电压
#define UB_SCROUT_ADD   2           //整流B相电压
#define UC_SCROUT_ADD   3           //整流C相电压
#define IA_SCROUT_ADD	4			//逆变输出A相电流

#define FRAB_PHASE_ADD      6           //整流A相频率
#define FRA_BP_ADD		7			//整流A相频率
#define FRB_BP_ADD      8           //整流A相频率

#define UA_SCROUTR_ADD 10			//整流DC有效值

#define DC_Target_ADD 16           //整流DC有效值

//-------------------------------------------------------------
// 结构体声明、枚举声明
//-------------------------------------------------------------
typedef enum ComModule
{
	RS232Mdl = 0
	,RS485Mdl
	,CanMdl
}ComModule;

typedef struct ComEnableBits	//使能位位定义
{								//	bit	description
	Uint16 Enable:1;			//	0	全局通讯使能
	Uint16 Rs232Enable:1;		//	1	232使能位
	Uint16 Rs485Enable:1;		//	2	484使能位
	Uint16	CanEnable:1;		//	3	Can使能位
}ComEnableBits;
typedef union ComEnable
{
	ComEnableBits bit;
	Uint16 all;
}ComEnable;


typedef struct Communication
{
	//属性
	ComEnable Enable;		//模块使能
	SCICom RS232Com;		//232模块
	SCICom RS485Com;		//485模块
	CANCom CANCom;			//Can模块

	//方法
	Uint16 (*Init)(void);
	Uint16 (*ChangeState)(State state);
}Communication;

//-------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------
extern Communication myCommunication;

//-------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------
void UsemyCommunication(void);
Uint16 Write2Buf(float _data,ComModule _md);
Uint16 Write06Buf(ComModule _md);
Uint16 Write16Buf(ComModule _md);
void Float2Uint16(float _fData,Uint16 *_u16Data);
Uint16 VerifyFrame(ComModule _md);
void Unit16toFloat(float _fData,Uint16 *_u16Data);

#endif
