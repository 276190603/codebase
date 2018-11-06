//----------------------------------------------------------------------------
// �ļ�����RS232.c
//----------------------------------------------------------------------------
// ����:  ������RS232ģ��
//
//----------------------------------------------------------------------------
// ˵���� ʹ����SCIBģ��
//----------------------------------------------------------------------------
// ���Ŷ��壺GPIO22	->	232TX
//			GPIO23	->	232RX
// оƬ��	TMS230F28335
//----------------------------------------------------------------------------
// ���ߣ� ���Ǳ�
// ��������: 2017��1��9��
// �޸����ڣ�
// �汾��v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "RS232.h"

//-----------------------------------------------------------------------------
//��������
//-----------------------------------------------------------------------------
Uint16 RS232Init(void);
Uint16 RS232ChangeState(State state);
Uint16 RS232SendData(Uint16 num);
Uint16 Rs232SendRemain(void);
Uint16 RS232MB03Return(float _data);
Uint16 RS232Scan(void);
//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
Uint16 RemainSendNum = 0;		//ʣ����Ҫ�������ݵĸ���
Uint16 TxBufferPtr = 0;			//���ͻ�����ָ�룬ʼ��ָ����һ��Ҫ���͵�����

//-----------------------------------------------------------------------------
// ��������void UseRS232(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��myCommunication��RS232��Ա����
//-----------------------------------------------------------------------------
// ����˵�������������еı�������֧����ѡ��ĳ�ʼ����Ա��������˶���һ������ר����
//			�ڶ�myCommunication�Ĳ��ֳ�Ա�������г�ʼ����ʹ��myCommunication֮ǰ����Ҫ
//			���ô˺�����
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
void UseRS232(void)
{
	RS232.BaudRate = 9600;
	RS232.DataBits = 8;
	RS232.StopBits = 1;
	RS232.Parity = NoP;
	RS232.RecNum = 0;
	RS232.Flag.all = 0;		//������б�־λ
	RS232.Flag.bit.SendEmptyFlag = 1;		//���ͻ�������

	RS232.Init = RS232Init;
	RS232.ChangeState = RS232ChangeState;
	RS232.SendData = RS232SendData;
	RS232.MB03Return = RS232MB03Return;
	RS232.Scan = RS232Scan;
}

