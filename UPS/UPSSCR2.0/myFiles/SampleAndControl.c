//----------------------------------------------------------------------------
// 文件名：SampleAndControl.c
//----------------------------------------------------------------------------
// 功能:  该文件定义了控制系统中控制器、采样模块、执行机构和软启动模块
//
//----------------------------------------------------------------------------
// 说明： 用到的硬件外设有ADC、Epwm1-4,定时器1
//----------------------------------------------------------------------------
// 引脚定义：
//			ADCINA0-7	->	A通道输入
//			ADCINB0-7	->	B通道输入
//			GPIO0-5		->	PWM驱动输出端口(单相只用4路)
// 芯片：	TMS230F28335
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2017年1月4日
// 修改日期：
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "SampleAndControl.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 IsAddControl(void);
Uint16 Control(void);
float CalRMS(float insValue,float* fifo,int* ptr,float* uSa,float* curPh);

Uint16 SACInit(void);
Uint16 ChangeSampleState(State state);
Uint16 SAC_Scan(void);
Uint16 CalRMSCmd(Bool cmd);

Uint16 SoftStartRun(void);

Uint16 ActuatorInit(void);
Uint16 ActuatorChangeState(State state);
float ActuatorGetOutputPhase(void);

Uint16 ControlSystemInit(void);

void scr_number_switch(unsigned int scr_number,unsigned int High_Low);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
ControlSystem myControlSystem;

float KiTs = 0;			//Ki*Ts
int16 SampleN;			//一个周期采样点数（采样比取整）
int iCalRMS = 0;		//计算有效值当前统计采样点数
int iCalRMS1 = 0;        //计算有效值当前统计采样点数
float SamplePhase = 0;	//采样时刻输出的相位
int16 RemainStep = 0;	//软启动剩余步数
float OutPerStep = 0;	//软启动输出步长
float PhasePerStep;		//每次执行机构输出移动的电角度(弧度0-2π)
float CurPhase;			//执行机构当前输出的电角度

float COPRD = 0;		//控制器输出乘以周期值

//带遗忘因子的有效值计算

float SNumRec = 1.0/SAMPLE_NUM;		//采样点数的倒数
float DeltaPh = _2PI/SAMPLE_NUM;	//两个采样间隔经过的相位

float UaFifo[SAMPLE_NUM];	//用于使用FIFIO法计算有效值的数组
int UaPtr = 0;				//指向下一个需要写入的Fifo位置
float UaSa = 0;				//保存方均值
float UaPh = 0;				//保存当前时刻Ua的瞬时值相位

float UA[150];			//用于测试时保存输出采样的数组
float UAi[150];			//保存瞬时有效值

