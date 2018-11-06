//-------------------------------------------------------------
// 文件名： "SampleAndControl.h"
//-------------------------------------------------------------
//功能：	1.对全局所用到的宏或枚举进行定义
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2016年12月20日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------

#ifndef GLOBALDEFINE_H_
#define GLOBALDEFINE_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define HSPCLK28xx 75000		//高速外设时钟(k)
#define LSPCLK28xx 37500		//低速时钟外设(k)
#define PI 3.141593
#define _2PI 6.283185

//-------------------------------------------------------------
// 结构体、联合体、枚举声明
//-------------------------------------------------------------
typedef enum State
{
	Disable = 0,
	Enable
} State;

typedef enum Bool
{
	False = 0,
	True
} Bool;

typedef enum ModelState
{
	Stop = 0
	,Inited
	,Run
	,Waring
	,Fault
}ModelState;

typedef enum PhaseShiftDir
{
	NotShift = 0,
	LeftShift,
	RightShift,
	NoneShift,
	NoShift
}PhaseShiftDir;

typedef struct HardwarePool1Bits		//如果对应的硬件资源被使用则置位1，否则为0
{							//	bit	description
	Uint16 CpuTimer0:1;		//	0	Cpu定时器0
	Uint16 CpuTimer1:1;		//	1	Cpu定时器1
	Uint16 CpuTimer2:1;		//	2	Cpu定时器2
	Uint16 EPWM1:1;			//	3	EPWM1
	Uint16 EPWM2:1;			//	4	EPWM2
	Uint16 EPWM3:1;			//	5	EPWM3
	Uint16 EPWM4:1;			//	6	EPWM4
	Uint16 EPWM5:1;			//	7	EPWM5
	Uint16 EPWM6:1;			//	8	EPWM6
	Uint16 Adc:1;			//	9	ADC
	Uint16 ECap1:1;			//	10	ECap1
	Uint16 ECap2:1;			//	11	ECap2
	Uint16 ECap3:1;			//	12	ECap3
	Uint16 ECap4:1;			//	13	ECap4
	Uint16 ECap5:1;			//	14	ECap4
	Uint16 ECap6:1;			//	15	ECap4
}HardwarePool1Bits;

typedef union HardwarePool1
{
	Uint16 all;
	HardwarePool1Bits bit;
}HardwarePool1;

typedef struct HardwarePool2Bits			//硬件资源池2
{								//	bit	description
	Uint16 Scia:1;				//	0	SCIA模块
	Uint16 Scib:1;				//	1	SCIB模块
	Uint16 Scic:1;				//	2	SCIC
	Uint16 Cana:1;				//	3	CANA
	Uint16 Canb:1;				//	4	CANB
	Uint16 XInt1:1;				//	5	外部中断1
	Uint16 Xint2:1;				//	6	外部中断2
	Uint16 Xint3:1;				//	7	外部中断3
	Uint16 Xint4:1;				//	8	外部中断4
	Uint16 Xint5:1;				//	9	外部中断5
	Uint16 Xint6:1;				//	10	外部中断6
	Uint16 Xint7:1;				//	11	外部中断7
	Uint16 Xnmi:1;				//	12	外部不可屏蔽中断
}HardwarePool2Bits;

typedef union HardwarePool2
{
	Uint16 all;
	HardwarePool2Bits bit;
}HardwarePool2;

//-------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------
extern HardwarePool1 myHardwarePool;
extern HardwarePool2 myHardwarePool2;

//-------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------
void ResetHardwarePool(void);

#endif /* GLOBALDEFINE_H_ */
