//-------------------------------------------------------------
// 文件名： "FaultDetect.h"
//-------------------------------------------------------------
//功能：	1.对故障检测所用到的宏或枚举进行定义
//		2.定义相关结构体
//		3.声明相关函数
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2017年1月3日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------

#ifndef FAULTDETECT_H_
#define FAULTDETECT_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "GlobalDefine.h"
#include "Scr.h"

//-------------------------------------------------------------
// 宏定义
//-------------------------------------------------------------
#define I_SCROUT_MIN myFaultDetect.RangeIns.I_SCROut.mSCRalue			//整流输出电流最小值
#define I_SCROUT_MAX myFaultDetect.RangeIns.I_SCROut.maxValue			//整流输出电流最大值
#define I_TOTOUT_MIN myFaultDetect.RangeIns.I_TotalOut.mSCRalue			//电流总输出最小值
#define I_TOTOUT_MAX myFaultDetect.RangeIns.I_TotalOut.maxValue			//电流总输出最大值
#define U_SCROUT_MIN myFaultDetect.RangeIns.U_SCROut.mSCRalue			//整流输出单元最小值
#define U_SCROUT_MAX myFaultDetect.RangeIns.U_SCROut.maxValue			//整流输出单元最小值
#define U_BP_MIN myFaultDetect.RangeIns.U_BP.mSCRalue					//旁路输出最小值
#define U_BP_MAX myFaultDetect.RangeIns.U_BP.maxValue					//旁路输出最大值

#define IRMS_SCROUT_MIN myFaultDetect.RangeRms.I_SCROut.mSCRalue		//整流输出电流有效值最小值
#define IRMS_SCROUT_MAX myFaultDetect.RangeRms.I_SCROut.maxValue		//整流输出电流有效值最小值
#define IRMS_TOTOUT_MIN myFaultDetect.RangeRms.I_TotalOut.mSCRalue		//总输出电流有效值最小值
#define IRMS_TOTOUT_MAX myFaultDetect.RangeRms.I_TotalOut.maxValue		//总输出电流有效值最大值
#define IRMS_DC_MIN myFaultDetect.RangeRms.U_DC.mSCRalue				//直流母线有效值最小值
#define IRMS_DC_MAX myFaultDetect.RangeRms.U_DC.maxValue				//直流母线有效值最大值
#define URMS_SCROUT_MIN myFaultDetect.RangeRms.U_SCROut.mSCRalue		//整流输出电压有效值最小值
#define URMS_SCROUT_MAX myFaultDetect.RangeRms.U_SCROut.maxValue		//整流输出电压有效值最大值
#define URMS_BP_MIN myFaultDetect.RangeRms.U_BP.mSCRalue				//旁路电压有效值最小值
#define URMS_BP_MAX myFaultDetect.RangeRms.U_BP.maxValue				//旁路电压有效值最大值

#define INSFLAGBIT myFaultDetect.FlagIns.bit							//瞬时值标志位（位域）
#define RMSFLAGBIT myFaultDetect.FlagRms.bit							//有效值标志位（位域）
#define SUB_ENABLE_BIT myFaultDetect.EnableSubFD.bit					//子模块使能（位域）

//-------------------------------------------------------------
// 结构体声明
//-------------------------------------------------------------

typedef struct Range		//表示一个范围
{
	float mSCRalue;
	float maxValue;
}Range;

typedef struct SubFDBit			//子模块故障使能位
{								//	bit		description
	Uint16 I_SCROut:1;			//	0		整流输出电流故障检测使能位
	Uint16 I_TotalOut:1;		//	1		整流总输出电流故障检测使能位
	Uint16 U_DC:1;				//	2		直流母线故障检测使能位
	Uint16 U_SCROut:1;			//	3		整流输出电压故障检测使能位
	Uint16 U_BP:1;				//	4		旁路电压故障使能检测位
	Uint16 Fr_BP:1;				//	5		旁路频率故障检测使能位
	Uint16 rsvd:10;				//	6:15	保留
} SubFDBit;

typedef union SubFD				//子模块故障使能位联合体
{
	Uint16 all;
	SubFDBit bit;
}SubFD;

typedef struct RangeSet			//采样检测范围
{
	Range I_SCROut;				//整流输出电流范围
	Range I_TotalOut;			//总输出电流瞬时值范围
	Range U_DC;					//直流母线输出范围
	Range U_SCROut;				//整流器输出电压范围
	Range U_BP;					//旁路电压范围
}RangeSet;

typedef struct FlagFaultBitSMP		//采样检测标志位
{								//	bit		description
	Uint16 IA_SCROut:1;			//	0		整流器A相输出电流标志位
	Uint16 IB_SCROut:1;			//	1		整流器B相输出电流
	Uint16 IC_SCROut:1;			//	2		整流器C相输出电流
	Uint16 IA_TotOut:1;			//	3		整流器A相总输出电流标志位
	Uint16 IB_TotOut:1;			//	4		B相
	Uint16 IC_TotOut:1;			//	5		C相
	Uint16 U_DC:1;				//	6		直流母线标志位
	Uint16 UA_SCROut:1;			//	7		整流器A相输出电压
	Uint16 UB_SCROut:1;			//	8		B相
	Uint16 UC_SCROut:1;			//	9		C相
	Uint16 UA_BP:1;				//	10		旁路A相电压
	Uint16 UB_BP:1;				//	11		旁路B相电压
	Uint16 UC_BP:1;				//	12		旁路C相电压
	Uint16 rsvd:3;				//	13:15	保留
}FlagFaultBitSMP;

typedef union FlagFaultSMP		//采样检测标志位联合体
{
	Uint16 all;
	FlagFaultBitSMP bit;
}FlagFaultSMP;

typedef struct FaultFlagBit		//其他错误标志位
{								//	bit		description
	Uint16 FrA_BP:1;			//	0		旁路A相频率标志位
	Uint16 FrB_BP:1;			//	1		B相
	Uint16 FrC_BP:1;			//	2		C相
	Uint16 PhaseSeq:1;			//	3		相序错误标志位
	Uint16 rsvd:12;				//	4:15	保留
}FaultFlagBit;

typedef union FaultFlag			//其他错误标志位联合体
{
	Uint16 all;
	FaultFlagBit bit;
}FaultFlag;

typedef struct FaultDetect				//故障检测单元
{
	//属性
	State state;						//故障检测总使能
	SubFD EnableSubFD;					//子故障检测使能
	RangeSet RangeIns;					//瞬时值范围
	RangeSet RangeRms;					//有效值范围
	RangeSet BackWinRms;				//有效值回窗范围
	Range Range_FreBP;					//旁路频率范围
	Range BackWin_FreBP;				//旁路频率回窗
	FlagFaultSMP FlagIns;				//瞬时值故障标志位
	FlagFaultSMP FlagRms;				//有效值故障标志位
	FaultFlag FlagOther;				//其他故障标志位

	//方法
	Uint16 (*InsFaultDetAndHandScan)(void);		//瞬时值故障检测及处理扫描
	Uint16 (*RmsFaultDetectScan)(void);			//有效值故障检测扫描
	Uint16 (*FaultHandlingScan)(void);			//故障处理扫描
} FaultDetect;


//-------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------
extern FaultDetect myFaultDetect;

//-------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------
void UsemyFaultDetect(void);

#endif /* GLOBALDEFINE_H_ */
