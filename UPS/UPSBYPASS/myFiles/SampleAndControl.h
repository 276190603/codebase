//-------------------------------------------------------------
// 文件名： "SampleAndControl.h"
//-------------------------------------------------------------
//功能：	1.包含SampleAndControl.c所用头文件
//		2.进行相关宏定义
//      3.声明全局变量
//-------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期：2016年12月20日
// 修改日期：
// 版本：v0.1
//-------------------------------------------------------------

#ifndef SAMPLEANDCONTROL_H_
#define SAMPLEANDCONTROL_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "GlobalDefine.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "math.h"
#include "Scr.h"
#include "PhaseLock.h"
#include "communication.h"

//-------------------------------------------------------------

//-------------------------------------------------------------
// 宏定义
//-------------------------------------------------------------
//变量宏定义
#define MAXPHA 0.1736482			//10°对应的sin值
#define MINPHA -0.1736482			//-10°对应的sin值
#define SQRT2 1.414214				//根号2
#define SQRT2_SCR 0.70710678		//根号2倒数
#define SAMPLE_NUM 30				//这里设置一个周期的采样点数，必须要和采样比相同

#define C_KP myControlSystem.Controller.ControlKp			//比例系数
#define I_KP myControlSystem.Controller.ControlKp_I           //比例系数
#define C_TARGET myControlSystem.Controller.Target			//控制器目标值
#define C_ERROR myControlSystem.Controller.Error			//控制器误差值
#define C_WEIGHT myControlSystem.Controller.WeightErr		//控制器误差权值
#define C_OUTPUT myControlSystem.Controller.Output			//控制器输出

#define SAMPLE_FRE myControlSystem.Sample.SampleRate		//采样频率
#define SAMPLE_OV myControlSystem.Sample.OriginalValue		//采样原始值
#define SAMPLE_K myControlSystem.Sample.K_Sample			//采样比例系数
#define SAMPLE_B myControlSystem.Sample.B_Sample			//采样截距系数

#define DIV_FAC EPwm4Regs.TBCTL.bit.CLKDIV						//采样时钟分频系数 sample_clk = HSPCLK/(2^DIV_FAC)


#define scr_1 0x0001
#define scr_2 0x0002
#define scr_3 0x0003
#define scr_4 0x0004
#define scr_5 0x0005
#define scr_6 0x0006

#define HIGH  0x0001
#define LOW   0x0000

//-------------------------------------------------------------
// 结构体声明
//-------------------------------------------------------------

typedef struct Controller        //控制实例
{
	//属性
	State State;						//控制使能
	float Target;						//给定值
	float ControlKp;					//控制器比例系数
	float ControlKi;					//控制器积分系数
    float ControlKp_I;                    //控制器积分系数
	float Error[7];						//控制器误差e(k-i) = Error[i];(i=0,1,2)
	float WeightErr[3];					//误差权值，用来进行加权均值滤波（必须保证权值都大于零且和为1）
	float InsUA_Out;					//保存当前UA的瞬时有效值
	float Output;						//控制器输出
	//Y(k)=ControlKp*(U(k)-U(k-1)) + ControlKi*Ts*U(k)

	//方法
	Uint16 (*IsAddControl)(void);				//判断是否可以投入控制器，并改变其状态
	Uint16 (*Control)(void);					//控制过程
}Controller;

typedef struct SoftStart		//软起实例
{
	//属性
	float SS_Target;					//软启动（soft-start）目标值（控制器输出）
	Uint16 SS_Stairs;					//软启动阶梯数
	Uint16 SS_Time;						//软启动总共用时(ms)

	//方法
	Uint16 (*Run)(void);					//软启动
}SoftStart;

typedef struct Sample			//采样实例
{
	//属性
	State State;						//采样（sample and control）状态
	float SampleRate;					//采样频率（Hz）
	float SampleRatio;					//采样比
	Bool FlagRMS;						//有效值计算标志位(通过调用CalRMSCmd函数改变，不可直接修改)
	Uint16 SampleFlag;					//采样标志位，每次采样中断完毕0位反转，1位置位
										//					瞬时值更新2位反转，3位置位
										//					有效值更新4位反转，5位置位

	Uint16 OriginalValue[16];			//保存原始采样寄存器值（右对齐）
	float K_Sample[16];					//采样比例系数
	float B_Sample[16];					//采样截距
	//实际值[i]=K_Sample[i]*OriginalValue[i]+B_Sample[i]

	//方法
	Uint16 (*Init)(void);							//采样控制初始化
	Uint16 (*ChangeState)(State state);				//改变采样状态（启动或停止）
	Uint16 (*Scan)(void);							//用于在主程序循环扫描，在一个采样周期内该函数必须
													//被执行至少一次，否则会丢失采样结果
	Uint16 (*CalRMSCmd)(Bool cmd);					//改变有效值计算状态
} Sample;

typedef struct Actuator			//执行机构
{
	//属性
	State State;				//执行机构状态
	float CarFre;				//载波频率(kHz)
	float CarRatio;				//载波比
	float DBTime;				//死区时间(0-13.5)us

	//方法
	Uint16 (*Init)(void);		//执行机构初始化
	Uint16 (*ChangeState)(State state);		//改变状态
	float (*GetOutputPhase)(void);			//获取输出相位
} Actuator;

typedef struct ControlSystem		//控制系统
{
	Controller Controller;
	Sample Sample;
	SoftStart SoftStart;
	Actuator Actuator;

	//方法
	Uint16 (*Init)(void);		//执行机构初始化
}ControlSystem;

//-------------------------------------------------------------
// 全局变量声明
//-------------------------------------------------------------
extern ControlSystem myControlSystem;

//extern unsigned int SOFT_START;
extern unsigned int SOFT_START;
extern unsigned int FIRST_START;
extern unsigned int DC_Target;
extern float PID;
extern unsigned int TIMER1_num;
extern unsigned int muxian_count;
void PID_change(float target);
void soft_start(void);
void scr_number_switch(unsigned int scr_number,unsigned int High_Low);
//-------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------
void UseControlSystem(void);

interrupt void ADC_ISR(void);
interrupt void Epwm1_ISR(void);
interrupt void CpuTimer1_ISR(void);

#endif /* SAMPLEANDCONTROL_H_ */
