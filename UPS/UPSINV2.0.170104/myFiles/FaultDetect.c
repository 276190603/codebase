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
Uint16 OtherFaultDetScan(void);
Uint16 ComCnt = 1;
Uint16 LastFaultFlag = 0;
Uint16 NowFaultFlag = 0;
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

	//逆变器输出瞬时值范围设置
	myFaultDetect.RangeIns.I_InvOut.minValue = -368;
	myFaultDetect.RangeIns.I_InvOut.maxValue = 368;
	//总输出电流
	myFaultDetect.RangeIns.I_TotalOut.minValue = myFaultDetect.RangeIns.I_InvOut.minValue;
	myFaultDetect.RangeIns.I_TotalOut.maxValue = myFaultDetect.RangeIns.I_InvOut.maxValue;
	//直流母线
	myFaultDetect.RangeIns.U_DC.minValue = 280;
	myFaultDetect.RangeIns.U_DC.maxValue = 420;
	//逆变输出电压
	myFaultDetect.RangeIns.U_InvOut.minValue = -404;
	myFaultDetect.RangeIns.U_InvOut.maxValue = 404;
	//旁路电压
	myFaultDetect.RangeIns.U_BP.minValue = myFaultDetect.RangeIns.U_InvOut.minValue;
	myFaultDetect.RangeIns.U_BP.maxValue = myFaultDetect.RangeIns.U_InvOut.maxValue;

	//逆变器输出有效值范围设置
	myFaultDetect.RangeRms.I_InvOut.minValue = 150;
	myFaultDetect.RangeRms.I_InvOut.maxValue = 220;
	//总输出电流
	myFaultDetect.RangeRms.I_TotalOut.minValue = myFaultDetect.RangeRms.I_InvOut.minValue;
	myFaultDetect.RangeRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_InvOut.maxValue;
	//直流母线
	myFaultDetect.RangeRms.U_DC.minValue = 176;//176
	myFaultDetect.RangeRms.U_DC.maxValue = 264;
	//逆变输出电压
	myFaultDetect.RangeRms.U_InvOut.minValue = 187;//187
	myFaultDetect.RangeRms.U_InvOut.maxValue = 242;
	//旁路电压
	myFaultDetect.RangeRms.U_BP.minValue = myFaultDetect.RangeRms.U_InvOut.minValue;
	myFaultDetect.RangeRms.U_BP.maxValue = myFaultDetect.RangeRms.U_InvOut.maxValue;

	//逆变器输出有效值回窗范围设置
	myFaultDetect.BackWinRms.I_InvOut.minValue = 170;
	myFaultDetect.BackWinRms.I_InvOut.maxValue = 200;
	//总输出电流
	myFaultDetect.BackWinRms.I_TotalOut.minValue = myFaultDetect.RangeRms.I_InvOut.minValue;
	myFaultDetect.BackWinRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_InvOut.maxValue;
	//直流母线
	myFaultDetect.BackWinRms.U_DC.minValue = 310;
	myFaultDetect.BackWinRms.U_DC.maxValue = 410;
	//逆变输出电压
	myFaultDetect.BackWinRms.U_InvOut.minValue = 190;
	myFaultDetect.BackWinRms.U_InvOut.maxValue = 240;
	//旁路电压
	myFaultDetect.BackWinRms.U_BP.minValue = myFaultDetect.RangeRms.U_InvOut.minValue;
	myFaultDetect.BackWinRms.U_BP.maxValue = myFaultDetect.RangeRms.U_InvOut.maxValue;

	//旁路频率范围
	myFaultDetect.Range_FreBP.minValue = 48;//48
	myFaultDetect.Range_FreBP.maxValue = 52;//52
	//旁路频率回窗
	myFaultDetect.BackWin_FreBP.minValue = 49.8;
	myFaultDetect.BackWin_FreBP.maxValue = 50.2;

	//标志位清零
	myFaultDetect.FlagIns.all = 0;
	myFaultDetect.FlagRms.all = 0;
	myFaultDetect.FlagOther.all = 0;

	//方法定位
	RMSFLAGBIT.U_DC = 0;
	myFaultDetect.InsFaultDetAndHandScan = InsFaultDetAndHandScan;
	myFaultDetect.RmsFaultDetectScan = RmsFaultDetectScan;
	myFaultDetect.FaultHandlingScan = FaultHandlingScan;
	myFaultDetect.OtherFaultDetScan = OtherFaultDetScan;
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

	if(SUB_ENABLE_BIT.I_InvOut)		//使能电流检测模块
	{
		if((myInverter.IA_InvOut < I_INVOUT_MIN)|(myInverter.IA_InvOut > I_INVOUT_MAX))
			INSFLAGBIT.IA_InvOut = 1;			//逆变器A相输出瞬时值超量程
		if((myInverter.IB_InvOut < I_INVOUT_MIN)|(myInverter.IB_InvOut > I_INVOUT_MAX))
			INSFLAGBIT.IB_InvOut = 1;			//B相
		if((myInverter.IC_InvOut < I_INVOUT_MIN)|(myInverter.IC_InvOut > I_INVOUT_MAX))
			INSFLAGBIT.IC_InvOut = 1;			//C相
	}
	else
	{
		INSFLAGBIT.IA_InvOut = 0;
		INSFLAGBIT.IB_InvOut = 0;
		INSFLAGBIT.IC_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)	//使能电流总输出模块检测
	{
		if((myInverter.IA_TotalOut < I_TOTOUT_MIN)|(myInverter.IA_InvOut > I_TOTOUT_MAX))
			INSFLAGBIT.IA_InvOut = 1;			//A相总电流输出瞬时值超量程
		if((myInverter.IB_TotalOut < I_TOTOUT_MIN)|(myInverter.IB_InvOut > I_TOTOUT_MAX))
			INSFLAGBIT.IB_InvOut = 1;			//B相
		if((myInverter.IC_TotalOut < I_TOTOUT_MIN)|(myInverter.IC_InvOut > I_TOTOUT_MAX))
			INSFLAGBIT.IC_InvOut = 1;			//C相
	}
	else
	{
		INSFLAGBIT.IA_TotOut = 0;
		INSFLAGBIT.IB_TotOut = 0;
		INSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)		//使能DC母线检测模块
	{
		if((myInverter.U_DC < myFaultDetect.RangeIns.U_DC.minValue)|(myInverter.U_DC > myFaultDetect.RangeIns.U_DC.maxValue))
			INSFLAGBIT.U_DC = 1;			//DC母线超出量程
	}
	else
		INSFLAGBIT.U_DC = 0;

	if(SUB_ENABLE_BIT.U_InvOut)		//使能逆变输出电压检测
	{
		if((myInverter.UA_InvOut < U_INVOUT_MIN)|(myInverter.UA_InvOut > U_INVOUT_MAX))
			INSFLAGBIT.UA_InvOut = 1;			//逆变器A相输出电压瞬时值超量程
		if((myInverter.UB_InvOut < U_INVOUT_MIN)|(myInverter.UB_InvOut > U_INVOUT_MAX))
			INSFLAGBIT.UB_InvOut = 1;			//B相
		if((myInverter.UC_InvOut < U_INVOUT_MIN)|(myInverter.UC_InvOut > U_INVOUT_MAX))
			INSFLAGBIT.UC_InvOut = 1;			//C相
	}
	else
	{
		INSFLAGBIT.UA_InvOut = 0;
		INSFLAGBIT.UB_InvOut = 0;
		INSFLAGBIT.UC_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)		//使能旁路电压输出检测
	{
		if((myInverter.UA_BP < U_BP_MIN)|(myInverter.UA_BP > U_BP_MAX))
			INSFLAGBIT.UA_BP = 1;			//A相旁路输出电压瞬时值超量程
		if((myInverter.UB_BP < U_BP_MIN)|(myInverter.UB_BP > U_BP_MAX))
			INSFLAGBIT.UB_BP = 1;			//B相
		if((myInverter.UC_BP < U_BP_MIN)|(myInverter.UC_BP > U_BP_MAX))
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

Uint16 OtherFaultDetScan(void)         //有效值故障检测
{
    if((myFaultDetect.state == 0)|(myFaultDetect.EnableSubFD.all == 0))     //没有使能总开关或所有子模块都关闭
    {
        myFaultDetect.FlagOther.all = 0;      //清除所有标志位
        return 2;
    }
    if((myInverter.FrA_BP<myFaultDetect.Range_FreBP.minValue)|(myInverter.FrA_BP>myFaultDetect.Range_FreBP.maxValue))
    {
        FlagOtherBIT.FrA_BP = 1;
    }
    else
    {
        FlagOtherBIT.FrA_BP = 0;
    }
        if((myInverter.UA_BPRMS<URMS_BP_MIN)|(myInverter.UA_BPRMS>URMS_BP_MAX))
            FlagOtherBIT.UA_BP = 1;       //旁路A相输出电压有效值超量程
        else
            FlagOtherBIT.UA_BP = 0;
 //   if(myPhaseLock.PSD == NotShift)
  //  {FlagOtherBIT.PSD = 1;}
  //  else
    //{FlagOtherBIT.PSD = 0;}

    return 0;

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

	if(SUB_ENABLE_BIT.I_InvOut)		//使能逆变器电流输出检测
	{
		if((myInverter.IA_InvOutRMS<IRMS_INVOUT_MIN)|(myInverter.IA_InvOutRMS>IRMS_INVOUT_MAX))
			RMSFLAGBIT.IA_InvOut = 1;		//逆变输出A相电流有效值超量程
		//if((myInverter.IB_InvOutRMS<IRMS_INVOUT_MIN)|(myInverter.IB_InvOutRMS>IRMS_INVOUT_MAX))
		//	RMSFLAGBIT.IB_InvOut = 1;		//逆变输出B相电流有效值超量程
		//if((myInverter.IC_InvOutRMS<IRMS_INVOUT_MIN)|(myInverter.IC_InvOutRMS>IRMS_INVOUT_MAX))
		//	RMSFLAGBIT.IC_InvOut = 1;		//逆变输出C相电流有效值超量程
	}
	else
	{
		RMSFLAGBIT.IA_InvOut = 0;
		//RMSFLAGBIT.IB_InvOut = 0;
		//RMSFLAGBIT.IB_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)			//使能总电流输出检测
	{
		if((myInverter.IA_TotalOutRMS<IRMS_TOTOUT_MIN)|(myInverter.IA_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IA_TotOut = 1;		//逆变总输出A相电流有效值超量程
		//if((myInverter.IB_TotalOutRMS<IRMS_TOTOUT_MIN)|(myInverter.IB_TotalOutRMS>IRMS_TOTOUT_MAX))
		//	RMSFLAGBIT.IB_TotOut = 1;		//逆变总输出B相电流有效值超量程
		//if((myInverter.IC_TotalOutRMS<IRMS_TOTOUT_MIN)|(myInverter.IC_TotalOutRMS>IRMS_TOTOUT_MAX))
		//	RMSFLAGBIT.IC_TotOut = 1;		//逆变总输出C相电流有效值超量程
	}
	else
	{
		RMSFLAGBIT.IA_TotOut = 0;
		//RMSFLAGBIT.IB_TotOut = 0;
		//RMSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)			//使能直流母线检测
	{
		if((myInverter.U_DCRMS< IRMS_DC_MIN)|(myInverter.U_DCRMS>IRMS_DC_MAX))
		{
			RMSFLAGBIT.U_DC = 1;		//逆变总输出A相电流有效值超量程
		}
		else
		{
		    RMSFLAGBIT.U_DC = 0;        //逆变总输出A相电流有效值超量程

		}
	}
	else
	{
		RMSFLAGBIT.U_DC = 0;
	}

	if(SUB_ENABLE_BIT.U_InvOut)			//使能逆变输出电压检测
	{
		if((myInverter.UA_InvOutRMS<URMS_INVOUT_MIN)|(myInverter.UA_InvOutRMS>URMS_INVOUT_MAX))
			RMSFLAGBIT.UA_InvOut = 1;		//逆变A相输出电压有效值超量程
		//if((myInverter.UB_InvOutRMS<URMS_INVOUT_MIN)|(myInverter.UB_InvOutRMS>URMS_INVOUT_MAX))
		//	RMSFLAGBIT.UB_InvOut = 1;		//逆变B相输出电压有效值超量程
		//if((myInverter.UC_InvOutRMS<URMS_INVOUT_MIN)|(myInverter.UC_InvOutRMS>URMS_INVOUT_MAX))
		//	RMSFLAGBIT.UC_InvOut = 1;		//逆变C相输出电压有效值超量程
	}
	else
	{
		RMSFLAGBIT.UA_InvOut = 0;
		//RMSFLAGBIT.UB_InvOut = 0;
		//RMSFLAGBIT.UC_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)			//使能旁路输出电压检测
	{
		if((myInverter.UA_BPRMS<URMS_BP_MIN)|(myInverter.UA_BPRMS>URMS_BP_MAX))
			RMSFLAGBIT.UA_BP = 1;		//旁路A相输出电压有效值超量程
		else
		    RMSFLAGBIT.UA_BP = 0;
		//if((myInverter.UB_BPRMS<URMS_BP_MIN)|(myInverter.UB_BPRMS>URMS_BP_MAX))
		//	RMSFLAGBIT.UB_BP = 1;		//旁路B相输出电压有效值超量程
		//if((myInverter.UC_BPRMS<URMS_BP_MIN)|(myInverter.UC_BPRMS>URMS_BP_MAX))
		//	RMSFLAGBIT.UC_BP = 1;		//旁路C相输出电压有效值超量程
	}
	else
	{
		RMSFLAGBIT.UA_BP = 0;
		//RMSFLAGBIT.UB_BP = 0;
		//RMSFLAGBIT.UC_BP = 0;
	}
  //  Uint16 returnValue = 0;
  //  if(myFaultDetect.FlagRms.all)       //是否有错误
   //     returnValue = 1;

	return 0;
}

Uint16 FaultHandlingScan(void)			//故障处理扫描
{
    if(myInverter.U_DCRMS<100)//170
    {

        myInverter.SOFTSTART = 0;
    }
    if(myFaultDetect.FlagOther.all )
    {myPhaseLock.StatePL = Disable;}
    else
    {myPhaseLock.StatePL = Enable;}
    if(SOFT_START == 0)
    {
        //if( ComCnt == 0)
       // {
            NowFaultFlag = myFaultDetect.FlagRms.all ;
             if(LastFaultFlag ^ NowFaultFlag)
             {
                 if(myPhaseLock.PSD_BYP == NoShift)
                 {
                     if(NowFaultFlag)
                     {CAN.SendDataWithPtc2(1,AutoSendMailBox);}//开旁路关逆变
                     else
                     {CAN.SendDataWithPtc2(0,AutoSendMailBox);}//开逆变关旁路
                 }
             }
             else
             {
                 /*
                 if(NowFaultFlag == 0)
                 {
                     if(myPhaseLock.PSD_BYP == NoShift)
                     {
                       if(FPGA_FLAG.bit.InvRun == 0)
                       {CAN.SendDataWithPtc2(0,AutoSendMailBox);} //开逆变关旁路
                     }
                 }
                 else if(NowFaultFlag == 1)
                 {
                     if(FPGA_FLAG.bit.InvRun )
                     {CAN.SendDataWithPtc2(1,AutoSendMailBox);}//开旁路关逆变

                 }
                 */
             }
          // }
             LastFaultFlag = NowFaultFlag;
         }
    /*
    if( ComCnt)
    {
        if(myFaultDetect.FlagOther.all||myFaultDetect.FlagRms.all)
        {
            myInverter.SOFTSTART = 0;
        }
        else
        {
            myInverter.SOFTSTART = 1;
        }
        ComCnt = 0;
    }
    */
    /*
       if( ComCnt)
       {

            if(myFaultDetect.FlagOther.all)
            {
                if(myFaultDetect.FlagRms.all == 0)
                {
                    CAN.SendDataWithPtc2(0,AutoSendMailBox);   //开逆变关旁路
                }
                else
                {
                    CAN.SendDataWithPtc2(1,AutoSendMailBox);   //开旁路关逆变
                }
            }

            if(myFaultDetect.FlagOther.all)
            {
                if(myFaultDetect.FlagRms.all == 0)
                {myFpgaDriver.InvSwitchOn(Enable);}
                else
                {myFpgaDriver.InvSwitchOn(Disable);}
            }

            ComCnt = 0;
    }
*/

	return 0;
}

