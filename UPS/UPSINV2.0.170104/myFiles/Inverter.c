//----------------------------------------------------------------------------
// 文件名：Inverter.c
//----------------------------------------------------------------------------
// 功能:  定义了逆变器模块，该模块相当于数据库保存了逆变器的大部分数据（除故障数据外）
//
//----------------------------------------------------------------------------
// 说明：
//----------------------------------------------------------------------------
// 引脚定义：未使用任何引脚
// 芯片：	TMS230F2812
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2016年12月27日
// 修改日期：
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "Inverter.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 InverterInit(void);
Uint16 InverterChangeState(State state);
Uint16 InvRSCC(void);
//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
Inverter myInverter;

//-----------------------------------------------------------------------------
// 函数名：void UseInverter(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myInverter成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//			于对myInverter的部分成员变量进行初始化。使用myInverter之前必须要
//			调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
void UseInverter(void)
{
	myInverter.Init = InverterInit;
	myInverter.ChangeState = InverterChangeState;
	myInverter.RunStateChangeCallback = InvRSCC;

	//调用其他模块的对象使用函数
	UseControlSystem();			//控制系统
	UsePhaseLock();				//相位锁存
	UsemyFaultDetect();			//错误检测
	UsemyCommunication();		//通讯模块
	UseSystemTick();			//使用滴答时钟
	UseFpgaDriver();			//使用FPGA模块
}

//-----------------------------------------------------------------------------
// 函数名：void InverterInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 逆变器初始化
//-----------------------------------------------------------------------------
// 函数说明：除了初始化逆变器模块的变量外，还会调用逆变器相关模块的初始化
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 初始化成功,1 -> 控制系统初始化失败,2 -> 锁相初始化失败
//				3 -> 通讯模块初始化失败,4 -> 滴答时钟初始化失败,5 -> FPGA驱动初始化失败
//-----------------------------------------------------------------------------
Uint16 InverterInit(void)
{
	Uint16 reValue = 0;

	myInverter.FrOut = 50;					//输出频率=50Hz
	myInverter.RunState = Stop;
	myInverter.LastRunState = Stop;
	myInverter.OutputSource = BP;			//默认旁路输出

	reValue = myControlSystem.Init();		//控制系统初始化
	if(reValue)
		return 1;
	reValue = myPhaseLock.Init();			//锁相初始化
	if(reValue)
		return 2;
	myFaultDetect.state = Disable;			//禁止故障检测
	//myPhaseLock.PhaseLockCmd (Enable);
	reValue = myCommunication.Init();		//初始化通讯模块
	if(reValue)
		return 3;
	if(mySystemTick.Init())							//初始化滴答时钟
		return 4;
	if(myFpgaDriver.Init())					//初始FPGA驱动
	{
		return 5;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 InverterChangeState(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变逆变器状态
//-----------------------------------------------------------------------------
// 函数说明：除了逆变器状态以外，所有子模块的状态也会改变
//-----------------------------------------------------------------------------
// 输入参数：	state -> 需要改变的状态
// 输出参数： 	0 -> 改变成功,1 -> 输入参数错误,2 -> 采样状态改变失败,3 -> 执行机构状态改变失败
//				4 -> 锁相状态改变错误, 5 -> 通讯模块改变失败,6 -> 滴答时钟初始化失败
//				7 -> FPGA驱动初始化失败
//-----------------------------------------------------------------------------
Uint16 InverterChangeState(State state)
{
	Uint16 reValue = 0;
	if(state == Enable)
		myInverter.State = Enable;
	else if(state == Disable)
		myInverter.State = Disable;
	else
		return 1;
	reValue = myControlSystem.Sample.ChangeState(state);		//改变采样状态
	if(reValue)
		return 2;
	reValue = myControlSystem.Actuator.ChangeState(state);		//改变执行机构状态
	if(reValue)
		return 3;
	reValue = myPhaseLock.ChangeState(state);					//改变锁相状态(只是能频率检测，同步需要单独使能)
	if(reValue)
		return 4;
	reValue = myCommunication.ChangeState(state);				//改变通讯状态
	if(reValue)
		return 5;
	if(mySystemTick.ChangeState(state))							//改变滴答时钟状态
		return 6;
	if(myFpgaDriver.ChangeState(state))
		return 7;
	myFaultDetect.state = state;
	myFaultDetect.EnableSubFD.bit.U_DC = 1;
	//myFaultDetect.EnableSubFD.bit.U_BP = 1;
	//myFaultDetect.EnableSubFD.bit.Fr_BP = 1;
	return 0;
}

Uint16 InvRSCC(void)
{
	return 0;
}
