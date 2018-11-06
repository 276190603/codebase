//----------------------------------------------------------------------------
// 文件名：Can.c
//----------------------------------------------------------------------------
// 功能:  定义了Can模块
//
//----------------------------------------------------------------------------
// 说明： 使用了CANB模块
//----------------------------------------------------------------------------
// 引脚定义: 	GPIO20		->		CANTX
//				GPIO21		->		CANRX
// 芯片：		TMS230F28335
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2017年1月16日
// 修改日期：
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "Can.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 CanInit(void);
Uint16 CanChangeState(State state);
Uint16 ChangeAutoSendState(State state);
Uint16 CanSendData(Uint16 num,MailBoxType mbType);
//Uint16 CanSendDataWithPtc(Uint16 FunCode,float data,MailBoxType mbType);
Uint16 CanSendDataWithPtc(Uint16 FunCode,float data1,float data2,float data3,float data4,float data5,Uint16 data6,MailBoxType mbType) ; //采用通讯协议发送数据
Uint16 CanSendDataWithPtc2(Uint16 FunCode,MailBoxType mbType) ;   //采用通讯协议发送数据
Uint16 CanAutoRecData(void);
Uint16 CanScan(void);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
Uint16 FrameEnd = 0;	//是否有数据需要处理
//-----------------------------------------------------------------------------
// 函数名：void UseCan(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myCommunication中Can成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//			于对myCommunication的部分成员变量进行初始化。使用myCommunication之前必须要
//			调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
void UseCan(void)
{
	//属性
	myCommunication.CANCom.BaudRate = br1M;			//初始化波特率 = 1M bps
	CAN.SendId.Id = 0x1;								//发送邮箱ID为1与触屏通讯邮箱
	CAN.RecId.Id = 0x1;								//接收邮箱ID为1与触屏通讯邮箱
	CAN.RecIdMask.Id = 0x0000000f;//~0x3;						//接收帧ID第0,1位必须相同
	CAN.AutoSendId.Id = 4;							//自动发送邮箱ID为4与旁路板通讯邮箱
	CAN.AutoRecId.Id = 5;							//自动接收邮箱ID为5与旁路板通讯邮箱
	CAN.Flag.all = 0;								//标志位清除
	CAN.Flag.bit.TxBufEmpty = 1;					//标记发送缓冲区为空
	CAN.Flag.bit.AutoTxBufEmpty = 1;				//标记自动发送缓冲区为空
	CAN.RecNum = 0;									//接收数据个数为零
	CAN.AutoRecNum = 0;								//自动接收数据个数为零

	//方法
	myCommunication.CANCom.Init = CanInit;
	CAN.ChangeState = CanChangeState;
	CAN.ChangeAutoSendState = ChangeAutoSendState;
	CAN.SendData = CanSendData;
	CAN.SendDataWithPtc2 = CanSendDataWithPtc2;
	CAN.AutoRecData = CanAutoRecData;
	CAN.Scan = CanScan;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 CanInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化Can模块
//-----------------------------------------------------------------------------
// 函数说明：使用到CANB外设，使用GPIO20，21
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 初始化完成,1 -> 硬件资源已被使用,2 -> 波特率设置不正确
//-----------------------------------------------------------------------------
Uint16 CanInit(void)
{
	struct ECAN_REGS ECanbShadow;	//CAN模块的一些寄存器不允许16位访问，
									//读取和写入必须借助阴影寄存器

	if(myHardwarePool2.bit.Canb)
		return 1;		//CANB模块已被使用

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=1;    // 使能时钟eCAN-B
	EDIS;

	//配置相应IO
	EALLOW;				// EALLOW enables access to protected bits

	GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;   // Enable pull-up for GPIO20 (CANTXB)
	GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;   // Enable pull-up for GPIO21 (CANRXB)
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 3;  // Configure GPIO20 for CANTXB operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 3;  // Configure GPIO21 for CANRXB operation


	 //GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up for GPIO20 (CANTXB)
	//GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up for GPIO21 (CANRXB)
	 //   GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 2;  // Configure GPIO20 for CANTXB operation
	  //GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 2;  // Configure GPIO21 for CANRXB operation

	EDIS;

	InitECanb();		//初始化CANB模块至已知状态,该函数由官方标准文件提供。具体包括
						//使能CANB模块IO；配置为ECAN模式；初始化所有消息控制器为零；
						//清除邮箱发送成功、接收到报文以及中断的标志位；
						//配置波特率为1Mbps（三次采样）；关闭所有的邮箱
	EALLOW;
	//从新定义采样波特率
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1,请求配置
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
	{
	    ECanbShadow.CANES.all = ECanbRegs.CANES.all;
	} while(ECanbShadow.CANES.bit.CCE != 1 ); 		// Wait for CCE bit to be  cleared..,等待允许配置


    ECanbShadow.CANBTC.all = 0;

	/* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default.*/
	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
	ECanbShadow.CANBTC.bit.TSEG1REG = 10;
	//CAN.BaudRate = br500k;
    switch(CAN.BaudRate)
    {
    	case br125k:
    		ECanbShadow.CANBTC.bit.BRPREG = 39;		//CAN时钟频率为1875kHz
    		break;
    	case br250k:
    		ECanbShadow.CANBTC.bit.BRPREG = 19;		//CAN时钟频率为3750kHz
    		break;
    	case br500k:
    		ECanbShadow.CANBTC.bit.BRPREG = 9;		//CAN时钟频率为7500kHz
    		break;
    	case br750k:		//28335搞不出来准确的800k波特率
    		ECanbShadow.CANBTC.bit.BRPREG = 4;		//CAN时钟频率为15000kHz
    		ECanbShadow.CANBTC.bit.TSEG2REG = 3;
    		ECanbShadow.CANBTC.bit.TSEG1REG = 14;
    		break;
    	case br1M:
    		ECanbShadow.CANBTC.bit.BRPREG = 4;		//CAN时钟频率为15000kHz
    		break;
    	default:
    		return 2;
    }

    ECanbShadow.CANBTC.bit.SAM = 1;
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
    {
        ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..,配置完成返回正常状态

    EDIS;

    //配置邮箱0为发送邮箱
    ECanbMboxes.MBOX0.MSGID.all = 0;
    ECanbMboxes.MBOX0.MSGID.bit.IDE = 1;				//扩展标识符
    ECanbMboxes.MBOX0.MSGID.all |= CAN.SendId.Id;		//写入id

    ECanbMboxes.MBOX0.MSGCTRL.bit.TPL = 0;				//传送优先级,越大优先级越高

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;		//配置为发送邮箱
    ECanbShadow.CANMD.bit.MD0 = 0;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//使能邮箱
    ECanbShadow.CANME.bit.ME0 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    //配置邮箱1为接收邮箱
    ECanbMboxes.MBOX1.MSGID.all = 0;
    ECanbMboxes.MBOX1.MSGID.bit.IDE = 1;				//扩展标识符
    ECanbMboxes.MBOX1.MSGID.bit.AME = 0;				//使用屏蔽掩码诸位比较
    ECanbMboxes.MBOX1.MSGID.all |= CAN.RecId.Id;		//写入id

    ECanaLAMRegs.LAM1.all = 0;
    ECanaLAMRegs.LAM1.bit.LAMI = 1;						//
    ECanaLAMRegs.LAM1.all |= CAN.RecIdMask.Id;			//写入屏蔽符

    ECanbMboxes.MBOX1.MSGCTRL.bit.TPL = 0;				//传送优先级,越大优先级越高

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;		//配置为接收邮箱
    ECanbShadow.CANMD.bit.MD1 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//使能邮箱
    ECanbShadow.CANME.bit.ME1 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    //配置邮箱2为自动发送邮箱
    ECanbMboxes.MBOX2.MSGID.all = 0;
    ECanbMboxes.MBOX2.MSGID.bit.IDE = 1;                //扩展标识符
    ECanbMboxes.MBOX2.MSGID.all |= CAN.AutoSendId.Id;       //写入id

    ECanbMboxes.MBOX2.MSGCTRL.bit.TPL = 0;              //传送优先级,越大优先级越高

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;        //配置为发送邮箱
    ECanbShadow.CANMD.bit.MD2 = 0;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;        //使能邮箱
    ECanbShadow.CANME.bit.ME2 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

//    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//使能邮箱
//    ECanbShadow.CANME.bit.ME2 = 1;
//    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    //配置邮箱3为自动接收邮箱
    ECanbMboxes.MBOX3.MSGID.all = 0;
    ECanbMboxes.MBOX3.MSGID.bit.IDE = 1;                //扩展标识符
    ECanbMboxes.MBOX3.MSGID.bit.AME = 0;                //使用屏蔽掩码诸位比较
    ECanbMboxes.MBOX3.MSGID.all |= CAN.AutoRecId.Id;        //写入id

    ECanaLAMRegs.LAM3.all = 0;
    ECanaLAMRegs.LAM3.bit.LAMI = 1;                     //
    ECanaLAMRegs.LAM3.all |= CAN.RecIdMask.Id;          //写入屏蔽符

    ECanbMboxes.MBOX3.MSGCTRL.bit.TPL = 0;              //传送优先级,越大优先级越高

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;        //配置为接收邮箱
    ECanbShadow.CANMD.bit.MD3 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;        //使能邮箱
    ECanbShadow.CANME.bit.ME3 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
    /*
    ECanbMboxes.MBOX3.MSGID.all = 0;
    ECanbMboxes.MBOX3.MSGID.bit.IDE = 1;				//扩展标识符
    ECanbMboxes.MBOX3.MSGID.all |= CAN.AutoRecId.Id;	//写入id

    ECanbMboxes.MBOX3.MSGCTRL.bit.TPL = 0;				//传送优先级,越大优先级越高
    ECanbMboxes.MBOX3.MSGCTRL.bit.RTR = 1;				//可以发送远程帧并使用该邮箱接收对应数据帧

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;		//配置为接收邮箱
    ECanbShadow.CANMD.bit.MD3 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//使能邮箱
    ECanbShadow.CANME.bit.ME3 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
*/
    EALLOW;

	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.DBO = 0;						//先发送数据的低字节
	ECanbShadow.CANMC.bit.ABO = 1;						//总线自动打开
	ECanbShadow.CANMC.bit.STM = 0;						//非自检测模式
	ECanbShadow.CANMC.bit.SCB = 1;
	ECanbShadow.CANMC.bit.MBNR = 0x0;					//请求邮箱0的写入权限
	//ECanbShadow.CANMC.bit.SCB = 1;
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    //中断配置
	ECanbRegs.CANGIM.all = 0x3;					// 使能中断0 1
	ECanbRegs.CANMIM.all = 0xf;					// 使能邮箱0~3收发中断
	ECanbRegs.CANMIL.all = 0xA;					// 邮箱0,2中断映射到中断0 .邮箱1,3中断映射到1
	ECanbRegs.CANOPC.all = 0;					// 允许新消息覆盖

	PieCtrlRegs.PIEIER9.bit.INTx7 = 1;		//使能PIE CANB INT0中断
	PieCtrlRegs.PIEIER9.bit.INTx8 = 1;		//使能PIE CANB INT1中断
	IER |= M_INT9;    						//开启CPU中断 组9

	EDIS;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=0;    // 暂时关闭时钟时钟eCAN-B
	EDIS;

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 CanChangeState(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变Can模块状态
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	state -> 需要改变的状态(Enable,Disable)
// 输出参数： 	0 -> 初始化完成,1 -> 硬件资源已被使用,2 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 CanChangeState(State state)
{
	struct ECAN_REGS ECanbShadow;	//CAN模块的一些寄存器不允许16位访问，
									//读取和写入必须借助阴影寄存器

	if(myHardwarePool2.bit.Canb)
		return 1;
	if(state == Enable)
	{
		EALLOW;
		SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=1;    // eCAN-B
		EDIS;
		myHardwarePool2.bit.Canb = 1;
		myCommunication.Enable.bit.CanEnable = 1;     //使能了模块
	}
	else if(state == Disable)
	{
		EALLOW;

	    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//禁止自动发送邮箱
	    ECanbShadow.CANME.bit.ME2 = 0;
	    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

		SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=0;    // eCAN-B
		EDIS;
		myHardwarePool2.bit.Canb = 0;
		myCommunication.Enable.bit.CanEnable = 0;     //使能了模块
	}
	else
		return 2;

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 ChangeAutoSendState(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变自动发送状态
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	state -> 需要改变的状态(Enable,Disable)
// 输出参数： 	0 -> 初始化完成,1 -> 硬件资源已被使用,2 -> 无效的输入参数
//-----------------------------------------------------------------------------
Uint16 ChangeAutoSendState(State state)
{
	struct ECAN_REGS ECanbShadow;	//CAN模块的一些寄存器不允许16位访问，
									//读取和写入必须借助阴影寄存器

	if(myHardwarePool2.bit.Canb)
		return 1;
	if(state == Enable)
	{
		EALLOW;
	    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//使能自动发送邮箱
	    ECanbShadow.CANME.bit.ME2 = 1;
	    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
		EDIS;
	}
	else if(state == Disable)
	{
		EALLOW;
	    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//禁止自动发送邮箱
	    ECanbShadow.CANME.bit.ME2 = 0;
	    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
		EDIS;
	}
	else
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 CanSendData(Uint16 num,MailBoxType mbType)
//-----------------------------------------------------------------------------
// 函数功能： 发送缓冲区中前num个数据
//-----------------------------------------------------------------------------
// 函数说明：如果使用发送邮箱会立刻发送数据，如果使用自动发送邮箱会在接收到一个远程帧
//			请求后发送，发送成功后Flag相应的位会置一
//-----------------------------------------------------------------------------
// 输入参数：	num -> 需要发送的数据个数,mbType -> 邮箱类型(SendMailBox,AutoSendMailBox)
// 输出参数： 	0 -> 发送成功,1 -> CAN模块未使能,2 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 CanSendData(Uint16 num,MailBoxType mbType)	//发送数据
{
	if(myCommunication.Enable.bit.CanEnable == 0)
		return 1;
	if(mbType == SendMailBox)		//发送邮箱
	{
		ECanbMboxes.MBOX0.MDL.byte.BYTE0 = CAN.SendBuf[0];		//数据写入邮箱
		ECanbMboxes.MBOX0.MDL.byte.BYTE1 = CAN.SendBuf[1];
		ECanbMboxes.MBOX0.MDL.byte.BYTE2 = CAN.SendBuf[2];
		ECanbMboxes.MBOX0.MDL.byte.BYTE3 = CAN.SendBuf[3];
		ECanbMboxes.MBOX0.MDH.byte.BYTE4 = CAN.SendBuf[4];
		ECanbMboxes.MBOX0.MDH.byte.BYTE5 = CAN.SendBuf[5];
		ECanbMboxes.MBOX0.MDH.byte.BYTE6 = CAN.SendBuf[6];
		ECanbMboxes.MBOX0.MDH.byte.BYTE7 = CAN.SendBuf[7];

		ECanbMboxes.MBOX0.MSGCTRL.bit.DLC = num;				//定义发送数据个数
		ECanbRegs.CANTRS.all = 0x00000001;						//发送数据
	}
	else if(mbType == AutoSendMailBox)	//自动发送邮箱
	{
		ECanbMboxes.MBOX2.MDL.byte.BYTE0 = CAN.SendBuf[0];		//数据写入邮箱
		ECanbMboxes.MBOX2.MDL.byte.BYTE1 = CAN.SendBuf[1];
		ECanbMboxes.MBOX2.MDL.byte.BYTE2 = CAN.SendBuf[2];
		ECanbMboxes.MBOX2.MDL.byte.BYTE3 = CAN.SendBuf[3];
		ECanbMboxes.MBOX2.MDH.byte.BYTE4 = CAN.SendBuf[4];
		ECanbMboxes.MBOX2.MDH.byte.BYTE5 = CAN.SendBuf[5];
		ECanbMboxes.MBOX2.MDH.byte.BYTE6 = CAN.SendBuf[6];
		ECanbMboxes.MBOX2.MDH.byte.BYTE7 = CAN.SendBuf[7];

		ECanbMboxes.MBOX2.MSGCTRL.bit.DLC = num;				//定义发送数据个数
		ECanbRegs.CANTRS.all = 0x00000004;                      //发送数据
	}
	else
		return 2;
	return 0;
}

Uint16 CanSendDataWithPtc2(Uint16 FunCode,MailBoxType mbType)    //采用通讯协议发送数据
{
    Uint16 reVal = 0;
    CAN.SendBuf[0] = FunCode;          //填写功能码
    reVal = CanSendData(1,mbType);      //发送数据
    return reVal;
}
Uint16 CanSendDataWithPtc3(Uint16 FunCode,MailBoxType mbType)    //采用通讯协议发送数据
{
    Uint16 reVal = 0;
    CAN.SendBuf[0] = 2;          //填写功能码
    CAN.SendBuf[1] = FunCode;          //填写功能码
    reVal = CanSendData(2,mbType);      //发送数据
    return reVal;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 CanSendDataWithPtc(float data,MailBoxType mbType)
//-----------------------------------------------------------------------------
// 函数功能： 采用通讯协议发送数据
//-----------------------------------------------------------------------------
// 函数说明：如果使用发送邮箱会立刻发送数据，如果使用自动发送邮箱会在接收到一个远程帧
//			请求后发送，发送成功后Flag相应的位会置一
//-----------------------------------------------------------------------------
// 输入参数：	FunCode -> 功能码,data -> 需要发送的数据,mbType -> 邮箱类型(SendMailBox,AutoSendMailBox)
// 输出参数： 	0 -> 发送成功,1 -> CAN模块未使能,2 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 CanSendDataWithPtc(Uint16 FunCode,float data1,float data2,float data3,float data4,float data5,Uint16 data6,MailBoxType mbType)	//采用通讯协议发送数据
{
	Uint16 reVal = 0;
	//Write2Buf(data,CanMdl);				//按协议将数据放入缓冲区中
	CAN.SendBuf[0] = (Uint16)data1;			//填写功能码
	CAN.SendBuf[1] = (Uint16)data2;           //填写功能码
	CAN.SendBuf[2] = (Uint16)data3;           //填写功能码
	CAN.SendBuf[3] = (Uint16)data4;           //填写功能码
	CAN.SendBuf[4] = (Uint16)data5;           //填写功能码
    CAN.SendBuf[5] = (Uint16)data6;           //填写功能码
	reVal = CanSendData(6,mbType);		//发送数据
	return reVal;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 CanAutoRecData(void)
//-----------------------------------------------------------------------------
// 函数功能： 发送一个远程帧，并自动接收数据帧
//-----------------------------------------------------------------------------
// 函数说明：如果接收到相应的数据帧后，Flag中的相应位会置一
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 请求成功,1 -> CAN模块未使能
//-----------------------------------------------------------------------------
Uint16 CanAutoRecData(void)
{
	if(myCommunication.Enable.bit.CanEnable == 0)
		return 1;
	ECanaRegs.CANTRS.all = 0x00000008;						//发送数据
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 CanScan(void)
//-----------------------------------------------------------------------------
// 函数功能：循环扫描有没有数据需处理
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 处理了一帧数据,1 -> 没有数据需要处理,2 -> 未识别的功能码
//-----------------------------------------------------------------------------
Uint16 CanScan(void)		//扫描有无中断需要处理
{
	if(FrameEnd == 0)
		return 1;
	if(CAN.Flag.bit.RecFrame)		//接收缓冲区有数据
	{
		if(CAN.RecNum == 5)		//接收到五个字节
		{
			switch(CAN.RecBuf[0])
			{
				case FRA_BP_ADD:		//逆变输出A相电压
					CanSendDataWithPtc(FRA_BP_ADD,FPGA_FLAG.bit.InvRun,myInverter.UA_InvOutRMS,myPhaseLock.PSD_BYP,myInverter.FrA_INV,myInverter.IA_TotalOutRMS,SOFT_START,SendMailBox);	//发送数据 FPGA_FLAG.bit.InvRun
					break;
                case INV_START:        //逆变输出A相电压
                    myInverter.SOFTSTART = CAN.RecBuf[1];
                    break;
				default:
					return 2;
			}
		}

		CAN.Flag.bit.RecFrame = 0;	//清除标志位
	}
	else if(CAN.Flag.bit.AutoRecFrame)	//自动接收一帧数据
	{
        if(CAN.AutoRecNum == 1)     //接收到五个字节
        {
            switch(CAN.AutoRecBuf[0])
            {
                case 0:        //逆变输出A相电压
                    if((myPhaseLock.PSD_BYP == NoShift)&&(myFaultDetect.FlagOther.all==0)&&(myFaultDetect.FlagRms.all==0))
                    myFpgaDriver.InvSwitchOn(Enable);
                    break;
                case 1:        //逆变输出A相电压
                   // if((myPhaseLock.PSD == NoneShift)&&(myFaultDetect.FlagOther.all==0)&&(myFaultDetect.FlagRms.all==0))
                    myFpgaDriver.InvSwitchOn(Disable);
                    break;
                case 2:
                    CanSendDataWithPtc3(!FPGA_FLAG.bit.InvRun,AutoSendMailBox);   //开逆变关旁路 SOFT_START FPGA_FLAG.bit.InvRun
                    break;
                default:
                    return 2;
            }
        }
		CAN.Flag.bit.AutoRecFrame = 0;	//清除标志位
	}
	return 0;
}

//-----------------------------------------------------------------------------
//下面为所用到的对应中断程序
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 函数名：interrupt void CanbInt0_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能：Canb Int0中断
//-----------------------------------------------------------------------------
// 函数说明：发送(0)和自动发送(2)邮箱中断映射到该中断
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void CanbInt0_ISR(void)
{
	struct ECAN_REGS ECanbShadow;

	ECanbShadow.CANGIF0.all = ECanbRegs.CANGIF0.all;
	if(ECanbShadow.CANGIF0.bit.MIV0 == 0)		//邮箱0中断
	{
		CAN.Flag.bit.TxBufEmpty = 1;		//发送完成，标记缓冲区空
		ECanbRegs.CANTA.all = 1;			//清除标志位
	}
	else if(ECanbShadow.CANGIF0.bit.MIV0 == 2)	//邮箱2中断
	{
		CAN.Flag.bit.AutoTxBufEmpty = 1;	//自动发送完成，标记缓冲区空
		ECanbRegs.CANTA.all = 0x4;			//清除标志位
	}
	PieCtrlRegs.PIEACK.all|=PIEACK_GROUP9;  	// Issue PIE ack
}

//-----------------------------------------------------------------------------
// 函数名：interrupt void CanbInt1_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能：Canb Int1中断
//-----------------------------------------------------------------------------
// 函数说明：接收(1)和自动接收(3)邮箱中断映射到该中断
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void CanbInt1_ISR(void)
{
	struct ECAN_REGS ECanbShadow;

	FrameEnd = 1;			//有一帧数据需要处理
	ECanbShadow.CANGIF1.all = ECanbRegs.CANGIF1.all;
	if(ECanbShadow.CANGIF1.bit.MIV1 == 1)		//接收邮箱接收到数据
	{
		CAN.RecBuf[0] = ECanbMboxes.MBOX1.MDL.byte.BYTE0;
		CAN.RecBuf[1] = ECanbMboxes.MBOX1.MDL.byte.BYTE1;
		CAN.RecBuf[2] = ECanbMboxes.MBOX1.MDL.byte.BYTE2;
		CAN.RecBuf[3] = ECanbMboxes.MBOX1.MDL.byte.BYTE3;
		CAN.RecBuf[4] = ECanbMboxes.MBOX1.MDH.byte.BYTE4;
		CAN.RecBuf[5] = ECanbMboxes.MBOX1.MDH.byte.BYTE5;
		CAN.RecBuf[6] = ECanbMboxes.MBOX1.MDH.byte.BYTE6;
		CAN.RecBuf[7] = ECanbMboxes.MBOX1.MDH.byte.BYTE7;
		CAN.RecNum = ECanbMboxes.MBOX1.MSGCTRL.bit.DLC;		//记录接收的字节数
		CAN.Flag.bit.RecFrame = 1;		//标记接收到一帧数据

		ECanbRegs.CANRMP.all = 2;		//清除中断标志位
	}
	else if(ECanbShadow.CANGIF1.bit.MIV1 == 3)	//自动接收邮箱接收到数据
	{
		CAN.AutoRecBuf[0] = ECanbMboxes.MBOX3.MDL.byte.BYTE0;
		CAN.AutoRecBuf[1] = ECanbMboxes.MBOX3.MDL.byte.BYTE1;
		CAN.AutoRecBuf[2] = ECanbMboxes.MBOX3.MDL.byte.BYTE2;
		CAN.AutoRecBuf[3] = ECanbMboxes.MBOX3.MDL.byte.BYTE3;
		CAN.AutoRecBuf[4] = ECanbMboxes.MBOX3.MDH.byte.BYTE4;
		CAN.AutoRecBuf[5] = ECanbMboxes.MBOX3.MDH.byte.BYTE5;
		CAN.AutoRecBuf[6] = ECanbMboxes.MBOX3.MDH.byte.BYTE6;
		CAN.AutoRecBuf[7] = ECanbMboxes.MBOX3.MDH.byte.BYTE7;
		CAN.AutoRecNum = ECanbMboxes.MBOX3.MSGCTRL.bit.DLC;		//记录接收的字节数
		CAN.Flag.bit.AutoRecFrame = 1;		//标记接收到一帧数据

		ECanbRegs.CANRMP.all = 8;		//清除中断标志位
	}
	PieCtrlRegs.PIEACK.all|=PIEACK_GROUP9;  	// Issue PIE ack
}



