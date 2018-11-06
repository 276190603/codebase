//-------------------------------------------------------------
// 文件名： "FpgaDriver.h"
//-------------------------------------------------------------
//功能：	1.包含FpgaDriver.c所用头文件
//		2.进行相关宏定义
//      3.声明全局变量
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2017年1月18日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------
#ifndef FPGADRIVER_H_
#define FPGADRIVER_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "GlobalDefine.h"

//-------------------------------------------------------------
// 宏定义
//-------------------------------------------------------------
#define FPGA_REG0 myFpgaDriver.FpgaRegs.Reg0
#define FPGA_REG1 myFpgaDriver.FpgaRegs.Reg1
#define FPGA_FLAG myFpgaDriver.Flag

//-------------------------------------------------------------
// 结构体声明、枚举声明
//-------------------------------------------------------------

//FPGA寄存器0
typedef struct FpgaReg0Bits		//FPGA寄存器0各个位定义
{								//	bit	description
	Uint16 SPWMEnable:1;		//	0	1->输出驱动使能
	Uint16 InvSwitchOn:1;		//	1	1->逆变器开关吸合
	Uint16 RecInputSW:1;		//	2	1->整流输入跳闸
}FpgaReg0Bits;
typedef union FpgaReg0			//FPGA设置寄存器，只写
{
	Uint16 all;
	FpgaReg0Bits bit;
}FpgaReg0;

//FPGA寄存器1
typedef struct FpgaReg1Bits			//FPGA寄存器1各个位定义
{									//	bit	description
	Uint16 RecSWState:1;			//	0	1->整流输入开关闭合
	Uint16 InvRunState:1;			//	1	1->逆变器运行
	Uint16 NeedTurnToBPState:1;		//	2	1->需要转旁路
	Uint16 TempHighState:1;			//	3	1->温度过高
	Uint16 ArmAState:1;				//	4	1->A相桥臂故障
	Uint16 ArmBState:1;				//	5	1->B相桥臂故障
	Uint16 ArmCState:1;				//	6	1->C相桥臂故障
}FpgaReg1Bits;
typedef union FpgaReg1				//FPGA读取状态寄存器,只读
{
	Uint16 all;
	FpgaReg1Bits bit;
}FpgaReg1;

//FPGA寄存器组
typedef struct FpgaRegs
{
	FpgaReg0 Reg0;
	FpgaReg1 Reg1;
}FpgaRegs;

typedef struct FpgaFlagBits
{							//	bit	description
	Uint16 Enable:1;		//	0	是否使能Fpga驱动
	//IN
	Uint16 RecSW:1;			//	1	整流开关是否闭合 1 -> 闭合
	Uint16 InvRun:1;		//	2	逆变是否运行 1 -> 运行
	Uint16 NeedTurnToBP:1;	//	3	需要转旁路 1 -> 需要转旁路
	Uint16 TempHigh:1;		//	4	温度过高 1 -> 温度过高
	Uint16 ArmAState:1;		//	5	桥臂A错误状态 1 -> 故障
	Uint16 ArmBState:1;		//	6	桥臂B错误状态 1 -> 故障
	Uint16 ArmCState:1;		//	7	桥臂C错误状态 1 -> 故障

	//OUT
	Uint16 SpwmOutEnable:1;	//	8	SPWM输出使能1 -> 使能SPWM输出
	Uint16 InvSwitchOn:1;	//	9	逆变输出接触器控制 1 -> 吸合(和FPGA输出信号相反)
	Uint16 RecInputSW:1;	//	10	整流输入开关跳闸控制 1 -> 开关跳闸

}FpgaFlagBits;

typedef union FpgaFlag
{
	Uint16 all;
	FpgaFlagBits bit;
}FpgaFlag;


typedef struct FpgaDriver
{
	//属性
	FpgaFlag Flag;
	FpgaRegs FpgaRegs;							//FPGA的寄存器，不可手动更改，需要调用相应函数更改

	//方法
	Uint16 (*Init)(void);						//初始化
	Uint16 (*ChangeState)(State state);			//改变状态
	Uint16 (*Scan)(void);						//扫描是否有事件需要处理
	Uint16 (*ReadFpgaRegs)(void);				//读取FPGA所有可读的寄存器

	Uint16 (*ReadEndCB)(void);						//数据读取完毕回调函数
	Uint16 (*RecSW_StChangeCB)(void);				//整流开关状态改变回调函数
	Uint16 (*InvRun_StChangeCB)(void);				//逆变运行状态改变回调函数
	Uint16 (*NeedTurnToBP_StChangeCB)(void);		//需要转旁路状态改变回调函数
	Uint16 (*TempHigh_StChangeCB)(void);			//温度过高状态改变回调函数
	Uint16 (*Arm_StChangeCB)(Uint16 isChange);		//桥臂状态改变回调函数

	Uint16 (*SpwmOutEnable)(State state);		//输出驱动使能
	Uint16 (*InvSwitchOn)(State state);			//逆变器输出吸合
	Uint16 (*RecInputSW)(State state);			//整流输入开关
}FpgaDriver;
//-------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------
extern FpgaDriver myFpgaDriver;

//-------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------
void UseFpgaDriver(void);
interrupt void Xint3_ISR(void);


#endif /* FPGADRIVER_H_ */
