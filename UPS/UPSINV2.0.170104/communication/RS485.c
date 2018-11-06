//----------------------------------------------------------------------------
// 文件名：RS485.c
//----------------------------------------------------------------------------
// 功能:  定义了RS485模块
//
//----------------------------------------------------------------------------
// 说明： 使用了SCIC模块
//----------------------------------------------------------------------------
// 引脚定义: 	GPIO61	->	485EN
//			 	GPIO62	->	485RX
//				GPIO63	->	485TX
// 芯片：	TMS230F28335
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2017年1月14日
// 修改日期：
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "RS485.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 RS485Init(void);
Uint16 RS485ChangeState(State state);
Uint16 RS485SendData(Uint16 num);
Uint16 Rs485SendRemain(void);
Uint16 RS485MB03Return(float _data);
Uint16 RS485Scan(void);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
Uint16 RemainSendNum485 = 0;		//剩余需要发送数据的个数
Uint16 TxBufferPtr485 = 0;			//发送缓冲区指针，始终指向下一个要发送的数据

//-----------------------------------------------------------------------------
// 函数名：void UseRS485(void)
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
void UseRS485(void)
{
	RS485.BaudRate = 9600;
	RS485.DataBits = 8;
	RS485.StopBits = 1;
	RS485.Parity = NoP;
	RS485.RecNum = 0;
	RS485.Flag.all = 0;		//清除所有标志位
	RS485.Flag.bit.SendEmptyFlag = 1;		//发送缓冲区空

	RS485.Init = RS485Init;
	RS485.ChangeState = RS485ChangeState;
	RS485.SendData = RS485SendData;
	RS485.MB03Return = RS485MB03Return;
	RS485.Scan = RS485Scan;
}


