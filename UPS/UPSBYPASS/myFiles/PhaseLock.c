//----------------------------------------------------------------------------
// �ļ�����PhaseLock.c
//----------------------------------------------------------------------------
// ����:  ����������ͬ��ģ�飬��ģ���������·Ƶ�ʲ��������෽��
//
//----------------------------------------------------------------------------
// ˵���� ʹ����ECap2ģ��
//----------------------------------------------------------------------------
// ���Ŷ��壺GPIO24  ->  ��·A��Ƶ������
// оƬ��  TMS230F28335
//----------------------------------------------------------------------------
// ���ߣ� ���Ǳ�
// ��������: 2016��12��30��
// �޸����ڣ�2017��1��6��
// �汾��v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "PhaseLock.h"

//-----------------------------------------------------------------------------
//��������
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

Uint16 FlagCapEnd = 0;      //������ɱ�־λ
Uint32 ValCap = 0;          //���沶�񵽵�ֵ
Uint32 ValCap5 = 0;          //���沶�񵽵�ֵ

//-----------------------------------------------------------------------------
// ��������void UsePhaseLock(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��myPhaseLock��Ա����
//-----------------------------------------------------------------------------
// ����˵�������������еı�������֧����ѡ��ĳ�ʼ����Ա��������˶���һ������ר����
//          �ڶ�myPhaseLock�Ĳ��ֳ�Ա�������г�ʼ����ʹ��myPhaseLock֮ǰ����Ҫ
//          ���ô˺�����
//-----------------------------------------------------------------------------
// ���������    ��
// ���������    ��
//-----------------------------------------------------------------------------
void UsePhaseLock(void)
{
    myPhaseLock.State = Disable;
    myPhaseLock.StatePL = Enable;
    myPhaseLock.PhaseComp = 0.02;//��·�����ͺ��Լ30��
    myPhaseLock.PSD = NotShift;

    myPhaseLock.Init = PhaseLockInit;
    myPhaseLock.ChangeState = PhaseLockChangeState;
    myPhaseLock.PhaseLockCmd = PhaseLockCmd;
    myPhaseLock.Scan = PhaseLockScan;
    myPhaseLock.CapOverFlowCallback = PLCapOverFlowCallback;
    myPhaseLock.CalOverFlowCallback = PLCalOverFlowCallback;
}