Uint16 I_KiEnable = 0;
Uint16 Ki_Enable = 0;
unsigned int SOFT_START=1;
float PID=180;
unsigned int TIMER1_num=0;
float Kp=0,Ki=0.08,Kd=0.09,T=0.014,Ti=0.025,Td=0.005,Kpo=0.6,Kio=0,Kdo=0,Kp1=2,Ki1=1,Ki2=1.5,Kd1=2;
//float Kp=0,Ki=0,Kd=0,T=0.014,Ti=0.025,Td=0.005,Kpo=0.175,Kio=0,Kdo=0,Kp1=2,Ki1=1,Ki2=1.5,Kd1=2;
float ek=0,ek1=0,ek2=0;
float ek_sum=0;
unsigned int Target=0;
unsigned int muxian_count=0;
unsigned int DC_Target=221;
unsigned int DC_SoftH_Target=200;
unsigned int DC_SoftL_Target=170;
float OUT0 = 0;
float UDC = 0;
//-----------------------------------------------------------------------------
// 函数名：void UseControlSystem(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myControlSystem成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//			于对myControlSystem的部分成员变量进行初始化。使用myControlSystem之前必须要
//			调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
 void UseControlSystem(void)
{
	//变量初始化
	//属性初始化

	//控制模块
	myControlSystem.Controller.ControlKi = 0.39;	//积分系数：0
	myControlSystem.Controller.ControlKp = 0.0025;	//比例系数：1
    myControlSystem.Controller.ControlKp_I = 0.005;   //比例系数：1
	myControlSystem.Controller.Error[0] = 0;	//e(k) = 0
	myControlSystem.Controller.Error[1] = 0;	//e(k-1) = 0
	myControlSystem.Controller.Error[2] = 0;	//e(k-2) = 0
    myControlSystem.Controller.Error[3] = 0;    //e(k-2) = 0
    myControlSystem.Controller.Error[4] = 0;    //e(k-2) = 0
	myControlSystem.Controller.WeightErr[0] = 1;//0.5714286;	//e(k)时刻权值
	myControlSystem.Controller.WeightErr[1] = 0;//0.2857143;	//e(k-1)时刻权值
	myControlSystem.Controller.WeightErr[2] = 0;//0.1428571;	//e(k-2)时刻权值，必须保证和为1
	myControlSystem.Controller.Output = 0.7;	//输出调制度：0.7
	myControlSystem.Controller.State = Disable;	//禁止控制器
	myControlSystem.Controller.Target = 221;	//目标值：220

	//软启动模块
	myControlSystem.SoftStart.SS_Stairs = 4;	//分4个阶段启动
	myControlSystem.SoftStart.SS_Target = 0.7;	//启动后控制器输出：0.7
	myControlSystem.SoftStart.SS_Time = 1000;	//软启动用时：1000 ms

	//采样模块
	myControlSystem.Sample.K_Sample[0] = 0;			//A0斜率
	myControlSystem.Sample.B_Sample[0] = 0;			//A0截距

	myControlSystem.Sample.K_Sample[1] = 0.00194465;			//A1斜率
	myControlSystem.Sample.B_Sample[1] = -4.35740715;			//A1截距

	myControlSystem.Sample.K_Sample[2] = 0.00194465;			//A2斜率
	myControlSystem.Sample.B_Sample[2] = -4.35740715;			//A2截距

	myControlSystem.Sample.K_Sample[3] = 0.00194465*75;			//A3斜率
	myControlSystem.Sample.B_Sample[3] = -4.35740715*75;			//A3截距

	myControlSystem.Sample.K_Sample[4] = 0.00194465*75;			//A4斜率
	myControlSystem.Sample.B_Sample[4] = -4.35740715*75;			//A4截距

	myControlSystem.Sample.K_Sample[5] = 0.00194465;			//A5斜率
	myControlSystem.Sample.B_Sample[5] = -4.35740715;			//A5截距

	myControlSystem.Sample.K_Sample[6] = 0.00194465*75;			//A6斜率
	myControlSystem.Sample.B_Sample[6] = -4.35740715*75;			//A6截距

	myControlSystem.Sample.K_Sample[7] = 0;			//A7斜率
	myControlSystem.Sample.B_Sample[7] = 0;			//A7截距

	myControlSystem.Sample.K_Sample[8] = 0.0658;		//B0斜率(直流母线参数)
	myControlSystem.Sample.B_Sample[8] = 1.5748;	//B0截距

	myControlSystem.Sample.K_Sample[9] = 0.2133;			//B1斜率
	myControlSystem.Sample.B_Sample[9] = -444.0385;			//B1截距()

	myControlSystem.Sample.K_Sample[10] = 0.2133;		//B2斜率
	myControlSystem.Sample.B_Sample[10] = -444.0385;		//B2截距()

	myControlSystem.Sample.K_Sample[11] = 0.2133;		//B4斜率
	myControlSystem.Sample.B_Sample[11] = -444.0385;		//B4截距	()

	myControlSystem.Sample.K_Sample[12] = 0.2462;				//B3斜率(UC_SCR)
	myControlSystem.Sample.B_Sample[12] = -514.6332;				//B3截距

	myControlSystem.Sample.K_Sample[13] = 0.2462;		//B5斜率(UA_SCR)
	myControlSystem.Sample.B_Sample[13] = -514.6332;		//B5截距

	myControlSystem.Sample.K_Sample[14] = 0.2462;		//B6斜率(UB_SCR)
	myControlSystem.Sample.B_Sample[14] = -514.6332;		//B6截距

	myControlSystem.Sample.K_Sample[15] = 0;		//B7斜率
	myControlSystem.Sample.B_Sample[15] = 0;		//B7截距

	myControlSystem.Sample.FlagRMS = False;			//不计算有效值
	myControlSystem.Sample.SampleFlag = 0;			//标志位清零
	myControlSystem.Sample.SampleRate = 1500;		//采样频率：1500 Hz
	myControlSystem.Sample.State = Disable;			//禁止采样模块

	//执行机构
	myControlSystem.Actuator.CarFre = 20;		//载波：20k
	myControlSystem.Actuator.DBTime = 3.5;		//死区：0us
	myControlSystem.Actuator.State = Disable;	//禁止执行机构

	//函数定位
	myControlSystem.Init = ControlSystemInit;

	myControlSystem.Controller.Control = Control;
	myControlSystem.Controller.IsAddControl = IsAddControl;

	myControlSystem.Sample.Init = SACInit;
	myControlSystem.Sample.ChangeState = ChangeSampleState;
	myControlSystem.Sample.Scan = SAC_Scan;
	myControlSystem.Sample.CalRMSCmd = CalRMSCmd;

	myControlSystem.SoftStart.Run = SoftStartRun;

	myControlSystem.Actuator.Init = ActuatorInit;
	myControlSystem.Actuator.ChangeState = ActuatorChangeState;
	myControlSystem.Actuator.GetOutputPhase = ActuatorGetOutputPhase;

	int i = 0;
	for(i = 0;i < SAMPLE_NUM;i++)	//初始化数组
		UaFifo[i] = 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 ControlSystemInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 控制系统初始化
//-----------------------------------------------------------------------------
// 函数说明：初始化控制系统所需硬件，包括采样ADC和执行机构EPWM
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 控制系统初始化成功， 1 -> 控制系统初始化失败
//-----------------------------------------------------------------------------
Uint16 ControlSystemInit(void)
{
	Uint16 returnValue = 0;

	returnValue = SACInit();
	if(returnValue)
		return 1;
	returnValue = ActuatorInit();
	if(returnValue)
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 IsAddControl(void)
//-----------------------------------------------------------------------------
// 函数功能： 判断当前状态是否可以投入控制器
//-----------------------------------------------------------------------------
// 函数说明：当满足条件时自动投入控制器
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 控制器状态改变了， 1 -> 控制器状态没改变
//-----------------------------------------------------------------------------
Uint16 IsAddControl(void)				//判断是否可以投入控制器，并改变其状态
{
	State temp = myControlSystem.Controller.State;

	if((myScr.U_DCRMS >= 190)&(myScr.U_DCRMS <= 254))	//输出值在窗口内开始调节
		myControlSystem.Controller.State = Enable;

	else
		myControlSystem.Controller.State = Disable;
         C_ERROR[3] = myScr.IA_SCROutRMS;
         C_ERROR[4] = C_ERROR[3];
	   // myControlSystem.Controller.Output = 0.1;

	if(temp == myControlSystem.Controller.State)
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 Control(void)
//-----------------------------------------------------------------------------
// 函数功能： 控制器调节
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 正常调节，4 -> 执行机构未运行， 第0位 -> 是否禁止了积分环节，第1位 -> 是否在过零点附近
//-----------------------------------------------------------------------------
Uint16 Control(void)						//控制过程
{
	int16 returnValue = 0;		//返回值
	int16 _KiEnable = 1;		//积分使能位

	if(myControlSystem.Actuator.State == Disable)		//执行机构未运行
		return 4;

	static int ui = 0;

	myControlSystem.Controller.InsUA_Out = CalRMS(myScr.UA_SCROut,UaFifo,&UaPtr,&UaSa,&UaPh);	//计算瞬时有效值

	UA[ui] = myScr.UA_SCROut;
	UAi[ui] = myControlSystem.Controller.InsUA_Out;
	ui++;
	if(ui >= 150)
		ui = 0;

	//C_ERROR[0] = C_TARGET - myControlSystem.Controller.InsUA_Out;

	C_ERROR[0] = C_TARGET - myScr.UA_SCROutRMS;
	C_ERROR[0] = C_WEIGHT[0]*C_ERROR[0] + C_WEIGHT[1]*C_ERROR[1] + C_WEIGHT[2]*C_ERROR[2];	//加权滤波
	C_ERROR[3] = myScr.IA_SCROutRMS;
	//C_ERROR[5] =  C_ERROR[3] - C_ERROR[4];

	if((C_ERROR[0] <= -100)|(C_ERROR[0] >= 100))		//误差值过大，禁止积分
	{
		_KiEnable = 0;
		returnValue |= 1;
	}
	else if((C_OUTPUT >= 0.95)|(C_OUTPUT <= 0.05))		//输出饱和，禁止积分
	{
		_KiEnable = 0;
		returnValue |= 1;
	}
	else
		_KiEnable = 1;
	float _deltaOut = C_KP*(C_ERROR[0]-C_ERROR[1]) + _KiEnable*KiTs*C_ERROR[0] + I_KP*(C_ERROR[3]-C_ERROR[4]);
    //float _deltaOut = I_KP*(C_ERROR[3]-C_ERROR[4]) ;
	C_OUTPUT = C_OUTPUT + _deltaOut;

	if(C_OUTPUT >= 0.95)			//输出幅值限制
		C_OUTPUT = 0.95;
	else if(C_OUTPUT <= 0.05)
		C_OUTPUT = 0.05;

	COPRD = C_OUTPUT*EPwm1Regs.TBPRD;		//输出和周期值相乘用于快速计算
	//COPRD = C_OUTPUT*EPwm1Regs.TBPRD;       //输出和周期值相乘用于快速计算

	C_ERROR[1] = C_ERROR[0];
	C_ERROR[2] = C_ERROR[1];		//误差更新
	C_ERROR[4] = C_ERROR[3];
	//C_ERROR[6] = C_ERROR[5];
	return returnValue;
}

//-----------------------------------------------------------------------------
// 函数名：float CalRMS(float insValue,float* fifo,int* ptr)
//-----------------------------------------------------------------------------
// 函数功能： 采用带遗忘因子算法计算有效值
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	insValue -> 瞬时值
//				fifo -> 对应的fifo的指针
//				ptr -> 指向fifo中下一个需要写入的位置的指针
//				uSa -> 方均值的指针
//
//				curPh -> 当前采样值的相位
// 输出参数： 	有效值
//-----------------------------------------------------------------------------
float CalRMS(float insValue,float* fifo,int* ptr,float* uSa,float* curPh)
{
	float lastU = 0;		//前一时刻的采样值
	float uKi = 0;
	float sinPh = 0;

	//根据FIFO方均根算法计算方均根
	if(*ptr < 0)
	{
		*ptr = SAMPLE_NUM - 1;
		lastU = fifo[0];
	}
	else
		lastU = fifo[*ptr+1];
	*uSa = *uSa + (insValue*insValue-fifo[*ptr]*fifo[*ptr])*SNumRec;
	fifo[*ptr] = insValue;
	float uKh = sqrt(*uSa);

	//根据瞬时值计算有效值 DeltaPh
	if((lastU < 0)&(insValue >= 0))
		*curPh = DeltaPh*insValue/(insValue - lastU);
	else
		*curPh = *curPh + DeltaPh;
	if(*curPh == 0)
		uKi = 0;
	else
	{
		sinPh = sin(*curPh);
		uKi = insValue*SQRT2_SCR/sinPh;
		if(uKi > 2*uKh)
			uKi = 2*uKh;
		else if(uKi < -2*uKh)
			uKi = -2*uKh;
	}
	(*ptr)--;		//指向下一个待写位置

	//加上遗忘因子
	if(sinPh < 0)
		sinPh = -sinPh;
	return (1-sinPh)*uKh+sinPh*uKi;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 SACInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 对采样控制模块进行初始化
//-----------------------------------------------------------------------------
// 函数说明： 1.对ADC模块进行初始化
//			 2.根据采样频率对Epwm4初始化
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 配置成功,1 -> 采样频率过高,2 -> 采样频率过低,3 -> 硬件资源已被使用
//-----------------------------------------------------------------------------
Uint16 SACInit(void)						//采样控制初始化
{
	if((myHardwarePool.bit.Adc)|(myHardwarePool.bit.EPWM4))
		return 3;

	if(SAMPLE_FRE > 5722.05)		//计算过程见ParameterCalculation.m中采样控制段
		return 1;					//采样频率过高
	else if(SAMPLE_FRE < 9.93)
		return 2;				//采样频率过低

	//禁止外设时钟
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;		// ADC
    SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;  	// ePWM4
	EDIS;

	//ADC配置
	InitAdc();									//使能时钟并上电

	AdcRegs.ADCTRL1.bit.SUSMOD = 1;				//仿真挂起模式：转换完成后排序器和其他逻辑停止工作，最终结果锁存，状态机更新
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xf;			//采样窗口大小，SOC脉冲宽度为16个ADCLK	SampleTime = 1/ADCCLK*ACQ_PS = 3.2us
	AdcRegs.ADCTRL1.bit.CPS=1;			//核时钟预定标器，等于1，将时钟进行2分频
	AdcRegs.ADCTRL1.bit.CONT_RUN=0;		//运行于启动/停止模式
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1;		//单序列发生器模式


	AdcRegs.ADCTRL3.bit.ADCCLKPS=0x4;	//ADCCLK=HSPCLK/(4×2×(CPS+1))=75/16=4.6875M
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;	//顺序采样

	AdcRegs.ADCMAXCONV.bit.MAX_CONV1=0xf;	//总共采样16路


	AdcRegs.ADCCHSELSEQ1.bit.CONV00=0x0; 	//采样ADCINA0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01=0x1; 	//采样ADCINA1
	AdcRegs.ADCCHSELSEQ1.bit.CONV02=0x2; 	//采样ADCINA2
	AdcRegs.ADCCHSELSEQ1.bit.CONV03=0x3; 	//采样ADCINA3
	AdcRegs.ADCCHSELSEQ2.bit.CONV04=0x4; 	//采样ADCINA4
	AdcRegs.ADCCHSELSEQ2.bit.CONV05=0x5; 	//采样ADCINA5
	AdcRegs.ADCCHSELSEQ2.bit.CONV06=0x6;	//采样ADCINA6
	AdcRegs.ADCCHSELSEQ2.bit.CONV07=0x7;	//采样ADCINA7
	AdcRegs.ADCCHSELSEQ3.bit.CONV08=0x8;	//采样ADCINB0
	AdcRegs.ADCCHSELSEQ3.bit.CONV09=0x9; 	//采样ADCINB1
	AdcRegs.ADCCHSELSEQ3.bit.CONV10=0xa; 	//采样ADCINB2
	AdcRegs.ADCCHSELSEQ3.bit.CONV11=0xb; 	//采样ADCINB3
	AdcRegs.ADCCHSELSEQ4.bit.CONV12=0xc; 	//采样ADCINB4
	AdcRegs.ADCCHSELSEQ4.bit.CONV13=0xd; 	//采样ADCINB5
	AdcRegs.ADCCHSELSEQ4.bit.CONV14=0xe; 	//采样ADCINB6
	AdcRegs.ADCCHSELSEQ4.bit.CONV15=0xf;	//采样ADCINB7

	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;		//清除SEQ1中的中断标志位INT_SEQ1
	AdcRegs.ADCST.bit.INT_SEQ2_CLR=1;		//清除SEQ2中的中断标志位INT_SEQ2

	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;		//使能SEQ1中断
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ=1;	//允许EPWM启动触发SEQ

	//ADC中断配置
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;		//使能ADC PIE中断
	IER |= M_INT1;

	//EPWM4的配置
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 1;		//模拟2812中的HSPCLK
	//这里通过合适的分频系数将周期寄存器的值设定在40%(26214)-80%(52429)之间
	if(SAMPLE_FRE >= 715.26)
		DIV_FAC = 1;				//2分频
	else if(SAMPLE_FRE >= 357.63)
		DIV_FAC = 2;				//4分频
	else if(SAMPLE_FRE >= 178.81)
		DIV_FAC = 3;				//8分频
	else if(SAMPLE_FRE >= 89.41)
		DIV_FAC = 4;				//16分频
	else if(SAMPLE_FRE >= 44.70)
		DIV_FAC = 5;				//32分频
	else if(SAMPLE_FRE >= 22.35)
		DIV_FAC = 6;				//64分频
	else
		DIV_FAC = 7;				//128分频

	myControlSystem.Sample.SampleRatio = SAMPLE_FRE/myScr.FrOut;	//计算采样比
	SampleN = (int16)myControlSystem.Sample.SampleRatio;	//一个周期采样点数
	KiTs = myControlSystem.Controller.ControlKi/myControlSystem.Sample.SampleRate;		//Ki*Ts

	EPwm4Regs.TBCTL.bit.SYNCOSEL = 0x3;		//禁止同步输出信号

	EPwm4Regs.ETSEL.bit.SOCBEN = 1;			//使能epwm4触发adc
	EPwm4Regs.ETSEL.bit.SOCBSEL =2;			//周期事件触发adc
	EPwm4Regs.ETPS.bit.SOCBPRD = 1;			//每次事件都触发
	EPwm4Regs.ETCLR.bit.SOCA = 1;			//清除标志位

	EPwm4Regs.TBPRD = (Uint16)(HSPCLK28xx/SAMPLE_FRE/(1<<DIV_FAC)*1000 - 1);
	EPwm4Regs.TBCTR = 0;					//计数值清零

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：int16 ChangeSampleState(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变采样模块状态
//-----------------------------------------------------------------------------
// 函数说明： 使能(禁止)epwm4定时器
//-----------------------------------------------------------------------------
// 输入参数：	state -> 采样模块状态
// 输出参数： 	0 -> 配置成功,1 -> 输入参数错误，2 -> 整流器未使能,3 -> 硬件资源已被使用
//-----------------------------------------------------------------------------
Uint16 ChangeSampleState(State state)	//改变采样状态（启动或停止）
{
	if(state == Enable)
	{
		if(myScr.State == Disable)
			return 2;			//整流器未使能
		if((myHardwarePool.bit.Adc)|(myHardwarePool.bit.EPWM4))
			return 3;
		EPwm4Regs.TBCTR = 0;					//计数值清零
		EPwm4Regs.TBCTL.bit.CTRMODE = 0;		//计数器开始向上计数

		myHardwarePool.bit.EPWM4 = 1;
		myHardwarePool.bit.Adc = 1;
		myControlSystem.Sample.State = Enable;
	}
	else if((state == Disable)&(myControlSystem.Sample.State == Enable))
	{
		EPwm4Regs.TBCTL.bit.CTRMODE = 3;		//停止计数器
		EPwm4Regs.TBCTR = 0;					//计数值清零

		myHardwarePool.bit.EPWM4 = 0;
		myHardwarePool.bit.Adc = 0;
		myControlSystem.Sample.State = Disable;
	}
	else
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：int16 SAC_Scan(void)
//-----------------------------------------------------------------------------
// 函数功能： 循环扫描是否有adc转换后的数据需要处理，如果有则进行处理
//-----------------------------------------------------------------------------
// 函数说明： 1.该模块必须在下一次采样中断之前执行一次，否则会丢失上次采样数据
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 进行了一次数据处理,1 -> 没有新数据采集,返回
//-----------------------------------------------------------------------------
Uint16 SAC_Scan(void)						//用于在主程序循环扫描，在一个采样周期内该函数必须执行至少一次，否则会丢失采样结果
{
	//static float sumIASCROut = 0;
	//static float sumIBSCROut = 0;
	//static float sumICSCROut = 0;
	//static float sumIATotalOut = 0;
	//static float sumIBTotalOut = 0;
	//static float sumICTotalOut = 0;
	static float sumUDC = 0;
	static float sumUDC1 = 0;
	//static float sumUASCROut = 0;
	//static float sumUBSCROut = 0;
	//static float sumUCSCROut = 0;
	static float sumUABP = 0;
	static float sumUBBP = 0;
	static float sumUCBP = 0;

	if(!(myControlSystem.Sample.SampleFlag&0x2))		//没有数据更新，返回
	{
		return 1;
	}
	myControlSystem.Sample.SampleFlag &= ~(1<<1);		//第1位清零
/*
	myScr.IA_SCROut = SAMPLE_OV[6]*SAMPLE_K[6]+SAMPLE_B[6];		//A6	整流器输出电流
	myScr.IB_SCROut = SAMPLE_OV[4]*SAMPLE_K[4]+SAMPLE_B[4];		//A4
	myScr.IC_SCROut = SAMPLE_OV[3]*SAMPLE_K[3]+SAMPLE_B[3];		//A3

	myScr.IB_TotalOut = SAMPLE_OV[2]*SAMPLE_K[2]+SAMPLE_B[2];		//A2	总输出电流
	myScr.IA_TotalOut = SAMPLE_OV[1]*SAMPLE_K[1]+SAMPLE_B[1];		//A1
	myScr.IC_TotalOut = SAMPLE_OV[5]*SAMPLE_K[5]+SAMPLE_B[5];		//A5
*/
	myScr.U_DC = SAMPLE_OV[8]*SAMPLE_K[8]+SAMPLE_B[8];				//A0	直流母线
/*
	myScr.UA_SCROut = -(SAMPLE_OV[11]*SAMPLE_K[11]+SAMPLE_B[11]);		//B4	整流器输出电压
	myScr.UB_SCROut = -(SAMPLE_OV[10]*SAMPLE_K[10]+SAMPLE_B[10]);		//B2
	myScr.UC_SCROut = -(SAMPLE_OV[9]*SAMPLE_K[9]+SAMPLE_B[9]);		//B1
*/
	myScr.UC_BP = -(SAMPLE_OV[12]*SAMPLE_K[12]+SAMPLE_B[12]);		//B3		旁路电压
	myScr.UB_BP = -(SAMPLE_OV[14]*SAMPLE_K[14]+SAMPLE_B[14]);		//B6
	myScr.UA_BP = -(SAMPLE_OV[13]*SAMPLE_K[13]+SAMPLE_B[13]);		//B5

	myControlSystem.Sample.SampleFlag = myControlSystem.Sample.SampleFlag^0x4;		//采样标志第2位取反
	myControlSystem.Sample.SampleFlag |= 0x8;										//采样标志第3位置位

     //  if(SOFT_START==0)
     //  {
     //      PID_change(DC_Target);//220
     // }
   // if(myControlSystem.Controller.State == Enable)          //如果使能控制器，则进行控制
    //    myControlSystem.Controller.Control();

	if(myControlSystem.Sample.FlagRMS)		//使能有效值计算
	{

	    if(iCalRMS1 < 8)
	    {
	        sumUDC1 += myScr.U_DC;
	        iCalRMS1++;
	    }
	    else
	    {
	        //float temp1 = 0;
	        UDC = sumUDC1/8;
           // myScr.U_DCRMS = sqrt(temp1);                    //DC母线电压有效值
            //sumUDC1 = 0;
	        if(myScr.SOFTSTART==1)
	        {
            if(SOFT_START==0)
            {
                PID_change(DC_Target);//220
            }
	        }
            sumUDC1 = 0;
            iCalRMS1 = 0;
	    }

		if(iCalRMS < SampleN)		//是否完成一个周期平方和计算
		{
		//	sumIASCROut += myScr.IA_SCROut*myScr.IA_SCROut;
		//	sumIBSCROut += myScr.IB_SCROut*myScr.IB_SCROut;
		//	sumICSCROut += myScr.IC_SCROut*myScr.IC_SCROut;

		//	sumIATotalOut += myScr.IA_TotalOut*myScr.IA_TotalOut;
		//	sumIBTotalOut += myScr.IB_TotalOut*myScr.IB_TotalOut;
		//	sumICTotalOut += myScr.IC_TotalOut*myScr.IC_TotalOut;

			sumUDC += myScr.U_DC*myScr.U_DC;

			//sumUASCROut += myScr.UA_SCROut*myScr.UA_SCROut;
			//sumUBSCROut += myScr.UB_SCROut*myScr.UB_SCROut;
			//sumUCSCROut += myScr.UC_SCROut*myScr.UC_SCROut;

			sumUABP += myScr.UA_BP*myScr.UA_BP;
			sumUBBP += myScr.UB_BP*myScr.UB_BP;
			sumUCBP += myScr.UC_BP*myScr.UC_BP;
		    iCalRMS++;
		}
		else
		{
			float temp1 = 0;
/*
			temp1 = sumIASCROut/SampleN;						//sum^2/n
			myScr.IA_SCROutRMS = sqrt(temp1);				//整流输出电流A相有效值
			sumIASCROut = 0;

			temp1 = sumIBSCROut/SampleN;						//sum^2/n
			myScr.IB_SCROutRMS = sqrt(temp1);				//整流输出电流B相有效值
			sumIBSCROut = 0;

			temp1 = sumICSCROut/SampleN;						//sum^2/n
			myScr.IC_SCROutRMS = sqrt(temp1);				//整流输出电流C相有效值
			sumICSCROut = 0;

			temp1 = sumIATotalOut/SampleN;
			myScr.IA_TotalOutRMS = sqrt(temp1);			//整流总输出电流A相有效值
			sumIATotalOut = 0;

			temp1 = sumIBTotalOut/SampleN;
			myScr.IB_TotalOutRMS = sqrt(temp1);			//整流总输出电流B相有效值
			sumIBTotalOut = 0;

			temp1 = sumICTotalOut/SampleN;
			myScr.IC_TotalOutRMS = sqrt(temp1);			//整流总输出电流C相有效值
			sumICTotalOut = 0;
*/
			temp1 = sumUDC/SampleN;
			myScr.U_DCRMS = sqrt(temp1);					//DC母线电压有效值
			sumUDC = 0;
/*
			temp1 = sumUASCROut/SampleN;
			myScr.UA_SCROutRMS = sqrt(temp1);				//整流输出A相电压有效值
			sumUASCROut = 0;

			temp1 = sumUBSCROut/SampleN;
			myScr.UB_SCROutRMS = sqrt(temp1);				//整流输出B相电压有效值
			sumUBSCROut = 0;

			temp1 = sumUCSCROut/SampleN;
			myScr.UC_SCROutRMS = sqrt(temp1);				//整流输出C相电压有效值
			sumUCSCROut = 0;
*/
			temp1 = sumUABP/SampleN;
			myScr.UA_BPRMS = 1.8162*sqrt(temp1)-1.5045;					//旁路A相电压有效值
			sumUABP = 0;

			temp1 = sumUBBP/SampleN;
			myScr.UB_BPRMS = 1.7259*sqrt(temp1)-2.4048;					//旁路B相电压有效值
			sumUBBP = 0;

			temp1 = sumUCBP/SampleN;
			myScr.UC_BPRMS = 1.7817*sqrt(temp1)+2.0284;					//旁路C相电压有效值
			sumUCBP = 0;
			if(myScr.SOFTSTART==1)
			{
            if(SOFT_START==1)
            {
                soft_start();
            }
          //  else if(SOFT_START==0)
          // {
          //     PID_change(DC_Target);//220
          //  }
            if(myScr.U_DC<DC_SoftL_Target)//170
            {
               SOFT_START=1;
            }
			}
               // */
			//myControlSystem.Controller.IsAddControl();
		    //if(myControlSystem.Controller.State == Enable)          //如果使能控制器，则进行控制
		       // myControlSystem.Controller.Control();

			iCalRMS = 0;
			myControlSystem.Sample.SampleFlag = myControlSystem.Sample.SampleFlag^0x10;		//采样标志第4位取反
			myControlSystem.Sample.SampleFlag |= 0x20;										//采样标志第5位置位
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 CalRMSCmd(Bool cmd)
//-----------------------------------------------------------------------------
// 函数功能： 使能(禁止)有效值计算
//-----------------------------------------------------------------------------
// 函数说明： 无
//-----------------------------------------------------------------------------
// 输入参数：	cmd -> 是否使用有效值计算
// 输出参数： 	0 -> 配置完成,1 -> 采样比过小,2 -> 采样比过大
//				4 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 CalRMSCmd(Bool cmd)
{
	if(cmd == True)
	{
		if(myControlSystem.Sample.SampleRatio < 4)		//采样比过小不计算有效值
		{
			myControlSystem.Sample.FlagRMS = False;
			return 1;
		}
		else if(myControlSystem.Sample.SampleRatio > 60)		//采样比过大不计算有效值
		{
			myControlSystem.Sample.FlagRMS = False;
			return 2;
		}
		else
			myControlSystem.Sample.FlagRMS = True;
	}
	else if(cmd == False)
	{
		iCalRMS = 0;		//计算有效值统计采样数清零
	}
	else
		return 4;

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 SoftStartRun(void)
//-----------------------------------------------------------------------------
// 函数功能： 软启动
//-----------------------------------------------------------------------------
// 函数说明： 1.需要使用CPU定时器1
//			 2.软起过程中不会投入控制器，软起后控制器状态和软起前一致
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 软起完成,1 -> 定时器1已被使用,2 -> 当前值和目标值距离过近
//				3 -> 设定值超出范围
//-----------------------------------------------------------------------------
Uint16 SoftStartRun(void)					//软启动
{
	float deltaUout = myControlSystem.SoftStart.SS_Target - C_OUTPUT;
	if((deltaUout > -0.1)&(deltaUout < 0.1))		//当前值和目标值距离过近不用软起
		return 2;
	if((myControlSystem.SoftStart.SS_Target<0)|(myControlSystem.SoftStart.SS_Target>1))
		return 3;
	if(myHardwarePool.bit.CpuTimer1)		//Cpu定时器1已被使用
		return 1;
	myHardwarePool.bit.CpuTimer1 = 1;		//Cpu定时器1正在被使用

	Uint16 _TperStep = myControlSystem.SoftStart.SS_Time/myControlSystem.SoftStart.SS_Stairs;	//ms
	ConfigCpuTimer(&CpuTimer1,150,_TperStep*1000.0);
	RemainStep = myControlSystem.SoftStart.SS_Stairs;		//时间步长
	OutPerStep = deltaUout/myControlSystem.SoftStart.SS_Stairs;	//输出步长
	XIntruptRegs.XNMICR.bit.SELECT = 0;			//Timer1连到int13

	IER |= M_INT13; 		/*定时器1开中断*/
	StartCpuTimer1();		//启动定时器1

	while(RemainStep > 0)				//等待软启动完成
	{
		myControlSystem.Sample.Scan();	//软起等待过程中会进行采样
		myPhaseLock.Scan();				//允许进行锁相
	}
	StopCpuTimer1();					//停止定时器1
	myHardwarePool.bit.CpuTimer1 = 0;		//Cpu定时器1使用完毕
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：int16 ActuatorInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 对执行机构进行初始化
//-----------------------------------------------------------------------------
// 函数说明： 1.配置GPIO0-3端口为PWM功能口
//			 2.配置Epwm1,2外设
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 配置完成,1 -> 载波频率过高,2 -> 载波频率过低,3 -> 死区时间过大
//				4 -> EMPW已被占用
//-----------------------------------------------------------------------------
Uint16 ActuatorInit(void)					//执行机构初始化
{
   EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;         //将GPIO0-3设置为spwm功能口
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;         //将GPIO0-3设置为spwm功能口
	EDIS;
    /*if(myControlSystem.Actuator.CarFre >25)
		return 1;
	if(myControlSystem.Actuator.CarFre <10)
		return 2;
	if((myControlSystem.Actuator.DBTime >= 13.5)|(myControlSystem.Actuator.DBTime<0))
		return 3;
	if((myHardwarePool.bit.EPWM1)&(myHardwarePool.bit.EPWM2))
		return 4;

	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;			//将GPIO0-3设置为spwm功能口
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;			//将GPIO0-3设置为spwm功能口
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;			//将GPIO0-3设置为spwm功能口
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;			//将GPIO0-3设置为spwm功能口

	//使能外设时钟
	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
	EDIS;

	//Epwm1配置
	//时基单元设置
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;
	EPwm1Regs.TBCTL.bit.CLKDIV = 0;		//时钟不分频，Epwm1Clk = HSPCLK = 75000k
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 0x01;	//ctr=0时，输出同步信号
	EPwm1Regs.TBPRD = (Uint16)(HSPCLK28xx/myControlSystem.Actuator.CarFre/2);		//设置周期寄存器值
	EPwm1Regs.TBCTR = 0;			
	//比较单元配置
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0;		//使能阴影寄存器并在cnt=0时重装载
	EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = 0;		//使能阴影寄存器并在cnt=0时重装载
	//动作单元配置
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;		//A匹配之间为高
	//死区单元配置
	EPwm1Regs.DBCTL.bit.IN_MODE = 0;		//S(4,5)=(0,0)
	EPwm1Regs.DBCTL.bit.POLSEL = 2;			//S(2,3)=(0,1)
	EPwm1Regs.DBCTL.bit.OUT_MODE = 3;		//S(0,1)=(1,1)
	EPwm1Regs.DBRED = (Uint16)(myControlSystem.Actuator.DBTime*75);
	EPwm1Regs.DBFED = EPwm1Regs.DBRED;		//设置死区时间
	//斩波模块配置
	EPwm1Regs.PCCTL.bit.CHPEN = 0;			//禁止PWM斩波
	//事件触发模块
	EPwm1Regs.ETSEL.bit.INTEN = 1;			//使能EPWM中断
	EPwm1Regs.ETSEL.bit.INTSEL = 2;			//计数值等于周期值时触发中断
	EPwm1Regs.ETPS.bit.INTPRD = 1;			//每个事件都触发中断
	EPwm1Regs.ETCLR.bit.INT = 1;			//清除中断标志位

	//Epwm2配置
	//时基单元设置
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 1;
	EPwm2Regs.TBCTL.bit.CLKDIV = 0;			//时钟不分频，Epwm1Clk = HSPCLK = 75000k
	EPwm2Regs.TBCTL.bit.SYNCOSEL = 0x3;		//禁止输出同步信号
	EPwm2Regs.TBCTL.bit.PHSEN = 1;			//使能相位同步
	EPwm2Regs.TBCTL.bit.PHSDIR = 1;			//同步之后向上计数
	EPwm2Regs.TBPRD = EPwm1Regs.TBPRD;		//设置周期寄存器值
	EPwm2Regs.TBCTR = 0;
	EPwm2Regs.TBPHS.half.TBPHS = 0;
	//比较单元配置
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = 0;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = 0;		//使能阴影寄存器并在cnt=0时重装载
	EPwm2Regs.CMPCTL.bit.LOADAMODE = 0;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = 0;		//使能阴影寄存器并在cnt=0时重装载
	//动作单元配置
	EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;		//A匹配之间为高
	//死区单元配置
	EPwm2Regs.DBCTL.bit.IN_MODE = 0;		//S(4,5)=(0,0)
	EPwm2Regs.DBCTL.bit.POLSEL = 2;			//S(2,3)=(0,1)
	EPwm2Regs.DBCTL.bit.OUT_MODE = 3;		//S(0,1)=(1,1)
	EPwm2Regs.DBRED = (Uint16)(myControlSystem.Actuator.DBTime*75);
	EPwm2Regs.DBFED = EPwm1Regs.DBRED;		//设置死区时间
	//斩波模块配置
	EPwm2Regs.PCCTL.bit.CHPEN = 0;			//禁止PWM斩波

	myControlSystem.Actuator.CarRatio = 1000*myControlSystem.Actuator.CarFre/myScr.FrOut;	//载波比
	PhasePerStep = 2*PI/myControlSystem.Actuator.CarRatio;								//每过一个载波,基波移动的电角度
	COPRD = C_OUTPUT*EPwm1Regs.TBPRD;		//输出和周期值相乘用于快速计算

	//中断寄存器配置
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;		//使能epwm1 PIE中断
	IER |= M_INT3;						//全局中断3
*/
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：int16 ChangeSampleState(State state)
//-----------------------------------------------------------------------------
// 函数功能：  改变执行机构状态
//-----------------------------------------------------------------------------
// 函数说明： 使能(禁止)T3定时器
//-----------------------------------------------------------------------------
// 输入参数：	state -> 执行机构状态
// 输出参数： 	0 -> 配置成功,1 -> 输入参数错误，2 -> 整流器未使能，4 -> EPW已被占用
//-----------------------------------------------------------------------------
Uint16 ActuatorChangeState(State state)		//执行机构改变状态
{
	/*if(state == Enable)
	{
		if(myScr.State == Disable)
			return 2;			//整流器未使能
		if((myHardwarePool.bit.EPWM1)&(myHardwarePool.bit.EPWM2))
			return 4;
		EPwm2Regs.TBCTR = 0;
		EPwm2Regs.TBCTL.bit.CTRMODE = 2;		//增减计数模式
		EPwm1Regs.TBCTR = 0;
		EPwm1Regs.TBCTL.bit.CTRMODE = 2;		//增减计数模式

		myHardwarePool.bit.EPWM1 = 1;
		myHardwarePool.bit.EPWM2 = 1;
		myControlSystem.Actuator.State = Enable;
	}
	else if((state == Disable)&(myControlSystem.Actuator.State == Enable))
	{
		EPwm2Regs.TBCTR = 0;
		EPwm2Regs.TBCTL.bit.CTRMODE = 3;		//停止计数模式
		EPwm1Regs.TBCTR = 0;
		EPwm1Regs.TBCTL.bit.CTRMODE = 3;		//停止计数模式

		myHardwarePool.bit.EPWM1 = 0;
		myHardwarePool.bit.EPWM2 = 0;
		myControlSystem.Actuator.State = Disable;
	}
	else
		return 1;
		*/
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：float ActuatorGetOutputPhase(void)
//-----------------------------------------------------------------------------
// 函数功能：  获取执行机构当前输出的相位
//-----------------------------------------------------------------------------
// 函数说明： 无
//-----------------------------------------------------------------------------
// 输入参数：	state -> 执行机构状态
// 输出参数： 	0-2*π -> 当前输出相位,-1 -> 执行机构未运行
//-----------------------------------------------------------------------------
float ActuatorGetOutputPhase(void)			//执行机构获取输出相位
{
	if(myControlSystem.Actuator.State == Disable)
		return -1;
	if(CurPhase < 0)
		CurPhase = CurPhase + _2PI;
	if(CurPhase >= _2PI)
		CurPhase = CurPhase - _2PI;
	return CurPhase;
}

//-----------------------------------------------------------------------------
//下面为所用到的对应中断程序
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 函数名：interrupt ADC_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能： ADC采样中断
//-----------------------------------------------------------------------------
// 函数说明： 1.将寄存器值右对齐进行保存
//			 2.中断服务中不对数据进行过多处理
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void ADC_ISR(void)
{
	SAMPLE_OV[8] = AdcRegs.ADCRESULT0>>4;
	//SAMPLE_OV[1] = AdcRegs.ADCRESULT1>>4;
	//SAMPLE_OV[2] = AdcRegs.ADCRESULT2>>4;
	//SAMPLE_OV[3] = AdcRegs.ADCRESULT3>>4;
	//SAMPLE_OV[4] = AdcRegs.ADCRESULT4>>4;
	//SAMPLE_OV[5] = AdcRegs.ADCRESULT5>>4;
	//SAMPLE_OV[6] = AdcRegs.ADCRESULT6>>4;
	//SAMPLE_OV[7] = AdcRegs.ADCRESULT7>>4;
	//SAMPLE_OV[0] = AdcRegs.ADCRESULT8>>4;
	//SAMPLE_OV[9] = AdcRegs.ADCRESULT9>>4;
	//SAMPLE_OV[10] = AdcRegs.ADCRESULT10>>4;
	SAMPLE_OV[12] = AdcRegs.ADCRESULT11>>4;
	//SAMPLE_OV[11] = AdcRegs.ADCRESULT12>>4;
	SAMPLE_OV[13] = AdcRegs.ADCRESULT13>>4;
	SAMPLE_OV[14] = AdcRegs.ADCRESULT14>>4;
	//SAMPLE_OV[15] = AdcRegs.ADCRESULT15>>4;

	//SamplePhase = myControlSystem.Actuator.GetOutputPhase();	//获取当前输出相位

	myControlSystem.Sample.SampleFlag = myControlSystem.Sample.SampleFlag^0x1;		//采样标志第0位取反
	myControlSystem.Sample.SampleFlag |= 0x2;								//采样标志第1位置位

	PieCtrlRegs.PIEACK.bit.ACK1 = 1;		//响应PIE同组中断
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;		//清除AD中断的标志
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;		//复位SEQ1为初始状态CON00
	EINT;  									//使能全局中断

}

//-----------------------------------------------------------------------------
// 函数名：interrupt T3Prd_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能： T3定时器周期中断
//-----------------------------------------------------------------------------
// 函数说明： 1.根据当前调制波输出电角度改变比较值
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void Epwm1_ISR(void)
{
	static int flagNewPrd = 1;		//为1时，表示需要产生新的一个周期

	if(flagNewPrd)		//新的一个周期
	{
		flagNewPrd = 0;
		myPhaseLock.ZeroTimeAct = ECap2Regs.TSCTR;	//保存控制器输出过零时刻，用于锁相
		CurPhase = 0;
		if(myPhaseLock.State == Enable)					//使能锁相
		{
			if(myPhaseLock.PSD == LeftShift)	//输出相位左移
				CurPhase = 0.02617993;			//少输出1.5°(实际为一个载波跨越电角度的整数倍)
			else if(myPhaseLock.PSD == RightShift)	//右移
				CurPhase = -0.02617993;			//多输出1.5°(同上，所以当一个载波跨越电角度大于1.5时，无法移相)
			else
				CurPhase = 0;
		}
		else
			CurPhase = 0;
	}

	if(CurPhase <= PI)		//前半周期
	{
		EPwm2Regs.CMPA.half.CMPA = 0;		//V1通,V2断
		EPwm1Regs.CMPA.half.CMPA = (Uint16)(COPRD*sin(CurPhase));		//控制器输出×周期值×sin(CurPhase)
		//EPwm2Regs.CMPA.half.CMPA = (Uint16)(COPRD);       //控制器输出×周期值×sin(CurPhase)
	}
	else					//后半周期
	{
		EPwm1Regs.CMPA.half.CMPA = 0;		//V3通,V4断
		EPwm2Regs.CMPA.half.CMPA = (Uint16)(COPRD*sin(CurPhase-PI));	//控制器输出×周期值×sin(CurPhase)
		//EPwm1Regs.CMPA.half.CMPA = (Uint16)(COPRD);    //控制器输出×周期值×sin(CurPhase)
	}
	CurPhase = CurPhase + PhasePerStep;		//计算下一次输出相位
	if(CurPhase >= _2PI)
		flagNewPrd = 1;						//开始新的一个周期

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
	EPwm1Regs.ETCLR.bit.INT = 1;			//清除中断标志位
	EINT;
}

//-----------------------------------------------------------------------------
// 函数名：interrupt void CpuTimer1_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能： CPU定时器1周期中断
//-----------------------------------------------------------------------------
// 函数说明：用于软启动
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void CpuTimer1_ISR(void)
{
//	C_OUTPUT = C_OUTPUT + OutPerStep;
//	RemainStep--;
    scr_number_switch(scr_4,0);
    scr_number_switch(scr_3,1);
	CpuTimer1Regs.TCR.bit.TIF = 1;		//清除标志位
	PieCtrlRegs.PIEACK.bit.ACK1=1;    //响应同组其他中断
	DELAY_US(200);
	StopCpuTimer1();
	scr_number_switch(scr_3,0);
}
/*
scr_number_switch(scr_3,1);
CpuTimer0Regs.TCR.bit.TIF=1;  //清除定时器中断标志位
PieCtrl.PIEACK.bit.ACK1=1;    //响应同组其他中断
StopCpuTimer0();
scr_number_switch(scr_3,0);
*/
//可控硅触发
void scr_number_switch(unsigned int scr_number,unsigned int High_Low)
{
    switch(scr_number)
    {
        case scr_1:
                    if(High_Low==HIGH)
                    {
                        GpioDataRegs.GPADAT.bit.GPIO0=1;
                    }
                    else if(High_Low==LOW)
                    {
                        GpioDataRegs.GPADAT.bit.GPIO0=0;
                    }
                    break;
        case scr_2:
                    if(High_Low==HIGH)
                    {
                        GpioDataRegs.GPASET.bit.GPIO1=1;
                    }
                    else if(High_Low==LOW)
                    {
                        GpioDataRegs.GPACLEAR.bit.GPIO1=1;
                    }
                    break;
        case scr_3:
                    if(High_Low==HIGH)
                    {
                        GpioDataRegs.GPADAT.bit.GPIO2=1;

                    }
                    else if(High_Low==LOW)
                    {
                        GpioDataRegs.GPADAT.bit.GPIO2=0;
                    }
                    break;
        case scr_4:
                    if(High_Low==HIGH)
                    {
                        GpioDataRegs.GPADAT.bit.GPIO3=1;

                    }
                    else if(High_Low==LOW)
                    {
                        GpioDataRegs.GPADAT.bit.GPIO3=0;
                    }
                    break;
        case scr_5:
                    if(High_Low==HIGH)
                    {
                        GpioDataRegs.GPASET.bit.GPIO4=1;
                    }
                    else if(High_Low==LOW)
                    {
                        GpioDataRegs.GPACLEAR.bit.GPIO4=1;
                    }
                    break;
        case scr_6:
                    if(High_Low==HIGH)
                    {
                        GpioDataRegs.GPASET.bit.GPIO5=1;
                    }
                    else if(High_Low==LOW)
                    {
                        GpioDataRegs.GPACLEAR.bit.GPIO5=1;
                    }
                    break;
        default:
                    break;

    }
}

void soft_start(void)
{
    if(muxian_count>1)
    {
        muxian_count=0;

        if(myScr.U_DCRMS<DC_SoftH_Target)//200
        {

            PID-=1;
            if(PID>190)
                PID=190;
            else if(PID<110)
                PID=110;
            //TIMER1_num=PID/360.0*20000;

        }
        else
        {
            SOFT_START=0;
        }
     }
}
void PID_change(float target)
{
  // if(muxian_count>1)
   // {
        //muxian_count=0;
        ek2=ek1;
        ek1=ek;
        ek=target-UDC;

        //  Kpo=0.02;
            Kio=Kpo*T/Ti;
            Kdo=Kpo*Td/T;

            Kp=Kpo;//*(1+Kp1*(1-exp(-100*ek*ek)));
         //  Ki=0.098;
         //  Kd = 0.035;
/*
           if(ek <= -5)        //误差值过大，禁止积分
              {
               Ki_Enable = 0;
               OUT0 = -0.5;
               //returnValue |= 1;
              }
           else if(ek >= 5)
             {
               Ki_Enable = 0;
               OUT0 = 0.5;
               //returnValue |= 1;
             }
           //  else if((PID >= 190)|(PID <= 118))      //输出饱和，禁止积分
            //  {
            //     Ki_Enable = 0;
                 // returnValue |= 1;
             // }
             else
             {
                 OUT0 = 0;
                 Ki_Enable = 1;
             }
*/
         //   Kd=0.02;
        //    Ki=Kio;//*(Ki1+Ki2*exp(-100*ek*ek));
         //   Kd=Kdo;//*(1+Kd1*exp(-25*ek*ek));

        //  PID=PID-(Kp*(2.45*ek-3.5*ek1+1.25*ek2));
            PID=PID-((Kp+Ki+Kd)*ek-(Kp+2*Kd)*ek1+Kd*ek2+OUT0);

        if(PID>190)
            PID=190;
        else if(PID<110)
            PID=110;

       // TIMER1_num=PID/360.0*20000;
   // }
}
