//----------------------------------------------------------------------------
// �ļ�����Can.c
//----------------------------------------------------------------------------
// ����:  ������Canģ��
//
//----------------------------------------------------------------------------
// ˵���� ʹ����CANBģ��
//----------------------------------------------------------------------------
// ���Ŷ���: 	GPIO20		->		CANTX
//				GPIO21		->		CANRX
// оƬ��		TMS230F28335
//----------------------------------------------------------------------------
// ���ߣ� ���Ǳ�
// ��������: 2017��1��16��
// �޸����ڣ�
// �汾��v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "Can.h"

//-----------------------------------------------------------------------------
//��������
//-----------------------------------------------------------------------------
Uint16 CanInit(void);
Uint16 CanChangeState(State state);
Uint16 ChangeAutoSendState(State state);
Uint16 CanSendData(Uint16 num,MailBoxType mbType);
//Uint16 CanSendDataWithPtc(Uint16 FunCode,float data,MailBoxType mbType);
Uint16 CanSendDataWithPtc(Uint16 FunCode,float data1,float data2,float data3,float data4,float data5,Uint16 data6,MailBoxType mbType) ; //����ͨѶЭ�鷢������
Uint16 CanSendDataWithPtc2(Uint16 FunCode,MailBoxType mbType) ;   //����ͨѶЭ�鷢������
Uint16 CanAutoRecData(void);
Uint16 CanScan(void);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
Uint16 FrameEnd = 0;	//�Ƿ���������Ҫ����
//-----------------------------------------------------------------------------
// ��������void UseCan(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��myCommunication��Can��Ա����
//-----------------------------------------------------------------------------
// ����˵�������������еı�������֧����ѡ��ĳ�ʼ����Ա��������˶���һ������ר����
//			�ڶ�myCommunication�Ĳ��ֳ�Ա�������г�ʼ����ʹ��myCommunication֮ǰ����Ҫ
//			���ô˺�����
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
void UseCan(void)
{
	//����
	myCommunication.CANCom.BaudRate = br1M;			//��ʼ�������� = 1M bps
	CAN.SendId.Id = 0x1;								//��������IDΪ1�봥��ͨѶ����
	CAN.RecId.Id = 0x1;								//��������IDΪ1�봥��ͨѶ����
	CAN.RecIdMask.Id = 0x0000000f;//~0x3;						//����֡ID��0,1λ������ͬ
	CAN.AutoSendId.Id = 4;							//�Զ���������IDΪ4����·��ͨѶ����
	CAN.AutoRecId.Id = 5;							//�Զ���������IDΪ5����·��ͨѶ����
	CAN.Flag.all = 0;								//��־λ���
	CAN.Flag.bit.TxBufEmpty = 1;					//��Ƿ��ͻ�����Ϊ��
	CAN.Flag.bit.AutoTxBufEmpty = 1;				//����Զ����ͻ�����Ϊ��
	CAN.RecNum = 0;									//�������ݸ���Ϊ��
	CAN.AutoRecNum = 0;								//�Զ��������ݸ���Ϊ��

	//����
	myCommunication.CANCom.Init = CanInit;
	CAN.ChangeState = CanChangeState;
	CAN.ChangeAutoSendState = ChangeAutoSendState;
	CAN.SendData = CanSendData;
	CAN.SendDataWithPtc2 = CanSendDataWithPtc2;
	CAN.AutoRecData = CanAutoRecData;
	CAN.Scan = CanScan;
}

