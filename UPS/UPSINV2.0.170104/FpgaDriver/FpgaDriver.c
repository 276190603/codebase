//----------------------------------------------------------------------------
// 文件名：FpgaDriver.c
//----------------------------------------------------------------------------
// 功能:  定义了FPGA的驱动程序
//----------------------------------------------------------------------------
// 说明： 使用了外部存储区XZ7
//----------------------------------------------------------------------------
// 引脚定义：	GPIO37		->	FPGA片选
//				GPIO34		->	FPGA给DSP的中断输入INT1
//				GPIO38		->	写使能
//				XRDn		->	读使能
//				GPIO79-72	->	数据总线XD0-7
//				GPIO40-43	->	地址总线XA0-3
// 芯片：	TMS230F28335
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2017年1月18日
// 修改日期：2017年1月6日
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "FpgaDriver.h"
#include "SampleAndControl.h"
//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 FpgaDriverInit(void);
Uint16 FpgaDriverChangeState(State state);
Uint16 FpgaDriverScan(void);
Uint16 ReadFpgaRegs(void);

Uint16 ReadEndCB(void);
Uint16 RecSW_StChangeCB(void);
Uint16 InvRun_StChangeCB(void);
Uint16 NeedTurnToBP_StChangeCB(void);
Uint16 TempHigh_StChangeCB(void);
Uint16 Arm_StChangeCB(Uint16 isChange);

Uint16 FDSpwmOutEnable(State state);
Uint16 FDInvSwitchOn(State state);
Uint16 FDRecInputSW(State state);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
FpgaDriver myFpgaDriver;
Uint16 *FpgaAddr0 = (Uint16*)0x200000;		//Fpga第0个寄存器地址，0-7位分别表示如下：
											//0:驱动输出使能状态	Output	1 -> 使能
											//1:逆变器吸合状态	Output	1 -> 吸合
											//2:整流输入跳闸状态	Output	1 -> 跳闸

Uint16 *FpgaAddr1 = (Uint16*)0x200001;		//0:整流开关状态			Input	1 -> 闭合
											//1:逆变器运行状态		Input	1 -> 运行
											//2:需要转旁路状态		Input	1 -> 需要转
											//3:温度过高状态			Input	1 -> 过高
											//4-6:三相桥臂错误状态	Input	1 -> 错误

Uint16 FpgaIntFlag = 0;						//当有中断需要处理时，此位会置位

//-----------------------------------------------------------------------------
// 函数名：void UseFpgaDriver(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myCommunication中RS485成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//			于对myCommunication的部分成员变量进行初始化。使用myCommunication之前必须要
//			调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
void UseFpgaDriver(void)
{
	myFpgaDriver.Flag.all = 0;		//标志位清除

	myFpgaDriver.Init = FpgaDriverInit;
	myFpgaDriver.ChangeState = FpgaDriverChangeState;
	myFpgaDriver.Scan = FpgaDriverScan;
	myFpgaDriver.ReadFpgaRegs = ReadFpgaRegs;

	myFpgaDriver.ReadEndCB = ReadEndCB;
	myFpgaDriver.RecSW_StChangeCB = RecSW_StChangeCB;
	myFpgaDriver.InvRun_StChangeCB = InvRun_StChangeCB;
	myFpgaDriver.NeedTurnToBP_StChangeCB = NeedTurnToBP_StChangeCB;
	myFpgaDriver.TempHigh_StChangeCB = TempHigh_StChangeCB;
	myFpgaDriver.Arm_StChangeCB = Arm_StChangeCB;

	myFpgaDriver.SpwmOutEnable = FDSpwmOutEnable;
	myFpgaDriver.InvSwitchOn = FDInvSwitchOn;
	myFpgaDriver.RecInputSW = FDRecInputSW;
}

