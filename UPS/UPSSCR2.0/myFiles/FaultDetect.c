//----------------------------------------------------------------------------
// 文件名：FaultDetect.c
//----------------------------------------------------------------------------
// 功能:  定义故障检测模块
//
//----------------------------------------------------------------------------
// 说明： 无
//----------------------------------------------------------------------------
// 引脚定义：无
// 芯片：	TMS230F2812
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2017年1月3日
// 修改日期：
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "FaultDetect.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 InsFaultDetAndHandScan(void);
Uint16 RmsFaultDetectScan(void);
Uint16 FaultHandlingScan(void);

void InsFaultHandling(void);		//瞬时值错误处理

//-----------------------------------------------------------------------------
//变量声明
//-----------------------------------------------------------------------------
FaultDetect myFaultDetect;


//-----------------------------------------------------------------------------
// 函数名：void UsemyFaultDetect(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myFaultDetect成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//			于对myFaultDetect的部分成员变量进行初始化。使用myFaultDetect之前必须要
//			调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
void UsemyFaultDetect(void)
{
	myFaultDetect.state = Disable;			//默认不使用故障检测
	myFaultDetect.EnableSubFD.all = 0x0;	//关闭所有子模块

	//整流器输出瞬时值范围设置  输入范围190-242
	myFaultDetect.RangeIns.I_SCROut.mSCRalue = -368;
	myFaultDetect.RangeIns.I_SCROut.maxValue = 368;
	//总输出电流
	myFaultDetect.RangeIns.I_TotalOut.mSCRalue = myFaultDetect.RangeIns.I_SCROut.mSCRalue;
	myFaultDetect.RangeIns.I_TotalOut.maxValue = myFaultDetect.RangeIns.I_SCROut.maxValue;
	//直流母线
	myFaultDetect.RangeIns.U_DC.mSCRalue = 280;
	myFaultDetect.RangeIns.U_DC.maxValue = 420;
	//整流输出电压
	myFaultDetect.RangeIns.U_SCROut.mSCRalue = -404;
	myFaultDetect.RangeIns.U_SCROut.maxValue = 404;
	//旁路电压
	myFaultDetect.RangeIns.U_BP.mSCRalue = myFaultDetect.RangeIns.U_SCROut.mSCRalue;
	myFaultDetect.RangeIns.U_BP.maxValue = myFaultDetect.RangeIns.U_SCROut.maxValue;

	//整流器输出有效值范围设置
	myFaultDetect.RangeRms.I_SCROut.mSCRalue = 150;
	myFaultDetect.RangeRms.I_SCROut.maxValue = 220;
	//总输出电流
	myFaultDetect.RangeRms.I_TotalOut.mSCRalue = myFaultDetect.RangeRms.I_SCROut.mSCRalue;
	myFaultDetect.RangeRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_SCROut.maxValue;
	//直流母线
	myFaultDetect.RangeRms.U_DC.mSCRalue = 280;
	myFaultDetect.RangeRms.U_DC.maxValue = 420;
	//整流输出电压
	myFaultDetect.RangeRms.U_SCROut.mSCRalue = 170;
	myFaultDetect.RangeIns.U_SCROut.maxValue = 250;
	//旁路电压
	myFaultDetect.RangeRms.U_BP.mSCRalue = myFaultDetect.RangeRms.U_SCROut.mSCRalue;
	myFaultDetect.RangeRms.U_BP.maxValue = myFaultDetect.RangeRms.U_SCROut.maxValue;

	//整流器输出有效值回窗范围设置
	myFaultDetect.BackWinRms.I_SCROut.mSCRalue = 170;
	myFaultDetect.BackWinRms.I_SCROut.maxValue = 200;
	//总输出电流
	myFaultDetect.BackWinRms.I_TotalOut.mSCRalue = myFaultDetect.RangeRms.I_SCROut.mSCRalue;
	myFaultDetect.BackWinRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_SCROut.maxValue;
	//直流母线
	myFaultDetect.BackWinRms.U_DC.mSCRalue = 310;
	myFaultDetect.BackWinRms.U_DC.maxValue = 410;
	//整流输出电压
	myFaultDetect.BackWinRms.U_SCROut.mSCRalue = 190;
	myFaultDetect.BackWinRms.U_SCROut.maxValue = 240;
	//旁路电压
	myFaultDetect.BackWinRms.U_BP.mSCRalue = myFaultDetect.RangeRms.U_SCROut.mSCRalue;
	myFaultDetect.BackWinRms.U_BP.maxValue = myFaultDetect.RangeRms.U_SCROut.maxValue;

	//旁路频率范围
	myFaultDetect.Range_FreBP.mSCRalue = 49.5;
	myFaultDetect.Range_FreBP.maxValue = 50.5;
	//旁路频率回窗
	myFaultDetect.BackWin_FreBP.mSCRalue = 49.8;
	myFaultDetect.BackWin_FreBP.maxValue = 50.2;

	//标志位清零
	myFaultDetect.FlagIns.all = 0;
	myFaultDetect.FlagRms.all = 0;
	myFaultDetect.FlagOther.all = 0;

	//方法定位
	myFaultDetect.InsFaultDetAndHandScan = InsFaultDetAndHandScan;
	myFaultDetect.RmsFaultDetectScan = RmsFaultDetectScan;
	myFaultDetect.FaultHandlingScan = FaultHandlingScan;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 InsFaultDetAndHandScan(void)
//-----------------------------------------------------------------------------
// 函数功能： 检测瞬时值是否超范围并进行处理
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 没有错误，1 -> 出现错误,2 -> 没有使能故障检测
//-----------------------------------------------------------------------------
Uint16 InsFaultDetAndHandScan(void)		//瞬时故障检测及处理扫描
{
	if((myFaultDetect.state == 0)|(myFaultDetect.EnableSubFD.all == 0))		//没有使能总开关或所有子模块都关闭
	{
		myFaultDetect.FlagIns.all = 0;		//清除所有标志位
		return 2;
	}

	if(SUB_ENABLE_BIT.I_SCROut)		//使能电流检测模块
	{
		if((myScr.IA_SCROut < I_SCROUT_MIN)|(myScr.IA_SCROut > I_SCROUT_MAX))
			INSFLAGBIT.IA_SCROut = 1;			//整流器A相输出瞬时值超量程
		if((myScr.IB_SCROut < I_SCROUT_MIN)|(myScr.IB_SCROut > I_SCROUT_MAX))
			INSFLAGBIT.IB_SCROut = 1;			//B相
		if((myScr.IC_SCROut < I_SCROUT_MIN)|(myScr.IC_SCROut > I_SCROUT_MAX))
			INSFLAGBIT.IC_SCROut = 1;			//C相
	}
	else
	{
		INSFLAGBIT.IA_SCROut = 0;
		INSFLAGBIT.IB_SCROut = 0;
		INSFLAGBIT.IC_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)	//使能电流总输出模块检测
	{
		if((myScr.IA_TotalOut < I_TOTOUT_MIN)|(myScr.IA_SCROut > I_TOTOUT_MAX))
			INSFLAGBIT.IA_SCROut = 1;			//A相总电流输出瞬时值超量程
		if((myScr.IB_TotalOut < I_TOTOUT_MIN)|(myScr.IB_SCROut > I_TOTOUT_MAX))
			INSFLAGBIT.IB_SCROut = 1;			//B相
		if((myScr.IC_TotalOut < I_TOTOUT_MIN)|(myScr.IC_SCROut > I_TOTOUT_MAX))
			INSFLAGBIT.IC_SCROut = 1;			//C相
	}
	else
	{
		INSFLAGBIT.IA_TotOut = 0;
		INSFLAGBIT.IB_TotOut = 0;
		INSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)		//使能DC母线检测模块
	{
		if((myScr.U_DC < myFaultDetect.RangeIns.U_DC.mSCRalue)|(myScr.U_DC > myFaultDetect.RangeIns.U_DC.maxValue))
			INSFLAGBIT.U_DC = 1;			//DC母线超出量程
	}
	else
		INSFLAGBIT.U_DC = 0;

	if(SUB_ENABLE_BIT.U_SCROut)		//使能整流输出电压检测
	{
		if((myScr.UA_SCROut < U_SCROUT_MIN)|(myScr.UA_SCROut > U_SCROUT_MAX))
			INSFLAGBIT.UA_SCROut = 1;			//整流器A相输出电压瞬时值超量程
		if((myScr.UB_SCROut < U_SCROUT_MIN)|(myScr.UB_SCROut > U_SCROUT_MAX))
			INSFLAGBIT.UB_SCROut = 1;			//B相
		if((myScr.UC_SCROut < U_SCROUT_MIN)|(myScr.UC_SCROut > U_SCROUT_MAX))
			INSFLAGBIT.UC_SCROut = 1;			//C相
	}
	else
	{
		INSFLAGBIT.UA_SCROut = 0;
		INSFLAGBIT.UB_SCROut = 0;
		INSFLAGBIT.UC_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)		//使能旁路电压输出检测
	{
		if((myScr.UA_BP < U_BP_MIN)|(myScr.UA_BP > U_BP_MAX))
			INSFLAGBIT.UA_BP = 1;			//A相旁路输出电压瞬时值超量程
		if((myScr.UB_BP < U_BP_MIN)|(myScr.UB_BP > U_BP_MAX))
			INSFLAGBIT.UB_BP = 1;			//B相
		if((myScr.UC_BP < U_BP_MIN)|(myScr.UC_BP > U_BP_MAX))
			INSFLAGBIT.UC_BP = 1;			//C相
	}
	else
	{
		INSFLAGBIT.UA_BP = 0;
		INSFLAGBIT.UB_BP = 0;
		INSFLAGBIT.UC_BP = 0;
	}
	Uint16 returnValue = 0;
	if(myFaultDetect.FlagIns.all)		//是否有错误
		returnValue = 1;

	InsFaultHandling();					//错误处理

	return returnValue;
}

//-----------------------------------------------------------------------------
// 函数名：void InsFaultHandling(void)
//-----------------------------------------------------------------------------
// 函数功能： 该函数直接判断有无瞬时值标志位置位，并进行相应的处理
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
void InsFaultHandling(void)
{}

//-----------------------------------------------------------------------------
// 函数名：Uint16 RmsFaultDetectScan(void)
//-----------------------------------------------------------------------------
// 函数功能： 检测有效值是否超出范围
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 没有错误，1 -> 出现错误,2 -> 没有使能故障检测
//-----------------------------------------------------------------------------
Uint16 RmsFaultDetectScan(void)			//有效值故障检测
{

	if((myFaultDetect.state == 0)|(myFaultDetect.EnableSubFD.all == 0))		//没有使能总开关或所有子模块都关闭
	{
		myFaultDetect.FlagRms.all = 0;		//清除所有标志位
		return 2;
	}

	if(SUB_ENABLE_BIT.I_SCROut)		//使能整流器电流输出检测
	{
		if((myScr.IA_SCROutRMS<IRMS_SCROUT_MIN)|(myScr.IA_SCROutRMS>IRMS_SCROUT_MAX))
			RMSFLAGBIT.IA_SCROut = 1;		//整流输出A相电流有效值超量程
		if((myScr.IB_SCROutRMS<IRMS_SCROUT_MIN)|(myScr.IB_SCROutRMS>IRMS_SCROUT_MAX))
			RMSFLAGBIT.IB_SCROut = 1;		//整流输出B相电流有效值超量程
		if((myScr.IC_SCROutRMS<IRMS_SCROUT_MIN)|(myScr.IC_SCROutRMS>IRMS_SCROUT_MAX))
			RMSFLAGBIT.IC_SCROut = 1;		//整流输出C相电流有效值超量程
	}
	else
	{
		RMSFLAGBIT.IA_SCROut = 0;
		RMSFLAGBIT.IB_SCROut = 0;
		RMSFLAGBIT.IB_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)			//使能总电流输出检测
	{
		if((myScr.IA_TotalOutRMS<IRMS_TOTOUT_MIN)|(myScr.IA_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IA_TotOut = 1;		//整流总输出A相电流有效值超量程
		if((myScr.IB_TotalOutRMS<IRMS_TOTOUT_MIN)|(myScr.IB_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IB_TotOut = 1;		//整流总输出B相电流有效值超量程
		if((myScr.IC_TotalOutRMS<IRMS_TOTOUT_MIN)|(myScr.IC_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IC_TotOut = 1;		//整流总输出C相电流有效值超量程
	}
	else
	{
		RMSFLAGBIT.IA_TotOut = 0;
		RMSFLAGBIT.IB_TotOut = 0;
		RMSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)			//使能直流母线检测
	{
		if((myScr.U_DCRMS< IRMS_DC_MIN)|(myScr.U_DCRMS>IRMS_DC_MAX))
			RMSFLAGBIT.U_DC = 1;		//整流总输出A相电流有效值超量程
	}
	else
	{
		RMSFLAGBIT.U_DC = 0;
	}

	if(SUB_ENABLE_BIT.U_SCROut)			//使能整流输出电压检测
	{
		if((myScr.UA_SCROutRMS<URMS_SCROUT_MIN)|(myScr.UA_SCROutRMS>URMS_SCROUT_MAX))
			RMSFLAGBIT.UA_SCROut = 1;		//整流A相输出电压有效值超量程
		if((myScr.UB_SCROutRMS<URMS_SCROUT_MIN)|(myScr.UB_SCROutRMS>URMS_SCROUT_MAX))
			RMSFLAGBIT.UB_SCROut = 1;		//整流B相输出电压有效值超量程
		if((myScr.UC_SCROutRMS<URMS_SCROUT_MIN)|(myScr.UC_SCROutRMS>URMS_SCROUT_MAX))
			RMSFLAGBIT.UC_SCROut = 1;		//整流C相输出电压有效值超量程
	}
	else
	{
		RMSFLAGBIT.UA_SCROut = 0;
		RMSFLAGBIT.UB_SCROut = 0;
		RMSFLAGBIT.UC_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)			//使能旁路输出电压检测
	{
		if((myScr.UA_BPRMS<URMS_BP_MIN)|(myScr.UA_BPRMS>URMS_BP_MAX))
			RMSFLAGBIT.UA_BP = 1;		//旁路A相输出电压有效值超量程
		if((myScr.UB_BPRMS<URMS_BP_MIN)|(myScr.UB_BPRMS>URMS_BP_MAX))
			RMSFLAGBIT.UB_BP = 1;		//旁路B相输出电压有效值超量程
		if((myScr.UC_BPRMS<URMS_BP_MIN)|(myScr.UC_BPRMS>URMS_BP_MAX))
			RMSFLAGBIT.UC_BP = 1;		//旁路C相输出电压有效值超量程
	}
	else
	{
		RMSFLAGBIT.UA_BP = 0;
		RMSFLAGBIT.UB_BP = 0;
		RMSFLAGBIT.UC_BP = 0;
	}

	return 0;
}

Uint16 FaultHandlingScan(void)			//故障处理扫描
{
	return 0;
}

