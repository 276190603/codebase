//----------------------------------------------------------------------------
// 文件名：PhaseLock.c
//----------------------------------------------------------------------------
// 功能:  定义了锁相同步模块，该模块计算了旁路频率并给出移相方向
//
//----------------------------------------------------------------------------
// 说明： 使用了ECap2模块
//----------------------------------------------------------------------------
// 引脚定义：GPIO24  ->  旁路A相频率输入
// 芯片：  TMS230F28335
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2016年12月30日
// 修改日期：2017年1月6日
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "PhaseLock.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 PhaseLockInit(void);
Uint16 PhaseLockChangeState(State state);
Uint16 PhaseLockCmd(State state);
Uint16 PhaseLockScan(void);
void PLCapOverFlowCallback(void);
void PLCalOverFlowCallback(void);


//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
PhaseLock myPhaseLock;

Uint16 FlagCapEnd = 0;      //捕获完成标志位
Uint32 ValCap = 0;          //保存捕获到的值
Uint32 ValCap5 = 0;          //保存捕获到的值

//-----------------------------------------------------------------------------
// 函数名：void UsePhaseLock(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myPhaseLock成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//          于对myPhaseLock的部分成员变量进行初始化。使用myPhaseLock之前必须要
//          调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：    无
// 输出参数：    无
//-----------------------------------------------------------------------------
void UsePhaseLock(void)
{
    myPhaseLock.State = Disable;
    myPhaseLock.StatePL = Enable;
    myPhaseLock.PhaseComp = 0.02;//旁路采样滞后大约30度
    myPhaseLock.PSD = NotShift;

    myPhaseLock.Init = PhaseLockInit;
    myPhaseLock.ChangeState = PhaseLockChangeState;
    myPhaseLock.PhaseLockCmd = PhaseLockCmd;
    myPhaseLock.Scan = PhaseLockScan;
    myPhaseLock.CapOverFlowCallback = PLCapOverFlowCallback;
    myPhaseLock.CalOverFlowCallback = PLCalOverFlowCallback;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 PhaseLockInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化锁相模块
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//          于对myControlSystem的部分成员变量进行初始化。使用myControlSystem之前必须要
//          调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：    无
// 输出参数：    0 -> 配置完成，1 -> ECap2已被使用
//-----------------------------------------------------------------------------
Uint16 PhaseLockInit(void)
{
    if(myHardwarePool.bit.ECap2)
        return 1;

    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 1;            //配置为捕获1输入端口
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 1;            //配置为捕获1输入端口
    //使能外设时钟
    SysCtrlRegs.PCLKCR1.bit.ECAP2ENCLK = 1;         //使能ECAP1时钟
    SysCtrlRegs.PCLKCR1.bit.ECAP5ENCLK = 1;         //使能ECAP1时钟
    EDIS;

    //初始化ECAP1
    ECap2Regs.ECCTL1.bit.FREE_SOFT = 0;             //仿真挂起立刻停止
    ECap2Regs.ECCTL1.bit.PRESCALE = 0;              //捕获输入不分配
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;               //使能捕获
    ECap2Regs.ECCTL1.bit.CTRRST1 = 1;               //捕获1清零计数器
    ECap2Regs.ECCTL1.bit.CAP1POL = 0;               //捕获1捕获上升沿
    ECap2Regs.ECCTL1.bit.CTRRST2 = 1;               //捕获2清零计数器
    ECap2Regs.ECCTL1.bit.CAP2POL = 0;               //捕获2捕获上升沿
    ECap2Regs.ECCTL1.bit.CTRRST3 = 1;               //捕获3清零计数器
    ECap2Regs.ECCTL1.bit.CAP3POL = 0;               //捕获3捕获上升沿
    ECap2Regs.ECCTL1.bit.CTRRST4 = 1;               //捕获4清零计数器
    ECap2Regs.ECCTL1.bit.CAP4POL = 0;               //捕获4捕获上升沿

    ECap2Regs.ECCTL2.bit.CAP_APWM = 0;              //CAP模式
    ECap2Regs.ECCTL2.bit.SYNCI_EN = 0;              //禁止同步
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;             //计数器停止
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;           //连续运行模式

    ECap2Regs.ECCLR.all = 0xffff;                   //清除中断标志位

    ECap2Regs.TSCTR = 0;                            //计数器清零

    //初始化ECAP5
        ECap5Regs.ECCTL1.bit.FREE_SOFT = 0;             //仿真挂起立刻停止
        ECap5Regs.ECCTL1.bit.PRESCALE = 0;              //捕获输入不分配
        ECap5Regs.ECCTL1.bit.CAPLDEN = 1;               //使能捕获
        ECap5Regs.ECCTL1.bit.CTRRST1 = 1;               //捕获1清零计数器
        ECap5Regs.ECCTL1.bit.CAP1POL = 0;               //捕获1捕获上升沿
        ECap5Regs.ECCTL1.bit.CTRRST2 = 1;               //捕获2清零计数器
        ECap5Regs.ECCTL1.bit.CAP2POL = 0;               //捕获2捕获上升沿
        ECap5Regs.ECCTL1.bit.CTRRST3 = 1;               //捕获3清零计数器
        ECap5Regs.ECCTL1.bit.CAP3POL = 0;               //捕获3捕获上升沿
        ECap5Regs.ECCTL1.bit.CTRRST4 = 1;               //捕获4清零计数器
        ECap5Regs.ECCTL1.bit.CAP4POL = 0;               //捕获4捕获上升沿

        ECap5Regs.ECCTL2.bit.CAP_APWM = 0;              //CAP模式
        ECap5Regs.ECCTL2.bit.SYNCI_EN = 0;              //禁止同步
        ECap5Regs.ECCTL2.bit.TSCTRSTOP = 0;             //计数器停止
        ECap5Regs.ECCTL2.bit.CONT_ONESHT = 0;           //连续运行模式

        ECap5Regs.ECCLR.all = 0xffff;                   //清除中断标志位

        ECap5Regs.TSCTR = 0;                            //计数器清零

    //中断配置
        ECap2Regs.ECEINT.bit.CEVT1 = 1;                 //捕获寄存器1中断使能
        ECap2Regs.ECEINT.bit.CEVT2 = 1;                 //捕获寄存器2中断使能
        ECap2Regs.ECEINT.bit.CEVT3 = 1;                 //捕获寄存器3中断使能
        ECap2Regs.ECEINT.bit.CEVT4 = 1;                 //捕获寄存器4中断使能
        ECap2Regs.ECEINT.bit.CTROVF = 1;                //计数器溢出中断使能

    ECap5Regs.ECEINT.bit.CEVT1 = 1;                 //捕获寄存器1中断使能
    ECap5Regs.ECEINT.bit.CEVT2 = 1;                 //捕获寄存器2中断使能
    ECap5Regs.ECEINT.bit.CEVT3 = 1;                 //捕获寄存器3中断使能
    ECap5Regs.ECEINT.bit.CEVT4 = 1;                 //捕获寄存器4中断使能
    ECap5Regs.ECEINT.bit.CTROVF = 1;                //计数器溢出中断使能


    PieCtrlRegs.PIEIER4.bit.INTx2 = 1;              //ECAP PIE中断
    PieCtrlRegs.PIEIER4.bit.INTx5 = 1;              //ECAP PIE中断
    IER |= M_INT4;

    return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 PhaseLockChangeState(Uint16 state)
//-----------------------------------------------------------------------------
// 函数功能： 改变锁相模块状态
//-----------------------------------------------------------------------------
// 函数说明：会使用ecap1
//-----------------------------------------------------------------------------
// 输入参数：    state -> 频率检测状态
// 输出参数：    0 -> 配置完成，1 -> ECAP已被使用，2 -> 逆变器未使能,3 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 PhaseLockChangeState(State state)
{
    if((state == Disable)&(myPhaseLock.State==Enable))      //关闭该模块
    {
        ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;             //计数器停止
        ECap2Regs.TSCTR = 0;                            //计数器清零

        myHardwarePool.bit.ECap2 = 0;       //释放定时器资源
        myPhaseLock.State = Disable;
    }
    else if(state == Enable)
    {
        if(myHardwarePool.bit.ECap2)
            return 1;
        if(myScr.State == Disable)
            return 2;           //逆变器未使能

        ECap2Regs.TSCTR = 0;                            //计数器清零
        ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;             //计数器运行

        ECap5Regs.TSCTR = 0;                            //计数器清零
        ECap5Regs.ECCTL2.bit.TSCTRSTOP = 1;             //计数器运行

        myHardwarePool.bit.ECap2 = 1;                       //使用定时器资源
        myPhaseLock.State = state;
    }
    else
        return 3;
    return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 PhaseLockCmd(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变锁相状态
//-----------------------------------------------------------------------------
// 函数说明：改变锁相状态
//-----------------------------------------------------------------------------
// 输入参数：    state -> 频率检测状态
// 输出参数：    0 -> 配置完成，1 -> 未使能锁相模块，2 -> 载波比过低,3 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 PhaseLockCmd(State state)
{
    if(state == Disable)
        myPhaseLock.StatePL = Disable;
    else if(state == Enable)
    {
        if(myPhaseLock.State == Disable)
            return 1;
        if(myControlSystem.Actuator.CarRatio < 277)     //载波比过小不允许使用同步
            return 2;
        myPhaseLock.StatePL = Enable;
    }
    else
        return 3;
    return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 PhaseLockScan(void)
//-----------------------------------------------------------------------------
// 函数功能： 扫描是否有捕获的数据需要处理，该函数必须要在下一次捕获之前执行一次，
//           否则会造成数据丢失。
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：    无
// 输出参数：    0 -> 处理了一次数据，1 -> 没有数据需要处理，2 -> 上次捕获的数据溢出
//-----------------------------------------------------------------------------
Uint16 PhaseLockScan(void)
{
    if(FlagCapEnd == 0)     //没有数据需要处理
    {
        return 1;
    }
    FlagCapEnd = 0;

    if(myPhaseLock.Flag.lastCalOF)      //上次捕获溢出，丢弃数据
    {
        myPhaseLock.Flag.lastCalOF = 0;     //下次捕获溢出清零
        return 2;
    }

    myScr.FrA_BP = 3000000/((ValCap+1)*1.0)*50;                        //计算频率(50Hz对应3000000)
   //myScr.FrA_INV = 3000000/((ValCap5+1)*1.0)*50;                        //计算频率(50Hz对应3000000)
/*
    if(myPhaseLock.StatePL == Enable)   //使能锁相同步
    {
        Uint32 _180P = ValCap/2;            //360/2=180°对应计数值
        Uint32 _2_8P = ValCap>>8;           //360/2^7 = 2.8125°对应计数值

        myPhaseLock.ZeroTimeAct = myPhaseLock.ZeroTimeAct + (Uint32)(ValCap*myPhaseLock.PhaseComp);
        if(myPhaseLock.ZeroTimeAct > ValCap)
            myPhaseLock.ZeroTimeAct = (myPhaseLock.ZeroTimeAct - ValCap);           //考虑相位补偿

        myPhaseLock.BP_INV_Phase = myPhaseLock.ZeroTimeAct;
        if((myPhaseLock.ZeroTimeAct > _180P)&&(myPhaseLock.ZeroTimeAct < ValCap))
            myPhaseLock.PSD = RightShift;               //右移
        else if((myPhaseLock.ZeroTimeAct > _2_8P)&&(myPhaseLock.ZeroTimeAct < _180P))
            myPhaseLock.PSD = LeftShift;                //左移
        else
            myPhaseLock.PSD = NotShift;
    }
    else
        myPhaseLock.PSD = NotShift;                     //不移动
    myPhaseLock.Flag.lastCalOF = 0;                     //数据已经计算
*/
    return 0;
}

void PLCapOverFlowCallback(void)    //捕获溢出回调函数
{}

void PLCalOverFlowCallback(void)    //计算溢出回调函数
{}


//-----------------------------------------------------------------------------
//下面为所用到的对应中断程序
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 函数名：interrupt void ECap2_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能： T2定时器周期中断
//-----------------------------------------------------------------------------
// 函数说明： 当进入此中断时，说明频率捕获超出了量程
//-----------------------------------------------------------------------------
// 输入参数：    无
// 输出参数：    无
//-----------------------------------------------------------------------------
interrupt void ECap2_ISR(void)
{
    if(FlagCapEnd)      //有一次捕获数据没有处理
    {
        myPhaseLock.Flag.lastCalOF = 1;     //上次捕获数据没有处理
        myPhaseLock.Flag.HisCalOF = 1;      //历史上有一次数据没有处理
        myPhaseLock.CalOverFlowCallback();  //有一次数据没有处理回调函数
    }

    if(ECap2Regs.ECFLG.bit.CEVT1)       //有捕获1中断
    {
        ValCap = ECap2Regs.CAP1;
        ECap2Regs.ECCLR.bit.CEVT1 = 1;      //清除标志位
    }
    else if(ECap2Regs.ECFLG.bit.CEVT2)      //有捕获2中断
    {
        ValCap = ECap2Regs.CAP2;
        ECap2Regs.ECCLR.bit.CEVT2 = 1;      //清除标志位
    }
    else if(ECap2Regs.ECFLG.bit.CEVT3)      //有捕获3中断
    {
        ValCap = ECap2Regs.CAP3;
        ECap2Regs.ECCLR.bit.CEVT3 = 1;      //清除标志位
    }
    else if(ECap2Regs.ECFLG.bit.CEVT4)      //有捕获4中断
    {
        ValCap = ECap2Regs.CAP4;
        ECap2Regs.ECCLR.bit.CEVT4 = 1;      //清除标志位
    }
    else if(ECap2Regs.ECFLG.bit.CTROVF)     //计数器溢出
    {
        myPhaseLock.Flag.CurFreOF = 1;      //标记当前频率溢出
        myPhaseLock.Flag.HisFreOF = 1;      //历史上有频率溢出
        myPhaseLock.CapOverFlowCallback();  //捕获溢出回调函数
        ECap2Regs.ECCLR.bit.CTROVF = 1;     //清除标志位
    }

    FlagCapEnd = 1;                         //捕获完成

    //清除中断标志位
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    ECap2Regs.ECCLR.bit.INT = 1;            //清除总标志位
    EINT;
}

interrupt void ECap5_ISR(void)
{
    if(FlagCapEnd)      //有一次捕获数据没有处理
    {
        myPhaseLock.Flag.lastCalOF = 1;     //上次捕获数据没有处理
        myPhaseLock.Flag.HisCalOF = 1;      //历史上有一次数据没有处理
        myPhaseLock.CalOverFlowCallback();  //有一次数据没有处理回调函数
    }

    if(ECap5Regs.ECFLG.bit.CEVT1)       //有捕获1中断
    {
        ValCap5 = ECap5Regs.CAP1;
        ECap5Regs.ECCLR.bit.CEVT1 = 1;      //清除标志位
    }
    else if(ECap5Regs.ECFLG.bit.CEVT2)      //有捕获2中断
    {
        ValCap5 = ECap5Regs.CAP2;
        ECap5Regs.ECCLR.bit.CEVT2 = 1;      //清除标志位
    }
    else if(ECap5Regs.ECFLG.bit.CEVT3)      //有捕获3中断
    {
        ValCap5 = ECap5Regs.CAP3;
        ECap5Regs.ECCLR.bit.CEVT3 = 1;      //清除标志位
    }
    else if(ECap5Regs.ECFLG.bit.CEVT4)      //有捕获4中断
    {
        ValCap5 = ECap5Regs.CAP4;
        ECap5Regs.ECCLR.bit.CEVT4 = 1;      //清除标志位
    }
    else if(ECap5Regs.ECFLG.bit.CTROVF)     //计数器溢出
    {
        myPhaseLock.Flag.CurFreOF = 1;      //标记当前频率溢出
        myPhaseLock.Flag.HisFreOF = 1;      //历史上有频率溢出
        myPhaseLock.CapOverFlowCallback();  //捕获溢出回调函数
        ECap5Regs.ECCLR.bit.CTROVF = 1;     //清除标志位
    }

    //FlagCapEnd = 1;                         //捕获完成

    //清除中断标志位
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    ECap5Regs.ECCLR.bit.INT = 1;            //清除总标志位
    EINT;
}

