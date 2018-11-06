//-------------------------------------------------------------
// 文件名： "Inverter.h"
//-------------------------------------------------------------
//功能：	1.包含Inverter.c所用头文件
//		2.进行相关宏定义
//      3.声明全局变量
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2016年12月27日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------

#ifndef INVERTER_H
#define INVERTER_H

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "GlobalDefine.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "SampleAndControl.h"
#include "PhaseLock.h"
#include "FaultDetect.h"
#include "communication.h"
#include "SystemTick.h"
#include "FpgaDriver.h"

//-------------------------------------------------------------

//-------------------------------------------------------------
// 宏定义
//-------------------------------------------------------------


//-------------------------------------------------------------
// 枚举类型声明
//-------------------------------------------------------------
typedef enum OutputSource
{
	BP = 0
	,Inv
}OutputSource;

//-------------------------------------------------------------
// 结构体声明
//-------------------------------------------------------------
typedef struct Inverter
{
	State State;					//逆变开关
	float IA_InvOut;				//逆变器A相输出电流
	float IB_InvOut;				//逆变器B相输出电流
	float IC_InvOut;				//逆变器C相输出电流
	float IA_TotalOut;				//A相总输出电流
	float IB_TotalOut;				//B相总输出电流
	float IC_TotalOut;				//C相总输出电流
	float U_DC;						//DC母线电压
	float UA_InvOut;				//逆变器A相输出电压
	float UB_InvOut;				//逆变器B相输出电压
	float UC_InvOut;				//逆变器C相输出电压
	float UA_BP;					//旁路A相电压
	float UB_BP;					//旁路B相电压
	float UC_BP;					//旁路C相电压

	float IA_InvOutRMS;				//逆变器A相输出电流有效值
	float IB_InvOutRMS;				//逆变器B相输出电流有效值
	float IC_InvOutRMS;				//逆变器C相输出电流有效值
	float IA_TotalOutRMS;			//A相总输出电流有效值
	float IB_TotalOutRMS;			//B相总输出电流有效值
	float IC_TotalOutRMS;			//C相总输出电流有效值
	float U_DCRMS;					//DC母线电压有效值
	float UA_InvOutRMS;				//逆变器A相输出电压有效值
	float UB_InvOutRMS;				//逆变器B相输出电压有效值
	float UC_InvOutRMS;				//逆变器C相输出电压有效值
	float UA_BPRMS;					//旁路A相电压有效值
	float UB_BPRMS;					//旁路B相电压有效值
	float UC_BPRMS;					//旁路C相电压有效值

	float FrA_BP;					//旁路A相频率
	float FrB_BP;					//旁路B相频率
	float FrC_BP;					//旁路C相频率
	float PhA_BP;					//旁路A相相位
	float PhB_BP;					//旁路B相相位
	float PhC_BP;					//旁路C相相位
	float FrOut;					//输出频率设置(Hz)

    float FrA_INV;                   //逆变A相频率
    float FrB_INV;                   //逆变B相频率
    float FrC_INV;                   //逆变C相频率
    Uint16 SOFTSTART;
	ModelState RunState;				//运行状态
	ModelState LastRunState;			//上一次运行状态
	OutputSource OutputSource;			//输出源

	Uint16 (*Init)(void);						//初始化
	Uint16 (*ChangeState)(State state);			//改变逆变器状态
	Uint16 (*RunStateChangeCallback)(void);		//运行状态改变回调函数
}Inverter;

//-------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------
extern Inverter myInverter;

//-------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------
void UseInverter(void);

#endif
