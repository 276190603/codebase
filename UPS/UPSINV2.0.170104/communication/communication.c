//----------------------------------------------------------------------------
// 文件名：communication.c
//----------------------------------------------------------------------------
// 功能:  定义了通讯模块
//
//----------------------------------------------------------------------------
// 说明： 使用了SCIA,SCIB,CAN模块
//----------------------------------------------------------------------------
// 引脚定义：GPIO22		->		232RX
//			GPIO23		->		232TX
//			GPIO20		->		CANTX
//			GPIO21		->		CANRX
//			GPIO61		->		485EN
//			GPIO62		->		485RX
//			GPIO63		->		485TX
// 芯片：	TMS230F28335
//----------------------------------------------------------------------------
// 作者： 刘亚彬
// 创建日期: 2017年1月9日
// 修改日期：
// 版本：v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "communication.h"

//-----------------------------------------------------------------------------
//函数声明
//-----------------------------------------------------------------------------
Uint16 ComInit(void);
Uint16 ComChangeState(State state);
Uint16 crc16(Uint16 *puchMsg, Uint16 usDataLen);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
Communication myCommunication;
/* CRC 高位字节值表 */
const unsigned char  auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;
/* CRC低位字节值表*/
const unsigned char  auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;

//-----------------------------------------------------------------------------
// 函数名：void UsemyCommunication(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化myCommunication成员变量
//-----------------------------------------------------------------------------
// 函数说明：并不是所有的编译器都支持有选择的初始化成员变量，因此定义一个函数专门用
//			于对myCommunication的部分成员变量进行初始化。使用myCommunication之前必须要
//			调用此函数。
//-----------------------------------------------------------------------------
// 输入参数：	无
// 输出参数： 	无
//-----------------------------------------------------------------------------
void UsemyCommunication(void)
{
	myCommunication.Enable.all = 0;			//禁止所有通讯模块

	//方法定位
	myCommunication.Init = ComInit;			//初始化
	myCommunication.ChangeState = ComChangeState;

	//调用子模块的使用函数
	UseRS232();				//使用232模块管理器
	UseRS485();				//使用485模块管理器
	UseCan();				//使用CAN模块管理器
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 ComInit(void)
//-----------------------------------------------------------------------------
// 函数功能： 初始化通讯模块所有外设
//-----------------------------------------------------------------------------
// 函数说明：使用的外设为SCIB, SCIC
//-----------------------------------------------------------------------------
// 输入参数：无
// 输出参数：0 -> 初始化成功,1 -> 初始化失败
//-----------------------------------------------------------------------------
Uint16 ComInit(void)			//初始化函数
{
	Uint16 reVal = 0;
	reVal = RS232.Init();		//初始化232外设 -> SCIB
	if(reVal != 0)
		return 1;
	reVal = RS485.Init();		//初始化485外设 -> SCIC
	if(reVal != 0)
		return 1;
	reVal = CAN.Init();			//初始化CAN
	if(reVal)
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 ComChangeState(State state)
//-----------------------------------------------------------------------------
// 函数功能： 改变总通讯模块状态
//-----------------------------------------------------------------------------
// 函数说明：该方式并不会开启任何一个子模块，调用该方法后，需要在分别调用子模块的
//			 使能方法才会使能子模块。但是如果是关闭通讯的话，会将所有子模块都关闭。
//-----------------------------------------------------------------------------
// 输入参数：无
// 输出参数：0 -> 改变完成，1 -> 输入参数错误
//-----------------------------------------------------------------------------
Uint16 ComChangeState(State state)
{
	if(state == Enable)
	{
		myCommunication.Enable.bit.Enable = 1;
	}
	else if(state == Disable)
	{
		myCommunication.Enable.all = 0;
		RS232.ChangeState(state);
		RS485.ChangeState(state);
	}
	else
		return 1;
	return 0;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 Write06Buf(ModuleBuf _md)
//-----------------------------------------------------------------------------
// 函数功能： 将一个浮点数以对应的协议写入到缓冲区中
//-----------------------------------------------------------------------------
// 函数说明：1.如果缓冲区为UART模块: 对应的协议为非标准modbus协议，
//              仅支持06命令，且上位机命令中读取的数据字节数只能为4
//          2.如果缓冲区为CAN模块: 对应的协议共五个字节，第一个字节为控制信号
//            后4个字节为浮点数
//-----------------------------------------------------------------------------
// 输入参数：    dataName -> 数据名称,_md -> 写入的模块，
// 输出参数：    0 -> 输入参数错误,其他 -> 使用缓冲区字节数
//-----------------------------------------------------------------------------
Uint16 Write06Buf(ComModule _md)
{
    Uint16 crcResult = 0;
    Uint16 *buffPtr = 0;

    if(_md == RS232Mdl)
    {
        buffPtr = &RS232.SendBuffer[0];     //写入到232buffer中
        *buffPtr++ = RS232_ADD;         //设备地址
    }
    else if(_md == RS485Mdl)
    {
        buffPtr = &RS485.SendBuffer[0];     //指向485发送buffer
        *buffPtr++ = RS485_ADD;         //设备地址
    }
    else if(_md == CanMdl)
    {
        buffPtr =CAN.SendBuf;           //指向CAN发送buffer
    }

    if((_md == RS232Mdl)|(_md == RS485Mdl)) //Modbus 03命令
    {
        *buffPtr++ = 0x06;              //功能码
        *buffPtr++ = RS485.RecBuffer[2];              //功能码
        *buffPtr++ = RS485.RecBuffer[3];              //功能码
        *buffPtr++ = RS485.RecBuffer[4];              //功能码
        *buffPtr++ = RS485.RecBuffer[5];              //功能码
      //  *buffPtr++ = 4;                 //字节数
       // Float2Uint16(_data,buffPtr);    //[3,4,5,6] -> 浮点数据
      //  buffPtr += 4;

        crcResult =  crc16(RS485.SendBuffer,6); //crc校验
        *buffPtr++ = crcResult>>8;      //crc高位
        *buffPtr = crcResult&0xff;      //crc低位
        return 8;
    }
    else if(_md == CanMdl)      //写入到Canbuffer中
    {
        buffPtr++;          //从第二个缓冲区开始写
      //  Float2Uint16(_data,buffPtr);    //[1,2,3,4] -> 浮点数
        return 5;
    }
    return 0;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 Write16Buf(ModuleBuf _md)
//-----------------------------------------------------------------------------
// 函数功能： 将一个浮点数以对应的协议写入到缓冲区中
//-----------------------------------------------------------------------------
// 函数说明：1.如果缓冲区为UART模块: 对应的协议为非标准modbus协议，
//              仅支持06命令，且上位机命令中读取的数据字节数只能为4
//          2.如果缓冲区为CAN模块: 对应的协议共五个字节，第一个字节为控制信号
//            后4个字节为浮点数
//-----------------------------------------------------------------------------
// 输入参数：    dataName -> 数据名称,_md -> 写入的模块，
// 输出参数：    0 -> 输入参数错误,其他 -> 使用缓冲区字节数
//-----------------------------------------------------------------------------
Uint16 Write16Buf(ComModule _md)
{
    Uint16 crcResult = 0;
    Uint16 *buffPtr = 0;

    if(_md == RS232Mdl)
    {
        buffPtr = &RS232.SendBuffer[0];     //写入到232buffer中
        *buffPtr++ = RS232_ADD;         //设备地址
    }
    else if(_md == RS485Mdl)
    {
        buffPtr = &RS485.SendBuffer[0];     //指向485发送buffer
        *buffPtr++ = RS485_ADD;         //设备地址
    }
    else if(_md == CanMdl)
    {
        buffPtr =CAN.SendBuf;           //指向CAN发送buffer
    }

    if((_md == RS232Mdl)|(_md == RS485Mdl)) //Modbus 03命令
    {
        *buffPtr++ = 0x10;              //功能码
        *buffPtr++ = RS485.RecBuffer[2];              //功能码
        *buffPtr++ = RS485.RecBuffer[3];              //功能码
        *buffPtr++ = RS485.RecBuffer[4];              //功能码
        *buffPtr++ = RS485.RecBuffer[5];              //功能码
      //  *buffPtr++ = 4;                 //字节数
       // Float2Uint16(_data,buffPtr);    //[3,4,5,6] -> 浮点数据
      //  buffPtr += 4;

        crcResult =  crc16(RS485.SendBuffer,6); //crc校验
        *buffPtr++ = crcResult>>8;      //crc高位
        *buffPtr = crcResult&0xff;      //crc低位
        return 8;
    }
    else if(_md == CanMdl)      //写入到Canbuffer中
    {
        buffPtr++;          //从第二个缓冲区开始写
      //  Float2Uint16(_data,buffPtr);    //[1,2,3,4] -> 浮点数
        return 5;
    }
    return 0;
}
//-----------------------------------------------------------------------------
// 函数名：Uint16 Write2Buf(float _data,ModuleBuf _md)
//-----------------------------------------------------------------------------
// 函数功能： 将一个浮点数以对应的协议写入到缓冲区中
//-----------------------------------------------------------------------------
// 函数说明：1.如果缓冲区为UART模块: 对应的协议为非标准modbus协议，
//			    仅支持03命令，且上位机命令中读取的数据字节数只能为4
//			2.如果缓冲区为CAN模块: 对应的协议共五个字节，第一个字节为控制信号
//			  后4个字节为浮点数
//-----------------------------------------------------------------------------
// 输入参数：	dataName -> 数据名称,_md -> 写入的模块，
// 输出参数： 	0 -> 输入参数错误,其他 -> 使用缓冲区字节数
//-----------------------------------------------------------------------------
Uint16 Write2Buf(float _data,ComModule _md)
{
	Uint16 crcResult = 0;
	Uint16 *buffPtr = 0;

	if(_md == RS232Mdl)
	{
		buffPtr = &RS232.SendBuffer[0];		//写入到232buffer中
		*buffPtr++ = RS232_ADD;			//设备地址
	}
	else if(_md == RS485Mdl)
	{
		buffPtr = &RS485.SendBuffer[0];		//指向485发送buffer
		*buffPtr++ = RS485_ADD;			//设备地址
	}
	else if(_md == CanMdl)
	{
		buffPtr =CAN.SendBuf;			//指向CAN发送buffer
	}

	if((_md == RS232Mdl)|(_md == RS485Mdl))	//Modbus 03命令
	{
		*buffPtr++ = 0x03;				//功能码
		*buffPtr++ = 4;					//字节数
		Float2Uint16(_data,buffPtr);	//[3,4,5,6] -> 浮点数据
		buffPtr += 4;

		crcResult =  crc16(RS485.SendBuffer,7);	//crc校验
		*buffPtr++ = crcResult>>8;		//crc高位
		*buffPtr = crcResult&0xff;		//crc低位
		return 9;
	}
	else if(_md == CanMdl)		//写入到Canbuffer中
	{
	    buffPtr++;          //从第二个缓冲区开始写
		Float2Uint16(_data,buffPtr);	//[1,2,3,4] -> 浮点数

		return 5;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 函数名：void Float2Uint16(float _fData,Uint16 *_u16Data)
//-----------------------------------------------------------------------------
// 函数功能： 将浮点数转换到Uint16数组
//-----------------------------------------------------------------------------
// 函数说明：DSP为小端存储，_u16Data[0]存浮点数低字节
//-----------------------------------------------------------------------------
// 输入参数：fData -> 需要转换的浮点数,_u16Data -> 转换后Uint16首地址
// 输出参数： 无
//-----------------------------------------------------------------------------
void Float2Uint16(float _fData,Uint16 *_u16Data)
{
    Uint16 *fPtr = 0;
    fPtr = (Uint16*)(&_fData);
    _u16Data[0] = *fPtr&0x00ff;
    _u16Data[1] = *fPtr>>8;
    _u16Data[2] = *(fPtr+1)&0x00ff;
    _u16Data[3] = *(fPtr+1)>>8;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 crc16(Uint16 *puchMsg, Uint16 usDataLen)
//-----------------------------------------------------------------------------
// 函数功能： 计算modbus crc
//-----------------------------------------------------------------------------
// 函数说明：无
//-----------------------------------------------------------------------------
// 输入参数：puchMsg -> 校验数组指针,usDataLen -> 校验数据个数
// 输出参数： crc结果
//-----------------------------------------------------------------------------
Uint16 crc16(Uint16 *puchMsg, Uint16 usDataLen)
{
	Uint16 uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
	Uint16 uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */
	Uint16 uIndex ; /* CRC循环中的索引 */
	while (usDataLen--) /* 传输消息缓冲区 */
	{
		uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;
}

//-----------------------------------------------------------------------------
// 函数名：Uint16 VerifyFrame(ComModule _md)
//-----------------------------------------------------------------------------
// 函数功能： 验证通讯模块接收到的数据帧格式是否正确
//-----------------------------------------------------------------------------
// 函数说明：modbus协议仅验证03命令
//-----------------------------------------------------------------------------
// 输入参数：_md -> 需要验证的模块
// 输出参数： 0 -> 验证通过, 1 -> 字节数错误,2 -> 校验错误,3 -> 地址不正确
//			 4 -> 功能码不正确, 5 -> 寄存器数量不正确
//-----------------------------------------------------------------------------
Uint16 VerifyFrame(ComModule _md)
{
	Uint16 *bufferPtr = 0;		//指向对应的接收缓冲区
	Uint16 recCrc = 0;	//接收到的crc
	Uint16 calCrc = 0;	//计算出的crc

	if(_md == RS232Mdl)
	{
		if(RS232.RecNum != 8)
			return 1;
		bufferPtr = &RS232.RecBuffer[0];
		if(*bufferPtr != RS232_ADD)
			return 3;			//地址不正确
	}
	else if(_md == RS485Mdl)
	{
		//if(RS485.RecNum != 8)
		//	return 1;
		bufferPtr = &RS485.RecBuffer[0];
		if(*bufferPtr != RS485_ADD)
			return 3;			//地址不正确
	}

	if((_md == RS232Mdl)|(_md == RS485Mdl))
	{
	    //if(*(bufferPtr+1) != 0x03)
	            //  return 4;       //功能码不正确
	            if(*(bufferPtr+1) == 0x03)
	            {
	            Uint16 regNum = (*(bufferPtr+4)<<8)|(*(bufferPtr+5));   //读取寄存器数量
	            if(regNum != 2)
	                return 5;//寄存器数量不正确
	            recCrc = *(bufferPtr+6)<<8;
	            recCrc |= *(bufferPtr+7);           //接收到的crc
	            calCrc = crc16(bufferPtr,6);        //计算的crc
	            if(recCrc != calCrc)
	                return 2;
	            }
	            else if(*(bufferPtr+1) == 0x06)
	            {
	                Uint16 regNum = (*(bufferPtr+2)<<8)|(*(bufferPtr+3));   //读取寄存器数量
	                if(regNum != 0)
	                {
	                    return 5;

	                }
	                recCrc = *(bufferPtr+6)<<8;
	                recCrc |= *(bufferPtr+7);           //接收到的crc
	                calCrc = crc16(bufferPtr,6);        //计算的crc
	                if(recCrc != calCrc)
	                   return 2;

	             //   Uint16 regNum = (*(bufferPtr+2)<<8)|(*(bufferPtr+3));   //读取寄存器数量
	            }
	            else if(*(bufferPtr+1) == 0x10)
	            {
	                Uint16 regNum = (*(bufferPtr+4)<<8)|(*(bufferPtr+5))*2;    //读取寄存器数量
	              //  if(regNum != 0)
	              //  {
	              //      return 5;

	              //  }
	                recCrc = *(bufferPtr+7+regNum)<<8;
	                recCrc |= *(bufferPtr+8+regNum);           //接收到的crc
	                calCrc = crc16(bufferPtr,7+regNum);        //计算的crc
	                if(recCrc != calCrc)
	                   return 2;

	             //   Uint16 regNum = (*(bufferPtr+2)<<8)|(*(bufferPtr+3));   //读取寄存器数量
	            }
	            /*
	            else if(*(bufferPtr+1) == 0x10)
	            {
	                Uint16 regNum = (*(bufferPtr+2)<<8)|(*(bufferPtr+3));   //读取寄存器数量
	                if(regNum != 0)
	                {
	                    return 5;

	                }
	                recCrc = *(bufferPtr+9)<<8;
	                recCrc |= *(bufferPtr+10);           //接收到的crc
	                calCrc = crc16(bufferPtr,9);        //计算的crc
	                if(recCrc != calCrc)
	                   return 2;

	             //   Uint16 regNum = (*(bufferPtr+2)<<8)|(*(bufferPtr+3));   //读取寄存器数量
	            }*/
	            else
	            {
	                return 4;//功能码不正确
	            }
	}
	return 0;
}
