//-----------------------------------------------------------------------------
// ��������Uint16 RS232Init(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��RS232ģ��
//-----------------------------------------------------------------------------
// ����˵����ʹ�õ�SCIBģ��,GPIO22��23
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ��ʼ���ɹ�,1 -> Ӳ����Դ��ռ��,2 -> �����ʲ�������
//				3 -> ����λ��������,4 -> ֹͣλ��������,5 -> У���������
//-----------------------------------------------------------------------------
Uint16 RS232Init(void)
{
	if(myHardwarePool2.bit.Scib)
		return 1;			//Ӳ����Դ��ʹ��

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;		//ʹ������ʱ��
	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;		//����ΪSCIB TX
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;		//����ΪSCIB RX
	EDIS;

	if((RS232.BaudRate >= 2000)&(RS232.BaudRate <= 40000))		//������
	{
		float temp = LSPCLK28xx*1.0/RS232.BaudRate/8;
		Uint16 brr = temp*1000 - 1;
		ScibRegs.SCIHBAUD = brr>>8;
		ScibRegs.SCILBAUD = brr&0xff;
	}
	else
		return 2;

	if((RS232.DataBits >= 1)&(RS232.DataBits <= 8))			//����λ
		ScibRegs.SCICCR.bit.SCICHAR = RS232.DataBits-1;
	else
		return 3;

	if(RS232.StopBits == 1)									//ֹͣλ
		ScibRegs.SCICCR.bit.STOPBITS = 0;
	else if(RS232.StopBits == 2)
		ScibRegs.SCICCR.bit.STOPBITS = 1;
	else
		return 4;

	switch(RS232.Parity)									//У��λ
	{
		case NoP:
			ScibRegs.SCICCR.bit.PARITYENA = 0;
			break;
		case OddP:
			ScibRegs.SCICCR.bit.PARITYENA = 1;
			ScibRegs.SCICCR.bit.PARITY = 0;
			break;
		case EvenP:
			ScibRegs.SCICCR.bit.PARITYENA = 1;
			ScibRegs.SCICCR.bit.PARITY = 1;
			break;
		default:
			return 5;
	}

	ScibRegs.SCICCR.bit.LOOPBKENA = 0;		//��ֹ�ػ�ģʽ
	ScibRegs.SCICCR.bit.ADDRIDLE_MODE = 0;	//������ģʽ

//	ScibRegs.SCICTL1.bit.TXENA = 1;			//ʹ�ܷ���
//	ScibRegs.SCICTL1.bit.RXENA = 1;			//ʹ�ܽ���
	ScibRegs.SCICTL1.bit.SWRESET = 0;		//��λSCI

	ScibRegs.SCIFFTX.bit.SCIRST = 1;		//���¿�ʼFIFO
	ScibRegs.SCIFFTX.bit.SCIFFENA = 1;		//ʹ��FIFO
	ScibRegs.SCIFFTX.bit.TXFFIL = 1;		//ʣ�෢���ֽ�С�ڵ���1ʱ�������ж�

	ScibRegs.SCIFFRX.bit.RXFFIL = 1;		//���յ�һ���ֽھͲ����ж�

	ScibRegs.SCICTL1.bit.SWRESET = 1;		//��λ��ϣ�ʹ��SCI

	ScibRegs.SCIFFTX.bit.TXFIFOXRESET = 1;	//��λFIFO����ָ��
	ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;	//��λFIFO����ָ��

	PieCtrlRegs.PIEIER9.bit.INTx4 = 1;		//ʹ��PIE SCIB TX�ж�
	PieCtrlRegs.PIEIER9.bit.INTx3 = 1;		//ʹ��PIE SCIB RX�ж�
	IER |= M_INT9;    						//����CPU�ж� ��9

	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 RS232ChangeState(State state)
//-----------------------------------------------------------------------------
// �������ܣ� �ı�232ģ���״̬
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	state -> ��Ҫ���õ�״̬
// ��������� 	0 -> ���óɹ�,1 -> Ӳ���ѱ�ռ��,2 -> ��ͨѶʹ��Ϊ����
//				3 -> �����������
//-----------------------------------------------------------------------------
Uint16 RS232ChangeState(State state)	//�ı�RS232ͨѶģ��״̬
{
	if(state == Enable)
	{
		if(myHardwarePool2.bit.Scib)
			return 1;			//Ӳ����Դ��ʹ��
		if(myCommunication.Enable.bit.Enable == 0)
			return 2;			//��ͨѶʹ��δ����

		ScibRegs.SCICTL1.bit.TXENA = 1;			//ʹ�ܷ���
		ScibRegs.SCICTL1.bit.RXENA = 1;			//ʹ�ܽ���

		ScibRegs.SCICTL2.bit.TXINTENA = 1;		//ʹ�ܷ����ж�	FIFOģʽ�������жϱ������ˣ����Բ�����Ҳ����
		ScibRegs.SCICTL2.bit.RXBKINTENA = 1;	//ʹ�ܽ����ж�

		ScibRegs.SCIFFTX.bit.TXFFIENA = 1;		//ʹ��FIFO�����ж�
		ScibRegs.SCIFFRX.bit.RXFFIENA = 1;		//ʹ��FIFO�����ж�

		myHardwarePool2.bit.Scib = 1;			//Ӳ����Դ��ռ��
		myCommunication.Enable.bit.Rs232Enable = 1;		//ʹ����232ģ��
	}
	else if(state == Disable)
	{
		ScibRegs.SCICTL1.bit.TXENA = 0;			//��ֹ����
		ScibRegs.SCICTL1.bit.RXENA = 0;			//��ֹ����

		ScibRegs.SCICTL2.bit.TXINTENA = 0;		//��ֹ�����ж�
		ScibRegs.SCICTL2.bit.RXBKINTENA = 0;	//��ֹ�����ж�

		ScibRegs.SCIFFTX.bit.TXFFIENA = 0;		//��ֹFIFO�����ж�
		ScibRegs.SCIFFRX.bit.RXFFIENA = 0;		//��ֹFIFO�����ж�

		myHardwarePool2.bit.Scib = 0;					//�ͷ�Ӳ����Դ
		myCommunication.Enable.bit.Rs232Enable = 0;		//ʧ��232ģ��
	}
	else
		return 3;

	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 RS232SendData(Uint16 num)
//-----------------------------------------------------------------------------
// �������ܣ� ���ͻ�������ǰnum������
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	num -> ��Ҫ���͵�����
// ��������� 	0 -> ���ͳɹ�,1 -> num���ڻ���������ֽ���,2 -> ��һ������δ������
//				3 -> ģ��Ϊʹ��
//-----------------------------------------------------------------------------
Uint16 RS232SendData(Uint16 num)		//���ͻ�������ǰnum������
{
	if(num > SCI_TXBUFFER_SIZE)
		return 1;
	if(RS232.Flag.bit.SendEmptyFlag == 0)
		return 2;
	if(myCommunication.Enable.bit.Rs232Enable == 0)
		return 3;
	RemainSendNum = num;
	TxBufferPtr = 0;			//ָ�򻺳����е�һ������
	Rs232SendRemain();			//����һ������
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uin16 Rs232SendRemain(void)
//-----------------------------------------------------------------------------
// �������ܣ����ͻ�������д��10�����ݣ����������ȫд��
//-----------------------------------------------------------------------------
// ����˵����д��ɹ������Ӧ���޸�ָ��TxBufferPtr��ʣ�෢�͵�����RemainSendNum
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> д��ɹ�,1 -> ���͵����ݳ�����������С,2 -> û����ѧ��Ҫ����
//-----------------------------------------------------------------------------
Uint16 Rs232SendRemain(void)				//���ͻ�������ʣ�µ�����
{
	if((RemainSendNum+TxBufferPtr) > SCI_TXBUFFER_SIZE)
		return 1;						//���͵����ݳ���������
	if(RemainSendNum == 0)
		return 2;

	RS232.Flag.bit.SendEmptyFlag = 0;		//��ʼ�������ݣ����ͻ���������
	if(RemainSendNum >= FIFO_SEND_NUM)
	{
		int i = 0;
		for(i = 0;i < FIFO_SEND_NUM;i++)		//��������������
		{
			ScibRegs.SCITXBUF = RS232.SendBuffer[TxBufferPtr++];
			RemainSendNum--;
		}
	}
	else
	{
		while(RemainSendNum)		//��ʣ�������ȫ������
		{
			ScibRegs.SCITXBUF = RS232.SendBuffer[TxBufferPtr++];
			RemainSendNum--;
		}
	}
	ScibRegs.SCIFFTX.bit.TXFFIENA = 1;		//ʹ��FIFO�����ж�
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 RS232MB03Return(Uint16 dataName)
//-----------------------------------------------------------------------------
// �������ܣ���Modbus 03�����ֵ��֡��ʽ��������
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	_data -> ��Ҫ���͵ĸ�����
// ��������� 	0 -> д��ɹ�,1 -> ���͵����ݳ�����������С,2 -> û����ѧ��Ҫ����
//-----------------------------------------------------------------------------
Uint16 RS232MB03Return(float _data)		//ʹ��modbus 03����ظ�ʽ��������
{
	Uint16 sendNum = 0;
	sendNum = Write2Buf(_data,RS232Mdl);			//������д�뵽��������
	RS232SendData(sendNum);							//��������
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 RS232Scan(void)
//-----------------------------------------------------------------------------
// �������ܣ�ѭ��ɨ����û����Ҫ����Ľ�������֡
//-----------------------------------------------------------------------------
// ����˵���������յ�������֡��ʽΪmodbus 03����ʱ���������ݴ���
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ������һ֡����,1 -> û����Ч������֡,2 -> ֡��ʽ����
//				3 -> ��Ч�����ݵ�ַ
//-----------------------------------------------------------------------------
Uint16 RS232Scan(void)					//232ɨ�躯��
{
	Uint16 returnVal = 0;

	if(RS232.Flag.bit.RecFrameFlag == 0)
		return 1;
	if(VerifyFrame(RS232Mdl) == 0)		//��֤��ַ�������롢��ȡ�Ĵ���������У���Ƿ���ȷ
	{
		Uint16 dataAdd = (RS232.RecBuffer[2]<<8)|(RS232.RecBuffer[3]);		//��ȡ���ݵĵ�ַ
		switch(dataAdd)
		{
			case UA_INVOUT_ADD:			//A�������ѹ
				RS232MB03Return(myInverter.UA_InvOut);		//��modbus 03 �������ʽ��������
				break;
			case UA_INVOUTR_ADD:		//���A�������ѹ��Чֵ
				RS232MB03Return(myInverter.UA_InvOutRMS);		//��modbus 03 �������ʽ��������
				break;
			default:
				returnVal = 3;
		}
	}
	else
		returnVal = 2;
	RS232.Flag.bit.RecOverflow = 0;		//���Ϊû�����
	RS232.RecNum = 0;					//�յ������ݸ�������
	RS232.Flag.bit.RecFrameFlag = 0;	//��ǻ�����û������֡
	return returnVal;
}

//-----------------------------------------------------------------------------
//����Ϊ���õ��Ķ�Ӧ�жϳ���
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ��������interrupt ScibTx_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ�SCIB�����ж�
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void ScibTx_ISR(void)
{
	if(RemainSendNum == 0)		//����ȫ��д�����
	{
		RS232.Flag.bit.SendEnd = 1;		//���ݴ������
		ScibRegs.SCIFFTX.bit.TXFFIENA = 0;		//��ֹFIFO�����ж�
	}

	Rs232SendRemain();			//����ʣ�������

	ScibRegs.SCIFFTX.bit.TXFFINTCLR = 1;		//�����־λ
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;		//��ӦPIEͬ���ж�
}

//-----------------------------------------------------------------------------
// ��������interrupt void ScibRx_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ�SCIB�����ж�
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void ScibRx_ISR(void)
{

	if(RS232.Flag.bit.RecOverflow)		//�����������
	{
		RS232.Flag.bit.RecData = 1;			//��ǽ��յ�������
	}
	else if(!RS232.Flag.bit.RecFrameFlag)		//��������û������֡
	{
		RS232.RecBuffer[RS232.RecNum++] = ScibRegs.SCIRXBUF.all;	//��ȡ����

		if(RS232.RecNum >= SCI_RXBUFFER_SIZE)
			RS232.Flag.bit.RecOverflow = 1;		//�������
		RS232.Flag.bit.RecData = 1;			//��ǽ��յ�������
	}

	ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1; 		 	// Clear Overflow flag
	ScibRegs.SCIFFRX.bit.RXFFINTCLR=1; 			// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all|=PIEACK_GROUP9;  	// Issue PIE ack
}