//-----------------------------------------------------------------------------
// ��������Uint16 CanInit(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��Canģ��
//-----------------------------------------------------------------------------
// ����˵����ʹ�õ�CANB���裬ʹ��GPIO20��21
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ��ʼ�����,1 -> Ӳ����Դ�ѱ�ʹ��,2 -> ���������ò���ȷ
//-----------------------------------------------------------------------------
Uint16 CanInit(void)
{
	struct ECAN_REGS ECanbShadow;	//CANģ���һЩ�Ĵ���������16λ���ʣ�
									//��ȡ��д����������Ӱ�Ĵ���

	if(myHardwarePool2.bit.Canb)
		return 1;		//CANBģ���ѱ�ʹ��

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=1;    // ʹ��ʱ��eCAN-B
	EDIS;

	//������ӦIO
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

	InitECanb();		//��ʼ��CANBģ������֪״̬,�ú����ɹٷ���׼�ļ��ṩ���������
						//ʹ��CANBģ��IO������ΪECANģʽ����ʼ��������Ϣ������Ϊ�㣻
						//������䷢�ͳɹ������յ������Լ��жϵı�־λ��
						//���ò�����Ϊ1Mbps�����β��������ر����е�����
	EALLOW;
	//���¶������������
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1,��������
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
	{
	    ECanbShadow.CANES.all = ECanbRegs.CANES.all;
	} while(ECanbShadow.CANES.bit.CCE != 1 ); 		// Wait for CCE bit to be  cleared..,�ȴ���������


    ECanbShadow.CANBTC.all = 0;

	/* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock) - default.*/
	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
	ECanbShadow.CANBTC.bit.TSEG1REG = 10;
	//CAN.BaudRate = br500k;
    switch(CAN.BaudRate)
    {
    	case br125k:
    		ECanbShadow.CANBTC.bit.BRPREG = 39;		//CANʱ��Ƶ��Ϊ1875kHz
    		break;
    	case br250k:
    		ECanbShadow.CANBTC.bit.BRPREG = 19;		//CANʱ��Ƶ��Ϊ3750kHz
    		break;
    	case br500k:
    		ECanbShadow.CANBTC.bit.BRPREG = 9;		//CANʱ��Ƶ��Ϊ7500kHz
    		break;
    	case br750k:		//28335�㲻����׼ȷ��800k������
    		ECanbShadow.CANBTC.bit.BRPREG = 4;		//CANʱ��Ƶ��Ϊ15000kHz
    		ECanbShadow.CANBTC.bit.TSEG2REG = 3;
    		ECanbShadow.CANBTC.bit.TSEG1REG = 14;
    		break;
    	case br1M:
    		ECanbShadow.CANBTC.bit.BRPREG = 4;		//CANʱ��Ƶ��Ϊ15000kHz
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
    } while(ECanbShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..,������ɷ�������״̬

    EDIS;

    //��������0Ϊ��������
    ECanbMboxes.MBOX0.MSGID.all = 0;
    ECanbMboxes.MBOX0.MSGID.bit.IDE = 1;				//��չ��ʶ��
    ECanbMboxes.MBOX0.MSGID.all |= CAN.SendId.Id;		//д��id

    ECanbMboxes.MBOX0.MSGCTRL.bit.TPL = 0;				//�������ȼ�,Խ�����ȼ�Խ��

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;		//����Ϊ��������
    ECanbShadow.CANMD.bit.MD0 = 0;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//ʹ������
    ECanbShadow.CANME.bit.ME0 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    //��������1Ϊ��������
    ECanbMboxes.MBOX1.MSGID.all = 0;
    ECanbMboxes.MBOX1.MSGID.bit.IDE = 1;				//��չ��ʶ��
    ECanbMboxes.MBOX1.MSGID.bit.AME = 0;				//ʹ������������λ�Ƚ�
    ECanbMboxes.MBOX1.MSGID.all |= CAN.RecId.Id;		//д��id

    ECanaLAMRegs.LAM1.all = 0;
    ECanaLAMRegs.LAM1.bit.LAMI = 1;						//
    ECanaLAMRegs.LAM1.all |= CAN.RecIdMask.Id;			//д�����η�

    ECanbMboxes.MBOX1.MSGCTRL.bit.TPL = 0;				//�������ȼ�,Խ�����ȼ�Խ��

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;		//����Ϊ��������
    ECanbShadow.CANMD.bit.MD1 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//ʹ������
    ECanbShadow.CANME.bit.ME1 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    //��������2Ϊ�Զ���������
    ECanbMboxes.MBOX2.MSGID.all = 0;
    ECanbMboxes.MBOX2.MSGID.bit.IDE = 1;                //��չ��ʶ��
    ECanbMboxes.MBOX2.MSGID.all |= CAN.AutoSendId.Id;       //д��id

    ECanbMboxes.MBOX2.MSGCTRL.bit.TPL = 0;              //�������ȼ�,Խ�����ȼ�Խ��

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;        //����Ϊ��������
    ECanbShadow.CANMD.bit.MD2 = 0;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;        //ʹ������
    ECanbShadow.CANME.bit.ME2 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

//    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//ʹ������
//    ECanbShadow.CANME.bit.ME2 = 1;
//    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

    //��������3Ϊ�Զ���������
    ECanbMboxes.MBOX3.MSGID.all = 0;
    ECanbMboxes.MBOX3.MSGID.bit.IDE = 1;                //��չ��ʶ��
    ECanbMboxes.MBOX3.MSGID.bit.AME = 0;                //ʹ������������λ�Ƚ�
    ECanbMboxes.MBOX3.MSGID.all |= CAN.AutoRecId.Id;        //д��id

    ECanaLAMRegs.LAM3.all = 0;
    ECanaLAMRegs.LAM3.bit.LAMI = 1;                     //
    ECanaLAMRegs.LAM3.all |= CAN.RecIdMask.Id;          //д�����η�

    ECanbMboxes.MBOX3.MSGCTRL.bit.TPL = 0;              //�������ȼ�,Խ�����ȼ�Խ��

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;        //����Ϊ��������
    ECanbShadow.CANMD.bit.MD3 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;        //ʹ������
    ECanbShadow.CANME.bit.ME3 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
    /*
    ECanbMboxes.MBOX3.MSGID.all = 0;
    ECanbMboxes.MBOX3.MSGID.bit.IDE = 1;				//��չ��ʶ��
    ECanbMboxes.MBOX3.MSGID.all |= CAN.AutoRecId.Id;	//д��id

    ECanbMboxes.MBOX3.MSGCTRL.bit.TPL = 0;				//�������ȼ�,Խ�����ȼ�Խ��
    ECanbMboxes.MBOX3.MSGCTRL.bit.RTR = 1;				//���Է���Զ��֡��ʹ�ø�������ն�Ӧ����֡

    ECanbShadow.CANMD.all = ECanbRegs.CANMD.all;		//����Ϊ��������
    ECanbShadow.CANMD.bit.MD3 = 1;
    ECanbRegs.CANMD.all = ECanbShadow.CANMD.all;

    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//ʹ������
    ECanbShadow.CANME.bit.ME3 = 1;
    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
*/
    EALLOW;

	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.DBO = 0;						//�ȷ������ݵĵ��ֽ�
	ECanbShadow.CANMC.bit.ABO = 1;						//�����Զ���
	ECanbShadow.CANMC.bit.STM = 0;						//���Լ��ģʽ
	ECanbShadow.CANMC.bit.SCB = 1;
	ECanbShadow.CANMC.bit.MBNR = 0x0;					//��������0��д��Ȩ��
	//ECanbShadow.CANMC.bit.SCB = 1;
	ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    //�ж�����
	ECanbRegs.CANGIM.all = 0x3;					// ʹ���ж�0 1
	ECanbRegs.CANMIM.all = 0xf;					// ʹ������0~3�շ��ж�
	ECanbRegs.CANMIL.all = 0xA;					// ����0,2�ж�ӳ�䵽�ж�0 .����1,3�ж�ӳ�䵽1
	ECanbRegs.CANOPC.all = 0;					// ��������Ϣ����

	PieCtrlRegs.PIEIER9.bit.INTx7 = 1;		//ʹ��PIE CANB INT0�ж�
	PieCtrlRegs.PIEIER9.bit.INTx8 = 1;		//ʹ��PIE CANB INT1�ж�
	IER |= M_INT9;    						//����CPU�ж� ��9

	EDIS;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=0;    // ��ʱ�ر�ʱ��ʱ��eCAN-B
	EDIS;

	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 CanChangeState(State state)
//-----------------------------------------------------------------------------
// �������ܣ� �ı�Canģ��״̬
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	state -> ��Ҫ�ı��״̬(Enable,Disable)
// ��������� 	0 -> ��ʼ�����,1 -> Ӳ����Դ�ѱ�ʹ��,2 -> �����������
//-----------------------------------------------------------------------------
Uint16 CanChangeState(State state)
{
	struct ECAN_REGS ECanbShadow;	//CANģ���һЩ�Ĵ���������16λ���ʣ�
									//��ȡ��д����������Ӱ�Ĵ���

	if(myHardwarePool2.bit.Canb)
		return 1;
	if(state == Enable)
	{
		EALLOW;
		SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=1;    // eCAN-B
		EDIS;
		myHardwarePool2.bit.Canb = 1;
		myCommunication.Enable.bit.CanEnable = 1;     //ʹ����ģ��
	}
	else if(state == Disable)
	{
		EALLOW;

	    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//��ֹ�Զ���������
	    ECanbShadow.CANME.bit.ME2 = 0;
	    ECanbRegs.CANME.all = ECanbShadow.CANME.all;

		SysCtrlRegs.PCLKCR0.bit.ECANBENCLK=0;    // eCAN-B
		EDIS;
		myHardwarePool2.bit.Canb = 0;
		myCommunication.Enable.bit.CanEnable = 0;     //ʹ����ģ��
	}
	else
		return 2;

	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 ChangeAutoSendState(State state)
//-----------------------------------------------------------------------------
// �������ܣ� �ı��Զ�����״̬
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	state -> ��Ҫ�ı��״̬(Enable,Disable)
// ��������� 	0 -> ��ʼ�����,1 -> Ӳ����Դ�ѱ�ʹ��,2 -> ��Ч���������
//-----------------------------------------------------------------------------
Uint16 ChangeAutoSendState(State state)
{
	struct ECAN_REGS ECanbShadow;	//CANģ���һЩ�Ĵ���������16λ���ʣ�
									//��ȡ��д����������Ӱ�Ĵ���

	if(myHardwarePool2.bit.Canb)
		return 1;
	if(state == Enable)
	{
		EALLOW;
	    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//ʹ���Զ���������
	    ECanbShadow.CANME.bit.ME2 = 1;
	    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
		EDIS;
	}
	else if(state == Disable)
	{
		EALLOW;
	    ECanbShadow.CANME.all = ECanbRegs.CANME.all;		//��ֹ�Զ���������
	    ECanbShadow.CANME.bit.ME2 = 0;
	    ECanbRegs.CANME.all = ECanbShadow.CANME.all;
		EDIS;
	}
	else
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 CanSendData(Uint16 num,MailBoxType mbType)
//-----------------------------------------------------------------------------
// �������ܣ� ���ͻ�������ǰnum������
//-----------------------------------------------------------------------------
// ����˵�������ʹ�÷�����������̷������ݣ����ʹ���Զ�����������ڽ��յ�һ��Զ��֡
//			������ͣ����ͳɹ���Flag��Ӧ��λ����һ
//-----------------------------------------------------------------------------
// ���������	num -> ��Ҫ���͵����ݸ���,mbType -> ��������(SendMailBox,AutoSendMailBox)
// ��������� 	0 -> ���ͳɹ�,1 -> CANģ��δʹ��,2 -> �����������
//-----------------------------------------------------------------------------
Uint16 CanSendData(Uint16 num,MailBoxType mbType)	//��������
{
	if(myCommunication.Enable.bit.CanEnable == 0)
		return 1;
	if(mbType == SendMailBox)		//��������
	{
		ECanbMboxes.MBOX0.MDL.byte.BYTE0 = CAN.SendBuf[0];		//����д������
		ECanbMboxes.MBOX0.MDL.byte.BYTE1 = CAN.SendBuf[1];
		ECanbMboxes.MBOX0.MDL.byte.BYTE2 = CAN.SendBuf[2];
		ECanbMboxes.MBOX0.MDL.byte.BYTE3 = CAN.SendBuf[3];
		ECanbMboxes.MBOX0.MDH.byte.BYTE4 = CAN.SendBuf[4];
		ECanbMboxes.MBOX0.MDH.byte.BYTE5 = CAN.SendBuf[5];
		ECanbMboxes.MBOX0.MDH.byte.BYTE6 = CAN.SendBuf[6];
		ECanbMboxes.MBOX0.MDH.byte.BYTE7 = CAN.SendBuf[7];

		ECanbMboxes.MBOX0.MSGCTRL.bit.DLC = num;				//���巢�����ݸ���
		ECanbRegs.CANTRS.all = 0x00000001;						//��������
	}
	else if(mbType == AutoSendMailBox)	//�Զ���������
	{
		ECanbMboxes.MBOX2.MDL.byte.BYTE0 = CAN.SendBuf[0];		//����д������
		ECanbMboxes.MBOX2.MDL.byte.BYTE1 = CAN.SendBuf[1];
		ECanbMboxes.MBOX2.MDL.byte.BYTE2 = CAN.SendBuf[2];
		ECanbMboxes.MBOX2.MDL.byte.BYTE3 = CAN.SendBuf[3];
		ECanbMboxes.MBOX2.MDH.byte.BYTE4 = CAN.SendBuf[4];
		ECanbMboxes.MBOX2.MDH.byte.BYTE5 = CAN.SendBuf[5];
		ECanbMboxes.MBOX2.MDH.byte.BYTE6 = CAN.SendBuf[6];
		ECanbMboxes.MBOX2.MDH.byte.BYTE7 = CAN.SendBuf[7];

		ECanbMboxes.MBOX2.MSGCTRL.bit.DLC = num;				//���巢�����ݸ���
		ECanbRegs.CANTRS.all = 0x00000004;                      //��������
	}
	else
		return 2;
	return 0;
}

Uint16 CanSendDataWithPtc2(Uint16 FunCode,MailBoxType mbType)    //����ͨѶЭ�鷢������
{
    Uint16 reVal = 0;
    CAN.SendBuf[0] = FunCode;          //��д������
    reVal = CanSendData(1,mbType);      //��������
    return reVal;
}
Uint16 CanSendDataWithPtc3(Uint16 FunCode,MailBoxType mbType)    //����ͨѶЭ�鷢������
{
    Uint16 reVal = 0;
    CAN.SendBuf[0] = 2;          //��д������
    CAN.SendBuf[1] = FunCode;          //��д������
    reVal = CanSendData(2,mbType);      //��������
    return reVal;
}
//-----------------------------------------------------------------------------
// ��������Uint16 CanSendDataWithPtc(float data,MailBoxType mbType)
//-----------------------------------------------------------------------------
// �������ܣ� ����ͨѶЭ�鷢������
//-----------------------------------------------------------------------------
// ����˵�������ʹ�÷�����������̷������ݣ����ʹ���Զ�����������ڽ��յ�һ��Զ��֡
//			������ͣ����ͳɹ���Flag��Ӧ��λ����һ
//-----------------------------------------------------------------------------
// ���������	FunCode -> ������,data -> ��Ҫ���͵�����,mbType -> ��������(SendMailBox,AutoSendMailBox)
// ��������� 	0 -> ���ͳɹ�,1 -> CANģ��δʹ��,2 -> �����������
//-----------------------------------------------------------------------------
Uint16 CanSendDataWithPtc(Uint16 FunCode,float data1,float data2,float data3,float data4,float data5,Uint16 data6,MailBoxType mbType)	//����ͨѶЭ�鷢������
{
	Uint16 reVal = 0;
	//Write2Buf(data,CanMdl);				//��Э�齫���ݷ��뻺������
	CAN.SendBuf[0] = (Uint16)data1;			//��д������
	CAN.SendBuf[1] = (Uint16)data2;           //��д������
	CAN.SendBuf[2] = (Uint16)data3;           //��д������
	CAN.SendBuf[3] = (Uint16)data4;           //��д������
	CAN.SendBuf[4] = (Uint16)data5;           //��д������
    CAN.SendBuf[5] = (Uint16)data6;           //��д������
	reVal = CanSendData(6,mbType);		//��������
	return reVal;
}

//-----------------------------------------------------------------------------
// ��������Uint16 CanAutoRecData(void)
//-----------------------------------------------------------------------------
// �������ܣ� ����һ��Զ��֡�����Զ���������֡
//-----------------------------------------------------------------------------
// ����˵����������յ���Ӧ������֡��Flag�е���Ӧλ����һ
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ����ɹ�,1 -> CANģ��δʹ��
//-----------------------------------------------------------------------------
Uint16 CanAutoRecData(void)
{
	if(myCommunication.Enable.bit.CanEnable == 0)
		return 1;
	ECanaRegs.CANTRS.all = 0x00000008;						//��������
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 CanScan(void)
//-----------------------------------------------------------------------------
// �������ܣ�ѭ��ɨ����û�������账��
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ������һ֡����,1 -> û��������Ҫ����,2 -> δʶ��Ĺ�����
//-----------------------------------------------------------------------------
Uint16 CanScan(void)		//ɨ�������ж���Ҫ����
{
	if(FrameEnd == 0)
		return 1;
	if(CAN.Flag.bit.RecFrame)		//���ջ�����������
	{
		if(CAN.RecNum == 5)		//���յ�����ֽ�
		{
			switch(CAN.RecBuf[0])
			{
				case FRA_BP_ADD:		//������A���ѹ
					CanSendDataWithPtc(FRA_BP_ADD,FPGA_FLAG.bit.InvRun,myInverter.UA_InvOutRMS,myPhaseLock.PSD_BYP,myInverter.FrA_INV,myInverter.IA_TotalOutRMS,SOFT_START,SendMailBox);	//�������� FPGA_FLAG.bit.InvRun
					break;
                case INV_START:        //������A���ѹ
                    myInverter.SOFTSTART = CAN.RecBuf[1];
                    break;
				default:
					return 2;
			}
		}

		CAN.Flag.bit.RecFrame = 0;	//�����־λ
	}
	else if(CAN.Flag.bit.AutoRecFrame)	//�Զ�����һ֡����
	{
        if(CAN.AutoRecNum == 1)     //���յ�����ֽ�
        {
            switch(CAN.AutoRecBuf[0])
            {
                case 0:        //������A���ѹ
                    if((myPhaseLock.PSD_BYP == NoShift)&&(myFaultDetect.FlagOther.all==0)&&(myFaultDetect.FlagRms.all==0))
                    myFpgaDriver.InvSwitchOn(Enable);
                    break;
                case 1:        //������A���ѹ
                   // if((myPhaseLock.PSD == NoneShift)&&(myFaultDetect.FlagOther.all==0)&&(myFaultDetect.FlagRms.all==0))
                    myFpgaDriver.InvSwitchOn(Disable);
                    break;
                case 2:
                    CanSendDataWithPtc3(!FPGA_FLAG.bit.InvRun,AutoSendMailBox);   //��������· SOFT_START FPGA_FLAG.bit.InvRun
                    break;
                default:
                    return 2;
            }
        }
		CAN.Flag.bit.AutoRecFrame = 0;	//�����־λ
	}
	return 0;
}

//-----------------------------------------------------------------------------
//����Ϊ���õ��Ķ�Ӧ�жϳ���
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ��������interrupt void CanbInt0_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ�Canb Int0�ж�
//-----------------------------------------------------------------------------
// ����˵��������(0)���Զ�����(2)�����ж�ӳ�䵽���ж�
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void CanbInt0_ISR(void)
{
	struct ECAN_REGS ECanbShadow;

	ECanbShadow.CANGIF0.all = ECanbRegs.CANGIF0.all;
	if(ECanbShadow.CANGIF0.bit.MIV0 == 0)		//����0�ж�
	{
		CAN.Flag.bit.TxBufEmpty = 1;		//������ɣ���ǻ�������
		ECanbRegs.CANTA.all = 1;			//�����־λ
	}
	else if(ECanbShadow.CANGIF0.bit.MIV0 == 2)	//����2�ж�
	{
		CAN.Flag.bit.AutoTxBufEmpty = 1;	//�Զ�������ɣ���ǻ�������
		ECanbRegs.CANTA.all = 0x4;			//�����־λ
	}
	PieCtrlRegs.PIEACK.all|=PIEACK_GROUP9;  	// Issue PIE ack
}

//-----------------------------------------------------------------------------
// ��������interrupt void CanbInt1_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ�Canb Int1�ж�
//-----------------------------------------------------------------------------
// ����˵��������(1)���Զ�����(3)�����ж�ӳ�䵽���ж�
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void CanbInt1_ISR(void)
{
	struct ECAN_REGS ECanbShadow;

	FrameEnd = 1;			//��һ֡������Ҫ����
	ECanbShadow.CANGIF1.all = ECanbRegs.CANGIF1.all;
	if(ECanbShadow.CANGIF1.bit.MIV1 == 1)		//����������յ�����
	{
		CAN.RecBuf[0] = ECanbMboxes.MBOX1.MDL.byte.BYTE0;
		CAN.RecBuf[1] = ECanbMboxes.MBOX1.MDL.byte.BYTE1;
		CAN.RecBuf[2] = ECanbMboxes.MBOX1.MDL.byte.BYTE2;
		CAN.RecBuf[3] = ECanbMboxes.MBOX1.MDL.byte.BYTE3;
		CAN.RecBuf[4] = ECanbMboxes.MBOX1.MDH.byte.BYTE4;
		CAN.RecBuf[5] = ECanbMboxes.MBOX1.MDH.byte.BYTE5;
		CAN.RecBuf[6] = ECanbMboxes.MBOX1.MDH.byte.BYTE6;
		CAN.RecBuf[7] = ECanbMboxes.MBOX1.MDH.byte.BYTE7;
		CAN.RecNum = ECanbMboxes.MBOX1.MSGCTRL.bit.DLC;		//��¼���յ��ֽ���
		CAN.Flag.bit.RecFrame = 1;		//��ǽ��յ�һ֡����

		ECanbRegs.CANRMP.all = 2;		//����жϱ�־λ
	}
	else if(ECanbShadow.CANGIF1.bit.MIV1 == 3)	//�Զ�����������յ�����
	{
		CAN.AutoRecBuf[0] = ECanbMboxes.MBOX3.MDL.byte.BYTE0;
		CAN.AutoRecBuf[1] = ECanbMboxes.MBOX3.MDL.byte.BYTE1;
		CAN.AutoRecBuf[2] = ECanbMboxes.MBOX3.MDL.byte.BYTE2;
		CAN.AutoRecBuf[3] = ECanbMboxes.MBOX3.MDL.byte.BYTE3;
		CAN.AutoRecBuf[4] = ECanbMboxes.MBOX3.MDH.byte.BYTE4;
		CAN.AutoRecBuf[5] = ECanbMboxes.MBOX3.MDH.byte.BYTE5;
		CAN.AutoRecBuf[6] = ECanbMboxes.MBOX3.MDH.byte.BYTE6;
		CAN.AutoRecBuf[7] = ECanbMboxes.MBOX3.MDH.byte.BYTE7;
		CAN.AutoRecNum = ECanbMboxes.MBOX3.MSGCTRL.bit.DLC;		//��¼���յ��ֽ���
		CAN.Flag.bit.AutoRecFrame = 1;		//��ǽ��յ�һ֡����

		ECanbRegs.CANRMP.all = 8;		//����жϱ�־λ
	}
	PieCtrlRegs.PIEACK.all|=PIEACK_GROUP9;  	// Issue PIE ack
}