//-----------------------------------------------------------------------------
// 函数名：void FpgaDriverInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化FPGA驱动所用外设
//-----------------------------------------------------------------------------
// 函数说明：所用外设包括XINTF和XINT3(GPIO34),配置外部中断3为上升沿触发
//			配置： GPIO79-72	->	数据总线XD0-7
//				  GPIO40-43	->	地址总线XA0-3
//				  GPIO37 	->	FPGA片选
//				  GPIO38	->	写使能
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 配置完成,1 -> 外部中断3已被配置
//-----------------------------------------------------------------------------
Uint16 FpgaDriverInit(void)		//Fpga驱动初始化
{
	if(myHardwarePool2.bit.Xint3 == 1)
		return 1;
	EALLOW;
	//外部中断配置
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;		//相应端口配置为通用IO
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;			//输入
	GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 1;		//输入滤波采样周期为2*1 Tsysclk(系统时钟)
	GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 1;		//三次采样
	GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;			//使能内部上拉

	GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 34;	//选择34端口为外部中断3中断源

	XIntruptRegs.XINT3CR.bit.POLARITY = 1;		//上升沿触发

	//配置XINTF来驱动FPGA
	SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;		//使能外设时钟

    GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7	配置地址及数据总线
    GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
    GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
    GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
    GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
    GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
    GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
    GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0
    GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
    GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0	写使能（读使能不可复用不用配置）
    GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7	片选

    XintfRegs.XINTCNF2.bit.XTIMCLK = 0;   //XTIMCLK等于SYSCLK
    XintfRegs.XINTCNF2.bit.CLKOFF = 1;    //禁止XCLKOUT时钟输出
    XintfRegs.XINTCNF2.bit.WRBUFF = 0;    //不使用写缓冲

    XintfRegs.XTIMING7.bit.XWRLEAD = 1;   //写建立时间=1 XTIMCLK
    XintfRegs.XTIMING7.bit.XWRACTIVE = 2; //写保持时间=2+1 XTIMCLK
    XintfRegs.XTIMING7.bit.XWRTRAIL = 1;  //写追踪时间=1 XTIMCLK

    XintfRegs.XTIMING7.bit.XRDLEAD = 1;     //读建立时间=1 XTIMCLK
    XintfRegs.XTIMING7.bit.XRDACTIVE = 3;   //读保持时间=3+1 XTIMCLK
    XintfRegs.XTIMING7.bit.XRDTRAIL = 0;    //读追踪时间=0 XTIMCLK

    XintfRegs.XTIMING7.bit.USEREADY = 0;    //不适用XREADY信号
    XintfRegs.XTIMING7.bit.READYMODE = 0;

    XintfRegs.XTIMING7.bit.X2TIMING = 0;    //配置时间不加倍

    XintfRegs.XTIMING7.bit.XSIZE = 3;       //16位位宽访问

    asm(" RPT #7 || NOP");

	EDIS;

	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;			//使能PIE外部中断3
	IER |= M_INT12;								//使能第十二组CPU中断
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 FpgaDriverChangeState(void)
//-----------------------------------------------------------------------------
// 函数功能： 改变FPGA驱动状态
//-----------------------------------------------------------------------------
// 函数说明：在初始化完毕后，即使不调用此函数也能读写FPGA的数据，如果需要使能FPGA给
//			 DSP的中断，则必须要调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 改变完毕, 1 -> 外部中断3被使用,2 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 FpgaDriverChangeState(State state)		//改变FpgaDriver状态
{
	if(state == Enable)
	{
		if(myHardwarePool2.bit.Xint3 == 1)
			return 1;
		EALLOW;
		XIntruptRegs.XINT3CR.bit.ENABLE = 1;		//使能外部中断3
		EDIS;
		myHardwarePool2.bit.Xint3 = 1;
	}
	else if(state == Disable)
	{
		EALLOW;
		XIntruptRegs.XINT3CR.bit.ENABLE = 0;		//禁止外部中断3
		EDIS;

		myHardwarePool2.bit.Xint3 = 0;		//释放硬件组资源
	}
	else
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 FpgaDriverScan(void)
//-----------------------------------------------------------------------------
// 函数功能： 循环扫描有无FPGA的中断需要处理
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 读取了FPGA的状态, 1 -> 没有来自FPGA的中断需要处理
//-----------------------------------------------------------------------------
Uint16 FpgaDriverScan(void)		//扫描有无FpgaDriver事件需要处理
{
	FpgaReg1 FpgaReg1Temp;		//保存前一时刻的FPGA只读寄存器，用来判断和当前值相比是否改变

	if(FpgaIntFlag == 0)
		return 1;
	FpgaIntFlag = 0;

	FpgaReg1Temp.all = FPGA_REG1.all;	//保存之前的状态寄存器1
	FPGA_REG1.all = *FpgaAddr1;			//读取状态寄存器1
	FpgaReg1Temp.all = FPGA_REG1.all^FpgaReg1Temp.all;		//状态改变(不同),对应位为1

	if(FpgaReg1Temp.bit.RecSWState)		//整流开关状态改变
		RecSW_StChangeCB();		//调用回调函数
	if(FpgaReg1Temp.bit.InvRunState)	//逆变运行状态改变
		InvRun_StChangeCB();	//调用回调函数
	if(FpgaReg1Temp.bit.NeedTurnToBPState)	//需要转旁路状态改变
		NeedTurnToBP_StChangeCB();	//调用回调函数
	if(FpgaReg1Temp.bit.TempHighState)	//温度过高状态改变
		TempHigh_StChangeCB();		//调用回调函数
	Uint16 isChange = (FpgaReg1Temp.bit.ArmCState<<2) + (FpgaReg1Temp.bit.ArmBState<<1) + FpgaReg1Temp.bit.ArmAState;
	if(isChange)
		Arm_StChangeCB(isChange);	//调用回调函数

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 ReadFpgaRegs(void)
//-----------------------------------------------------------------------------
// 函数功能： 读取FPGA中所有可读的寄存器并进行判断是否执行相关回调函数
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 读取了相关寄存器
//-----------------------------------------------------------------------------
Uint16 ReadFpgaRegs(void)		//读取FPGA中所有可读的寄存器并进行判断是否执行相关回调函数
{
	FpgaIntFlag = 1;			//标志位置位就会读取寄存器的值
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 ReadEndCB(void)
//-----------------------------------------------------------------------------
// 函数功能： 当扫描函数读取了寄存器值且读取完毕的时候会调用此函数
//-----------------------------------------------------------------------------
// 函数说明: 此函数会在各个状态改变回调函数之后调用
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0
//-----------------------------------------------------------------------------
Uint16 ReadEndCB(void)			//Fpga数据读取完毕回调函数
{
	//用户自定义代码

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 RecSW_StChangeCB(void)
//-----------------------------------------------------------------------------
// 函数功能： 当读到的整流开关状态改变时调用此回调函数
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	1 -> 执行了整流开关闭合代码, 2 -> 执行了整流开关没闭合代码
//-----------------------------------------------------------------------------
Uint16 RecSW_StChangeCB(void)		//整流开关状态改变回调函数
{
	FPGA_FLAG.bit.RecSW = FPGA_REG1.bit.RecSWState;		//更新标志位中的值
	if(FPGA_FLAG.bit.RecSW == 1)		//整流开关闭合
	{
		//整流开关闭合相关代码
		return 1;
	}
	else			//整理开关没有闭合
	{
		//整流开关没有闭合相关代码
		return 2;
	}
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 InvRun_StChangeCB(void)
//-----------------------------------------------------------------------------
// 函数功能： 逆变器运行状态改变时会调用此函数
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	1 -> 执行了逆变器正在运行代码,2 -> 执行了逆变器没有运行代码
//-----------------------------------------------------------------------------
Uint16 InvRun_StChangeCB(void)		//逆变运行状态改变回调函数
{
	FPGA_FLAG.bit.InvRun = FPGA_REG1.bit.InvRunState;	//更新标志位中的值
	if(FPGA_FLAG.bit.InvRun == 1)		//逆变器正在运行
	{
		//逆变器正在运行代码
		return 1;
	}
	else		//逆变器没有运行
	{
		//逆变器没有运行代码
		return 2;
	}
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 NeedTurnToBP_StChangeCB(void)
//-----------------------------------------------------------------------------
// 函数功能： 需要转旁路状态改变时会调用此函数
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	1 -> 执行了需要转旁路代码,2 -> 执行了不需要转旁路代码
//-----------------------------------------------------------------------------
Uint16 NeedTurnToBP_StChangeCB(void)	//需要转旁路回调函数
{
	FPGA_FLAG.bit.NeedTurnToBP = FPGA_REG1.bit.NeedTurnToBPState;		//更新标志位的值
	if(FPGA_FLAG.bit.NeedTurnToBP)		//需要转旁路
	{
		//需要转旁路代码
		return 1;
	}
	else
	{
		//不需要转旁路
		return 2;
	}
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 TempHigh_StChangeCB(void)
//-----------------------------------------------------------------------------
// 函数功能： 温度过高状态改变会调用此函数
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	1 -> 执行了温度过高代码,2 -> 执行了温度没有过高代码
//-----------------------------------------------------------------------------
Uint16 TempHigh_StChangeCB(void)		//温度过高状态改变回调函数
{
	FPGA_FLAG.bit.TempHigh = FPGA_REG1.bit.TempHighState;		//更新标志位的 值
	if(FPGA_FLAG.bit.TempHigh)		//温度过高
	{
		//温度过高代码
		return 1;
	}
	else
	{
		//温度没有过高
		return 2;
	}
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 Arm_StChangeCB(void)
//-----------------------------------------------------------------------------
// 函数功能： 每当三个桥臂错误检测位中的任意一位状态改变时会调用此函数
//-----------------------------------------------------------------------------
// 函数说明: 此回调函数应首先修改myFpgaDriver.Flag中对应位，然后在进行相应处理
//-----------------------------------------------------------------------------
// 输入参数：	isChange -> 0-2位分表表示A,B,C桥臂故障状态是否改变,1 -> 改变
// 输出参数： 	返回值0-2位分别表示桥臂A,B,C.对应值为1表示执行了桥臂故障代码,
//				0表示执行了桥臂正常代码
//-----------------------------------------------------------------------------
Uint16 Arm_StChangeCB(Uint16 isChange)			//桥臂状态改变回调函数
{
	Uint16 reValue = 0;		//返回值

	if(isChange & 0x1)		//桥臂A状态改变
	{
		FPGA_FLAG.bit.ArmAState = FPGA_REG1.bit.ArmAState;
		if(FPGA_FLAG.bit.ArmAState)
		{
			//桥臂A故障代码
		    myFpgaDriver.SpwmOutEnable(Disable);
		   // myControlSystem.Controller.Output = 0;
			reValue |= 1;
		}
		else
		{
			//桥臂A正常代码
            myFpgaDriver.SpwmOutEnable(Enable);
		}
	}
	if(isChange & 0x2)		//桥臂B状态改变
	{
		FPGA_FLAG.bit.ArmBState = FPGA_REG1.bit.ArmBState;
		if(FPGA_FLAG.bit.ArmBState)
		{
			//桥臂B故障代码
			reValue |= 2;
		}
		else
		{
			//桥臂B正常代码
		}
	}
	if(isChange & 0x4)		//桥臂C状态改变
	{
		FPGA_FLAG.bit.ArmCState = FPGA_REG1.bit.ArmCState;
		if(FPGA_FLAG.bit.ArmCState)
		{
			//桥臂C故障代码
            myFpgaDriver.SpwmOutEnable(Disable);
           // myControlSystem.Controller.Output = 0;
			reValue |= 4;
		}
		else
		{
			//桥臂C正常代码
            myFpgaDriver.SpwmOutEnable(Enable);
		}
	}
	return reValue;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 FDSpwmOutEnable(State state)
//-----------------------------------------------------------------------------
// 函数功能： 将SPWM输出使能的状态进行更改并写入到FPGA中
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	state -> Enable,Disable
// 输出参数： 	0 -> 进行了相关处理,1 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 FDSpwmOutEnable(State state)		//SPWM输出使能
{
	if(state == Enable)
	{
		FPGA_FLAG.bit.SpwmOutEnable = 1;	//改变标志位
		FPGA_REG0.bit.SPWMEnable = 1;		//改变寄存器的值
	}
	else if(state == Disable)
	{
		FPGA_FLAG.bit.SpwmOutEnable = 0;
		FPGA_REG0.bit.SPWMEnable = 0;
	}
	else
		return 1;
	*FpgaAddr0 = FPGA_REG0.all;		//将修改写入到FPGA中
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 FDInvSwitchOn(State state)
//-----------------------------------------------------------------------------
// 函数功能： 将逆变器输出吸合状态改变并写入FPGA中
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	state -> Enable,Disable
// 输出参数： 	0 -> 进行了相关处理,1 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 FDInvSwitchOn(State state)		//逆变器输出吸合
{
	if(state == Enable)
	{
		FPGA_FLAG.bit.InvSwitchOn = 1;	//更新标志位的值
		FPGA_REG0.bit.InvSwitchOn = 1;	//更新寄存器的值
	}
	else if(state == Disable)
	{
		FPGA_FLAG.bit.InvSwitchOn = 0;
		FPGA_REG0.bit.InvSwitchOn = 0;
	}
	else
		return 1;

	*FpgaAddr0 = FPGA_REG0.all;		//将寄存器值写入FPGA中
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 FDRecInputSW(State state)
//-----------------------------------------------------------------------------
// 函数功能： 将整流输入开关状态改变并写入FPGA中
//-----------------------------------------------------------------------------
// 函数说明：此回调函数应首先将myFpgaDriver.Flag对应位进行修改，然后再进行相应的处理
//-----------------------------------------------------------------------------
// 输入参数：	state -> Enable,Disable
// 输出参数： 	0 -> 进行了相关处理,1 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 FDRecInputSW(State state)			//整流输入开关
{
	if(state == Enable)
	{
		FPGA_FLAG.bit.RecInputSW = 1;		//改变标志位的值
		FPGA_REG0.bit.RecInputSW = 1;		//改变寄存器的值
	}
	else if(state == Disable)
	{
		FPGA_FLAG.bit.RecInputSW = 0;
		FPGA_REG0.bit.RecInputSW = 0;
	}

	*FpgaAddr0 = FPGA_REG0.all;			//将寄存器值写入FPGA中
	return 0;
}


//-----------------------------------------------------------------------------
//下面为所用到的对应中断程序
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 函数名：interrupt void Xint3_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能： 外部中断3
//-----------------------------------------------------------------------------
// 函数说明： 当进入此中断时，说明FPGA请求DSP读取其数据
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void Xint3_ISR(void)
{
	FpgaIntFlag = 1;	//FPGA发出读取请求

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;	//响应PIE同组中断
}