//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485Init(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化RS485模块
//-----------------------------------------------------------------------------
// 函数说明：使用到SCIC模块和GPIO, 使用IO为GPIO61，62，63
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 初始化成功,1 -> 硬件资源被占用,2 -> 波特率参数错误
//				3 -> 数据位参数错误,4 -> 停止位参数错误,5 -> 校验参数错误
//-----------------------------------------------------------------------------
Uint16 RS485Init(void)
{
	if(myHardwarePool2.bit.Scic)
		return 1;			//硬件资源已使用

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 1;		//使能外设时钟

	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;		//配置为通用IO
	GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;			//配置为输出
	GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;			//使能上拉

	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;		//配置为SCIC RX
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;		//配置为SCIC TX
	EDIS;

	if((RS485.BaudRate >= 2000)&(RS485.BaudRate <= 40000))		//波特率
	{
		float temp = LSPCLK28xx*1.0/RS485.BaudRate/8;
		Uint16 brr = temp*1000 - 1;
		ScicRegs.SCIHBAUD = brr>>8;
		ScicRegs.SCILBAUD = brr&0xff;
	}
	else
		return 2;

	if((RS485.DataBits >= 1)&(RS485.DataBits <= 8))			//数据位
		ScicRegs.SCICCR.bit.SCICHAR = RS485.DataBits-1;
	else
		return 3;

	if(RS485.StopBits == 1)									//停止位
		ScicRegs.SCICCR.bit.STOPBITS = 0;
	else if(RS485.StopBits == 2)
		ScicRegs.SCICCR.bit.STOPBITS = 1;
	else
		return 4;

	switch(RS485.Parity)									//校验位
	{
		case NoP:
			ScicRegs.SCICCR.bit.PARITYENA = 0;
			break;
		case OddP:
			ScicRegs.SCICCR.bit.PARITYENA = 1;
			ScicRegs.SCICCR.bit.PARITY = 0;
			break;
		case EvenP:
			ScicRegs.SCICCR.bit.PARITYENA = 1;
			ScicRegs.SCICCR.bit.PARITY = 1;
			break;
		default:
			return 5;
	}

	ScicRegs.SCICCR.bit.LOOPBKENA = 0;		//禁止回环模式
	ScicRegs.SCICCR.bit.ADDRIDLE_MODE = 0;	//空闲线模式

//	ScicRegs.SCICTL1.bit.TXENA = 1;			//使能发送
//	ScicRegs.SCICTL1.bit.RXENA = 1;			//使能接收
	ScicRegs.SCICTL1.bit.SWRESET = 0;		//复位SCI

	ScicRegs.SCIFFTX.bit.SCIRST = 1;		//从新开始FIFO
	ScicRegs.SCIFFTX.bit.SCIFFENA = 1;		//使能FIFO
	ScicRegs.SCIFFTX.bit.TXFFIL = 1;		//剩余发送字节小于等于1时，产生中断

	ScicRegs.SCIFFRX.bit.RXFFIL = 1;		//接收到一个字节就产生中断

	ScicRegs.SCICTL1.bit.SWRESET = 1;		//复位完毕，使能SCI

	ScicRegs.SCIFFTX.bit.TXFIFOXRESET = 1;	//复位FIFO发送指针
	ScicRegs.SCIFFRX.bit.RXFIFORESET = 1;	//复位FIFO接收指针

	PieCtrlRegs.PIEIER8.bit.INTx6 = 1;		//使能PIE SCIC TX中断
	PieCtrlRegs.PIEIER8.bit.INTx5 = 1;		//使能PIE SCIC RX中断
	IER |= M_INT8;    						//开启CPU中断 组8

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485ChangeState(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变485模块的状态
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	state -> 需要设置的状态
// 输出参数： 	0 -> 设置成功,1 -> 硬件已被占用,2 -> 总通讯使能为开启
//				3 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 RS485ChangeState(State state)	//改变RS485通讯模块状态
{
	if(state == Enable)
	{
		if(myHardwarePool2.bit.Scic)
			return 1;			//硬件资源已使用
		if(myCommunication.Enable.bit.Enable == 0)
			return 2;			//总通讯使能未开启

		ScicRegs.SCICTL1.bit.TXENA = 1;			//使能发送
		ScicRegs.SCICTL1.bit.RXENA = 1;			//使能接收

		ScicRegs.SCICTL2.bit.TXINTENA = 1;		//使能发送中断	FIFO模式这两个中断被屏蔽了，所以不设置也可以
		ScicRegs.SCICTL2.bit.RXBKINTENA = 1;	//使能接收中断

		ScicRegs.SCIFFTX.bit.TXFFIENA = 1;		//使能FIFO发送中断
		ScicRegs.SCIFFRX.bit.RXFFIENA = 1;		//使能FIFO接收中断

		myHardwarePool2.bit.Scic = 1;			//硬件资源被占有
		myCommunication.Enable.bit.Rs485Enable = 1;		//使能了485模块
	}
	else if(state == Disable)
	{
		ScicRegs.SCICTL1.bit.TXENA = 0;			//禁止发送
		ScicRegs.SCICTL1.bit.RXENA = 0;			//禁止接收

		ScicRegs.SCICTL2.bit.TXINTENA = 0;		//禁止发送中断
		ScicRegs.SCICTL2.bit.RXBKINTENA = 0;	//禁止接收中断

		ScicRegs.SCIFFTX.bit.TXFFIENA = 0;		//禁止FIFO发送中断
		ScicRegs.SCIFFRX.bit.RXFFIENA = 0;		//禁止FIFO接收中断

		myHardwarePool2.bit.Scic = 0;					//释放硬件资源
		myCommunication.Enable.bit.Rs485Enable = 0;		//失能485模块
	}
	else
		return 3;

	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485SendData(Uint16 num)
//-----------------------------------------------------------------------------
// 函数功能： 发送缓冲区中前num个数据
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	num -> 需要发送的数据
// 输出参数： 	0 -> 发送成功,1 -> num大于缓冲区最大字节数,2 -> 上一针数据未发送完
//				3 -> 模块为使能
//-----------------------------------------------------------------------------
Uint16 RS485SendData(Uint16 num)		//发送缓冲区中前num个数据
{
	if(num > SCI_TXBUFFER_SIZE)
		return 1;
	if(RS485.Flag.bit.SendEmptyFlag == 0)
		return 2;
	if(myCommunication.Enable.bit.Rs485Enable == 0)
		return 3;
	RS485_TXENABLE;				//使能发送
	RemainSendNum485 = num;
	TxBufferPtr485 = 0;			//指向缓冲区中第一个数据
	Rs485SendRemain();			//发送一波数据
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uin16 Rs485SendRemain(void)
//-----------------------------------------------------------------------------
// 函数功能：向发送缓冲区中写入10个数据，如果不足则全写入
//-----------------------------------------------------------------------------
// 函数说明：写入成功后会相应的修改指针TxBufferPtr485和剩余发送的数据RemainSendNum485
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 写入成功,1 -> 发送的数据超出缓冲区大小,2 -> 没有数学需要发送
//-----------------------------------------------------------------------------
Uint16 Rs485SendRemain(void)				//发送缓冲区中剩下的数据
{
	if((RemainSendNum485+TxBufferPtr485) > SCI_TXBUFFER_SIZE)
		return 1;						//发送的数据超出缓冲区
	if(RemainSendNum485 == 0)
		return 2;

	RS485.Flag.bit.SendEmptyFlag = 0;		//开始发送数据，发送缓冲区不空
	if(RemainSendNum485 >= RS485_FIFO_SEND_NUM)
	{
		int i = 0;
		for(i = 0;i < RS485_FIFO_SEND_NUM;i++)		//发送最大个数数据
		{
			ScicRegs.SCITXBUF = RS485.SendBuffer[TxBufferPtr485++];
			RemainSendNum485--;
		}
	}
	else
	{
		while(RemainSendNum485)		//将剩余的数据全部发送
		{
			ScicRegs.SCITXBUF = RS485.SendBuffer[TxBufferPtr485++];
			RemainSendNum485--;
		}
	}
	ScicRegs.SCIFFTX.bit.TXFFIENA = 1;		//使能FIFO发送中断
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485MB03Return(Uint16 dataName)
//-----------------------------------------------------------------------------
// 函数功能：以Modbus 03命令返回值的帧形式发送数据
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	_data -> 需要发送的浮点数
// 输出参数： 	0 -> 写入成功,1 -> 发送的数据超出缓冲区大小,2 -> 没有数学需要发送
//-----------------------------------------------------------------------------
Uint16 RS485MB03Return(float _data)		//使用modbus 03命令返回格式发送数据
{
	Uint16 sendNum = 0;
	sendNum = Write2Buf(_data,RS485Mdl);			//将数据写入到缓冲区中
	RS485SendData(sendNum);							//发送数据
	return 0;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485MB06Return()
//-----------------------------------------------------------------------------
// 函数功能：以Modbus 03命令返回值的帧形式发送数据
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：    _data -> 需要发送的浮点数
// 输出参数：    0 -> 写入成功,1 -> 发送的数据超出缓冲区大小,2 -> 没有数学需要发送
//-----------------------------------------------------------------------------
Uint16 RS485MB06Return()     //使用modbus 06命令返回格式发送数据
{
    Uint16 sendNum = 0;
    sendNum = Write06Buf(RS485Mdl);            //将数据写入到缓冲区中
    RS485SendData(sendNum);                         //发送数据
    return 0;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485MB16Return()
//-----------------------------------------------------------------------------
// 函数功能：以Modbus 03命令返回值的帧形式发送数据
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：    _data -> 需要发送的浮点数
// 输出参数：    0 -> 写入成功,1 -> 发送的数据超出缓冲区大小,2 -> 没有数学需要发送
//-----------------------------------------------------------------------------
Uint16 RS485MB16Return()     //使用modbus 16命令返回格式发送数据
{
    Uint16 sendNum = 0;
    sendNum = Write16Buf(RS485Mdl);            //将数据写入到缓冲区中
    RS485SendData(sendNum);                         //发送数据
    return 0;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 RS485Scan(void)
//-----------------------------------------------------------------------------
// 函数功能：循环扫描有没有需要处理的接收数据帧
//-----------------------------------------------------------------------------
// 函数说明：当接收到的数据帧格式为modbus 03命令时，进行数据处理
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	0 -> 处理了一帧数据,1 -> 没有有效的数据帧,2 -> 帧格式错误
//				3 -> 无效的数据地址
//-----------------------------------------------------------------------------
Uint16 RS485Scan(void)					//485扫描函数
{
	Uint16 returnVal = 0;

	if(RS485.Flag.bit.RecFrameFlag == 0)
		return 1;
	if(VerifyFrame(RS485Mdl) == 0)		//验证地址、功能码、读取寄存器个数、校验是否正确
	{
		Uint16 dataAdd = (RS485.RecBuffer[2]<<8)|(RS485.RecBuffer[3]);		//读取数据的地址
		switch(dataAdd)
		{
		    case INV_START:
		        myInverter.SOFTSTART = (RS485.RecBuffer[7]<<8)|(RS485.RecBuffer[8]);
		        RS485MB16Return();     //以modbus 03 命令返回形式返回数据
		        break;
			case FRA_BP_ADD:			//A相输出电压
				RS485MB03Return(myInverter.FrA_BP);		//以modbus 03 命令返回形式返回数据
				break;
            case FRA_INV_ADD:            //A相输出电压
                RS485MB03Return(myInverter.FrA_INV);     //以modbus 03 命令返回形式返回数据
                break;
            case UA_BP_ADD:        //逆变A相输出电压有效值
                RS485MB03Return(myInverter.UA_BPRMS);       //以modbus 03 命令返回形式返回数据
                break;
			case UA_INVOUTR_ADD:		//逆变A相输出电压有效值
				RS485MB03Return(myInverter.UA_InvOutRMS);		//以modbus 03 命令返回形式返回数据
				break;
            case Phase_INV_BP:        //逆变A相输出电压有效值
                RS485MB03Return(myPhaseLock.BP_INV_Phase);       //以modbus 03 命令返回形式返回数据
                break;

			default:
				returnVal = 3;
		}
	}
	else
		returnVal = 2;
	RS485.Flag.bit.RecOverflow = 0;		//标记为没有溢出
	RS485.RecNum = 0;					//收到的数据个数清零
	RS485.Flag.bit.RecFrameFlag = 0;	//标记缓冲区没有数据帧
	return returnVal;
}

//-----------------------------------------------------------------------------
//下面为所用到的对应中断程序
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 函数名：interrupt ScicTx_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能：SCIC发送中断
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void ScicTx_ISR(void)
{
	if(RemainSendNum485 == 0)		//数据全部写入完毕
	{
		RS485.Flag.bit.SendEnd = 1;				//数据传送完毕
		ScicRegs.SCIFFTX.bit.TXFFIENA = 0;		//禁止FIFO发送中断
	}

	Rs485SendRemain();			//发送剩余的数据

	ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;		//清除标志位
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;		//响应PIE同组中断
}

//-----------------------------------------------------------------------------
// 函数名：interrupt void ScicRx_ISR(void)
//-----------------------------------------------------------------------------
// 函数功能：SCIC接收中断
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
interrupt void ScicRx_ISR(void)
{
	if(RS485.Flag.bit.RecOverflow)		//接收数据溢出
	{
		RS485.Flag.bit.RecData = 1;			//标记接收到数据了
	}
	else if(!RS485.Flag.bit.RecFrameFlag)		//缓冲区中没有数据帧
	{
		RS485.RecBuffer[RS485.RecNum++] = ScicRegs.SCIRXBUF.all;	//读取数据
		if(RS485.RecNum >= SCI_RXBUFFER_SIZE)
			RS485.Flag.bit.RecOverflow = 1;		//接收溢出
		RS485.Flag.bit.RecData = 1;			//标记接收到数据了
	}

	ScicRegs.SCIFFRX.bit.RXFFOVRCLR=1; 		 	// Clear Overflow flag
	ScicRegs.SCIFFRX.bit.RXFFINTCLR=1; 			// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all|=PIEACK_GROUP8;  	// Issue PIE ack
}