//-----------------------------------------------------------------------------
// ��������Uint16 PhaseLockInit(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ������ģ��
//-----------------------------------------------------------------------------
// ����˵�������������еı�������֧����ѡ��ĳ�ʼ����Ա��������˶���һ������ר����
//          �ڶ�myControlSystem�Ĳ��ֳ�Ա�������г�ʼ����ʹ��myControlSystem֮ǰ����Ҫ
//          ���ô˺�����
//-----------------------------------------------------------------------------
// ���������    ��
// ���������    0 -> ������ɣ�1 -> ECap2�ѱ�ʹ��
//-----------------------------------------------------------------------------
Uint16 PhaseLockInit(void)
{
    if(myHardwarePool.bit.ECap2)
        return 1;

    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 1;            //����Ϊ����1����˿�
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 1;            //����Ϊ����1����˿�
    //ʹ������ʱ��
    SysCtrlRegs.PCLKCR1.bit.ECAP2ENCLK = 1;         //ʹ��ECAP1ʱ��
    SysCtrlRegs.PCLKCR1.bit.ECAP5ENCLK = 1;         //ʹ��ECAP1ʱ��
    EDIS;

    //��ʼ��ECAP1
    ECap2Regs.ECCTL1.bit.FREE_SOFT = 0;             //�����������ֹͣ
    ECap2Regs.ECCTL1.bit.PRESCALE = 0;              //�������벻����
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;               //ʹ�ܲ���
    ECap2Regs.ECCTL1.bit.CTRRST1 = 1;               //����1���������
    ECap2Regs.ECCTL1.bit.CAP1POL = 0;               //����1����������
    ECap2Regs.ECCTL1.bit.CTRRST2 = 1;               //����2���������
    ECap2Regs.ECCTL1.bit.CAP2POL = 0;               //����2����������
    ECap2Regs.ECCTL1.bit.CTRRST3 = 1;               //����3���������
    ECap2Regs.ECCTL1.bit.CAP3POL = 0;               //����3����������
    ECap2Regs.ECCTL1.bit.CTRRST4 = 1;               //����4���������
    ECap2Regs.ECCTL1.bit.CAP4POL = 0;               //����4����������

    ECap2Regs.ECCTL2.bit.CAP_APWM = 0;              //CAPģʽ
    ECap2Regs.ECCTL2.bit.SYNCI_EN = 0;              //��ֹͬ��
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;             //������ֹͣ
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;           //��������ģʽ

    ECap2Regs.ECCLR.all = 0xffff;                   //����жϱ�־λ

    ECap2Regs.TSCTR = 0;                            //����������

    //��ʼ��ECAP5
        ECap5Regs.ECCTL1.bit.FREE_SOFT = 0;             //�����������ֹͣ
        ECap5Regs.ECCTL1.bit.PRESCALE = 0;              //�������벻����
        ECap5Regs.ECCTL1.bit.CAPLDEN = 1;               //ʹ�ܲ���
        ECap5Regs.ECCTL1.bit.CTRRST1 = 1;               //����1���������
        ECap5Regs.ECCTL1.bit.CAP1POL = 0;               //����1����������
        ECap5Regs.ECCTL1.bit.CTRRST2 = 1;               //����2���������
        ECap5Regs.ECCTL1.bit.CAP2POL = 0;               //����2����������
        ECap5Regs.ECCTL1.bit.CTRRST3 = 1;               //����3���������
        ECap5Regs.ECCTL1.bit.CAP3POL = 0;               //����3����������
        ECap5Regs.ECCTL1.bit.CTRRST4 = 1;               //����4���������
        ECap5Regs.ECCTL1.bit.CAP4POL = 0;               //����4����������

        ECap5Regs.ECCTL2.bit.CAP_APWM = 0;              //CAPģʽ
        ECap5Regs.ECCTL2.bit.SYNCI_EN = 0;              //��ֹͬ��
        ECap5Regs.ECCTL2.bit.TSCTRSTOP = 0;             //������ֹͣ
        ECap5Regs.ECCTL2.bit.CONT_ONESHT = 0;           //��������ģʽ

        ECap5Regs.ECCLR.all = 0xffff;                   //����жϱ�־λ

        ECap5Regs.TSCTR = 0;                            //����������

    //�ж�����
        ECap2Regs.ECEINT.bit.CEVT1 = 1;                 //����Ĵ���1�ж�ʹ��
        ECap2Regs.ECEINT.bit.CEVT2 = 1;                 //����Ĵ���2�ж�ʹ��
        ECap2Regs.ECEINT.bit.CEVT3 = 1;                 //����Ĵ���3�ж�ʹ��
        ECap2Regs.ECEINT.bit.CEVT4 = 1;                 //����Ĵ���4�ж�ʹ��
        ECap2Regs.ECEINT.bit.CTROVF = 1;                //����������ж�ʹ��

    ECap5Regs.ECEINT.bit.CEVT1 = 1;                 //����Ĵ���1�ж�ʹ��
    ECap5Regs.ECEINT.bit.CEVT2 = 1;                 //����Ĵ���2�ж�ʹ��
    ECap5Regs.ECEINT.bit.CEVT3 = 1;                 //����Ĵ���3�ж�ʹ��
    ECap5Regs.ECEINT.bit.CEVT4 = 1;                 //����Ĵ���4�ж�ʹ��
    ECap5Regs.ECEINT.bit.CTROVF = 1;                //����������ж�ʹ��


    PieCtrlRegs.PIEIER4.bit.INTx2 = 1;              //ECAP PIE�ж�
    PieCtrlRegs.PIEIER4.bit.INTx5 = 1;              //ECAP PIE�ж�
    IER |= M_INT4;

    return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 PhaseLockChangeState(Uint16 state)
//-----------------------------------------------------------------------------
// �������ܣ� �ı�����ģ��״̬
//-----------------------------------------------------------------------------
// ����˵������ʹ��ecap1
//-----------------------------------------------------------------------------
// ���������    state -> Ƶ�ʼ��״̬
// ���������    0 -> ������ɣ�1 -> ECAP�ѱ�ʹ�ã�2 -> �����δʹ��,3 -> �����������
//-----------------------------------------------------------------------------
Uint16 PhaseLockChangeState(State state)
{
    if((state == Disable)&(myPhaseLock.State==Enable))      //�رո�ģ��
    {
        ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;             //������ֹͣ
        ECap2Regs.TSCTR = 0;                            //����������

        myHardwarePool.bit.ECap2 = 0;       //�ͷŶ�ʱ����Դ
        myPhaseLock.State = Disable;
    }
    else if(state == Enable)
    {
        if(myHardwarePool.bit.ECap2)
            return 1;
        if(myScr.State == Disable)
            return 2;           //�����δʹ��

        ECap2Regs.TSCTR = 0;                            //����������
        ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;             //����������

        ECap5Regs.TSCTR = 0;                            //����������
        ECap5Regs.ECCTL2.bit.TSCTRSTOP = 1;             //����������

        myHardwarePool.bit.ECap2 = 1;                       //ʹ�ö�ʱ����Դ
        myPhaseLock.State = state;
    }
    else
        return 3;
    return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 PhaseLockCmd(State state)
//-----------------------------------------------------------------------------
// �������ܣ� �ı�����״̬
//-----------------------------------------------------------------------------
// ����˵�����ı�����״̬
//-----------------------------------------------------------------------------
// ���������    state -> Ƶ�ʼ��״̬
// ���������    0 -> ������ɣ�1 -> δʹ������ģ�飬2 -> �ز��ȹ���,3 -> �����������
//-----------------------------------------------------------------------------
Uint16 PhaseLockCmd(State state)
{
    if(state == Disable)
        myPhaseLock.StatePL = Disable;
    else if(state == Enable)
    {
        if(myPhaseLock.State == Disable)
            return 1;
        if(myControlSystem.Actuator.CarRatio < 277)     //�ز��ȹ�С������ʹ��ͬ��
            return 2;
        myPhaseLock.StatePL = Enable;
    }
    else
        return 3;
    return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 PhaseLockScan(void)
//-----------------------------------------------------------------------------
// �������ܣ� ɨ���Ƿ��в����������Ҫ�����ú�������Ҫ����һ�β���֮ǰִ��һ�Σ�
//           �����������ݶ�ʧ��
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������    ��
// ���������    0 -> ������һ�����ݣ�1 -> û��������Ҫ����2 -> �ϴβ�����������
//-----------------------------------------------------------------------------
Uint16 PhaseLockScan(void)
{
    if(FlagCapEnd == 0)     //û��������Ҫ����
    {
        return 1;
    }
    FlagCapEnd = 0;

    if(myPhaseLock.Flag.lastCalOF)      //�ϴβ����������������
    {
        myPhaseLock.Flag.lastCalOF = 0;     //�´β����������
        return 2;
    }

    myScr.FrA_BP = 3000000/((ValCap+1)*1.0)*50;                        //����Ƶ��(50Hz��Ӧ3000000)
   //myScr.FrA_INV = 3000000/((ValCap5+1)*1.0)*50;                        //����Ƶ��(50Hz��Ӧ3000000)
/*
    if(myPhaseLock.StatePL == Enable)   //ʹ������ͬ��
    {
        Uint32 _180P = ValCap/2;            //360/2=180���Ӧ����ֵ
        Uint32 _2_8P = ValCap>>8;           //360/2^7 = 2.8125���Ӧ����ֵ

        myPhaseLock.ZeroTimeAct = myPhaseLock.ZeroTimeAct + (Uint32)(ValCap*myPhaseLock.PhaseComp);
        if(myPhaseLock.ZeroTimeAct > ValCap)
            myPhaseLock.ZeroTimeAct = (myPhaseLock.ZeroTimeAct - ValCap);           //������λ����

        myPhaseLock.BP_INV_Phase = myPhaseLock.ZeroTimeAct;
        if((myPhaseLock.ZeroTimeAct > _180P)&&(myPhaseLock.ZeroTimeAct < ValCap))
            myPhaseLock.PSD = RightShift;               //����
        else if((myPhaseLock.ZeroTimeAct > _2_8P)&&(myPhaseLock.ZeroTimeAct < _180P))
            myPhaseLock.PSD = LeftShift;                //����
        else
            myPhaseLock.PSD = NotShift;
    }
    else
        myPhaseLock.PSD = NotShift;                     //���ƶ�
    myPhaseLock.Flag.lastCalOF = 0;                     //�����Ѿ�����
*/
    return 0;
}

void PLCapOverFlowCallback(void)    //��������ص�����
{}

void PLCalOverFlowCallback(void)    //��������ص�����
{}


//-----------------------------------------------------------------------------
//����Ϊ���õ��Ķ�Ӧ�жϳ���
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ��������interrupt void ECap2_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ� T2��ʱ�������ж�
//-----------------------------------------------------------------------------
// ����˵���� ��������ж�ʱ��˵��Ƶ�ʲ��񳬳�������
//-----------------------------------------------------------------------------
// ���������    ��
// ���������    ��
//-----------------------------------------------------------------------------
interrupt void ECap2_ISR(void)
{
    if(FlagCapEnd)      //��һ�β�������û�д���
    {
        myPhaseLock.Flag.lastCalOF = 1;     //�ϴβ�������û�д���
        myPhaseLock.Flag.HisCalOF = 1;      //��ʷ����һ������û�д���
        myPhaseLock.CalOverFlowCallback();  //��һ������û�д���ص�����
    }

    if(ECap2Regs.ECFLG.bit.CEVT1)       //�в���1�ж�
    {
        ValCap = ECap2Regs.CAP1;
        ECap2Regs.ECCLR.bit.CEVT1 = 1;      //�����־λ
    }
    else if(ECap2Regs.ECFLG.bit.CEVT2)      //�в���2�ж�
    {
        ValCap = ECap2Regs.CAP2;
        ECap2Regs.ECCLR.bit.CEVT2 = 1;      //�����־λ
    }
    else if(ECap2Regs.ECFLG.bit.CEVT3)      //�в���3�ж�
    {
        ValCap = ECap2Regs.CAP3;
        ECap2Regs.ECCLR.bit.CEVT3 = 1;      //�����־λ
    }
    else if(ECap2Regs.ECFLG.bit.CEVT4)      //�в���4�ж�
    {
        ValCap = ECap2Regs.CAP4;
        ECap2Regs.ECCLR.bit.CEVT4 = 1;      //�����־λ
    }
    else if(ECap2Regs.ECFLG.bit.CTROVF)     //���������
    {
        myPhaseLock.Flag.CurFreOF = 1;      //��ǵ�ǰƵ�����
        myPhaseLock.Flag.HisFreOF = 1;      //��ʷ����Ƶ�����
        myPhaseLock.CapOverFlowCallback();  //��������ص�����
        ECap2Regs.ECCLR.bit.CTROVF = 1;     //�����־λ
    }

    FlagCapEnd = 1;                         //�������

    //����жϱ�־λ
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    ECap2Regs.ECCLR.bit.INT = 1;            //����ܱ�־λ
    EINT;
}

interrupt void ECap5_ISR(void)
{
    if(FlagCapEnd)      //��һ�β�������û�д���
    {
        myPhaseLock.Flag.lastCalOF = 1;     //�ϴβ�������û�д���
        myPhaseLock.Flag.HisCalOF = 1;      //��ʷ����һ������û�д���
        myPhaseLock.CalOverFlowCallback();  //��һ������û�д���ص�����
    }

    if(ECap5Regs.ECFLG.bit.CEVT1)       //�в���1�ж�
    {
        ValCap5 = ECap5Regs.CAP1;
        ECap5Regs.ECCLR.bit.CEVT1 = 1;      //�����־λ
    }
    else if(ECap5Regs.ECFLG.bit.CEVT2)      //�в���2�ж�
    {
        ValCap5 = ECap5Regs.CAP2;
        ECap5Regs.ECCLR.bit.CEVT2 = 1;      //�����־λ
    }
    else if(ECap5Regs.ECFLG.bit.CEVT3)      //�в���3�ж�
    {
        ValCap5 = ECap5Regs.CAP3;
        ECap5Regs.ECCLR.bit.CEVT3 = 1;      //�����־λ
    }
    else if(ECap5Regs.ECFLG.bit.CEVT4)      //�в���4�ж�
    {
        ValCap5 = ECap5Regs.CAP4;
        ECap5Regs.ECCLR.bit.CEVT4 = 1;      //�����־λ
    }
    else if(ECap5Regs.ECFLG.bit.CTROVF)     //���������
    {
        myPhaseLock.Flag.CurFreOF = 1;      //��ǵ�ǰƵ�����
        myPhaseLock.Flag.HisFreOF = 1;      //��ʷ����Ƶ�����
        myPhaseLock.CapOverFlowCallback();  //��������ص�����
        ECap5Regs.ECCLR.bit.CTROVF = 1;     //�����־λ
    }

    //FlagCapEnd = 1;                         //�������

    //����жϱ�־λ
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    ECap5Regs.ECCLR.bit.INT = 1;            //����ܱ�־λ
    EINT;
}

