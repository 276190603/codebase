//----------------------------------------------------------------------------
// �ļ�����FaultDetect.c
//----------------------------------------------------------------------------
// ����:  ������ϼ��ģ��
//
//----------------------------------------------------------------------------
// ˵���� ��
//----------------------------------------------------------------------------
// ���Ŷ��壺��
// оƬ��	TMS230F2812
//----------------------------------------------------------------------------
// ���ߣ� ���Ǳ�
// ��������: 2017��1��3��
// �޸����ڣ�
// �汾��v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "FaultDetect.h"

//-----------------------------------------------------------------------------
//��������
//-----------------------------------------------------------------------------
Uint16 InsFaultDetAndHandScan(void);
Uint16 RmsFaultDetectScan(void);
Uint16 FaultHandlingScan(void);
Uint16 OtherFaultDetScan(void);
Uint16 ComCnt = 1;
void InsFaultHandling(void);		//˲ʱֵ������

//-----------------------------------------------------------------------------
//��������
//-----------------------------------------------------------------------------
FaultDetect myFaultDetect;


//-----------------------------------------------------------------------------
// ��������void UsemyFaultDetect(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��myFaultDetect��Ա����
//-----------------------------------------------------------------------------
// ����˵�������������еı�������֧����ѡ��ĳ�ʼ����Ա��������˶���һ������ר����
//			�ڶ�myFaultDetect�Ĳ��ֳ�Ա�������г�ʼ����ʹ��myFaultDetect֮ǰ����Ҫ
//			���ô˺�����
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
void UsemyFaultDetect(void)
{
	myFaultDetect.state = Disable;			//Ĭ�ϲ�ʹ�ù��ϼ��
	myFaultDetect.EnableSubFD.all = 0x0;	//�ر�������ģ��
	SUB_ENABLE_BIT.U_BP = 1;
	//���������˲ʱֵ��Χ����  ���뷶Χ190-242
	myFaultDetect.RangeIns.I_SCROut.minValue = -368;
	myFaultDetect.RangeIns.I_SCROut.maxValue = 368;
	//���������
	myFaultDetect.RangeIns.I_TotalOut.minValue = myFaultDetect.RangeIns.I_SCROut.minValue;
	myFaultDetect.RangeIns.I_TotalOut.maxValue = myFaultDetect.RangeIns.I_SCROut.maxValue;
	//ֱ��ĸ��
	myFaultDetect.RangeIns.U_DC.minValue = 280;
	myFaultDetect.RangeIns.U_DC.maxValue = 420;
	//���������ѹ
	myFaultDetect.RangeIns.U_SCROut.minValue = -404;
	myFaultDetect.RangeIns.U_SCROut.maxValue = 404;
	//��·��ѹ
	myFaultDetect.RangeIns.U_BP.minValue = myFaultDetect.RangeIns.U_SCROut.minValue;
	myFaultDetect.RangeIns.U_BP.maxValue = myFaultDetect.RangeIns.U_SCROut.maxValue;

	//�����������Чֵ��Χ����
	myFaultDetect.RangeRms.I_SCROut.minValue = 150;
	myFaultDetect.RangeRms.I_SCROut.maxValue = 220;
	//���������
	myFaultDetect.RangeRms.I_TotalOut.minValue = myFaultDetect.RangeRms.I_SCROut.minValue;
	myFaultDetect.RangeRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_SCROut.maxValue;
	//ֱ��ĸ��
	myFaultDetect.RangeRms.U_DC.minValue = 280;
	myFaultDetect.RangeRms.U_DC.maxValue = 420;
	//���������ѹ
	myFaultDetect.RangeRms.U_SCROut.minValue = 187;//187
	myFaultDetect.RangeRms.U_SCROut.maxValue = 242;
	//��·��ѹ
	myFaultDetect.RangeRms.U_BP.minValue = myFaultDetect.RangeRms.U_SCROut.minValue;
	myFaultDetect.RangeRms.U_BP.maxValue = myFaultDetect.RangeRms.U_SCROut.maxValue;

	//�����������Чֵ�ش���Χ����
	myFaultDetect.BackWinRms.I_SCROut.minValue = 170;
	myFaultDetect.BackWinRms.I_SCROut.maxValue = 200;
	//���������
	myFaultDetect.BackWinRms.I_TotalOut.minValue = myFaultDetect.RangeRms.I_SCROut.minValue;
	myFaultDetect.BackWinRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_SCROut.maxValue;
	//ֱ��ĸ��
	myFaultDetect.BackWinRms.U_DC.minValue = 310;
	myFaultDetect.BackWinRms.U_DC.maxValue = 410;
	//���������ѹ
	myFaultDetect.BackWinRms.U_SCROut.minValue = 170;
	myFaultDetect.BackWinRms.U_SCROut.maxValue = 242;
	//��·��ѹ
	myFaultDetect.BackWinRms.U_BP.minValue = myFaultDetect.RangeRms.U_SCROut.minValue;
	myFaultDetect.BackWinRms.U_BP.maxValue = myFaultDetect.RangeRms.U_SCROut.maxValue;

	//��·Ƶ�ʷ�Χ
	myFaultDetect.Range_FreBP.minValue = 48;
	myFaultDetect.Range_FreBP.maxValue = 52;
	//��·Ƶ�ʻش�
	myFaultDetect.BackWin_FreBP.minValue = 49.8;
	myFaultDetect.BackWin_FreBP.maxValue = 50.2;

	//��־λ����
	myFaultDetect.FlagIns.all = 0;
	myFaultDetect.FlagRms.all = 0;
	myFaultDetect.FlagOther.all = 0;

	//������λ
	myFaultDetect.InsFaultDetAndHandScan = InsFaultDetAndHandScan;
	myFaultDetect.RmsFaultDetectScan = RmsFaultDetectScan;
	myFaultDetect.FaultHandlingScan = FaultHandlingScan;
    myFaultDetect.OtherFaultDetScan = OtherFaultDetScan;
}

//-----------------------------------------------------------------------------
// ��������Uint16 InsFaultDetAndHandScan(void)
//-----------------------------------------------------------------------------
// �������ܣ� ���˲ʱֵ�Ƿ񳬷�Χ�����д���
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> û�д���1 -> ���ִ���,2 -> û��ʹ�ܹ��ϼ��
//-----------------------------------------------------------------------------
Uint16 InsFaultDetAndHandScan(void)		//˲ʱ���ϼ�⼰����ɨ��
{
	if((myFaultDetect.state == 0)|(myFaultDetect.EnableSubFD.all == 0))		//û��ʹ���ܿ��ػ�������ģ�鶼�ر�
	{
		myFaultDetect.FlagIns.all = 0;		//������б�־λ
		return 2;
	}

	if(SUB_ENABLE_BIT.I_SCROut)		//ʹ�ܵ������ģ��
	{
		if((myScr.IA_SCROut < I_SCROUT_MIN)|(myScr.IA_SCROut > I_SCROUT_MAX))
			INSFLAGBIT.IA_SCROut = 1;			//������A�����˲ʱֵ������
		if((myScr.IB_SCROut < I_SCROUT_MIN)|(myScr.IB_SCROut > I_SCROUT_MAX))
			INSFLAGBIT.IB_SCROut = 1;			//B��
		if((myScr.IC_SCROut < I_SCROUT_MIN)|(myScr.IC_SCROut > I_SCROUT_MAX))
			INSFLAGBIT.IC_SCROut = 1;			//C��
	}
	else
	{
		INSFLAGBIT.IA_SCROut = 0;
		INSFLAGBIT.IB_SCROut = 0;
		INSFLAGBIT.IC_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)	//ʹ�ܵ��������ģ����
	{
		if((myScr.IA_TotalOut < I_TOTOUT_MIN)|(myScr.IA_SCROut > I_TOTOUT_MAX))
			INSFLAGBIT.IA_SCROut = 1;			//A���ܵ������˲ʱֵ������
		if((myScr.IB_TotalOut < I_TOTOUT_MIN)|(myScr.IB_SCROut > I_TOTOUT_MAX))
			INSFLAGBIT.IB_SCROut = 1;			//B��
		if((myScr.IC_TotalOut < I_TOTOUT_MIN)|(myScr.IC_SCROut > I_TOTOUT_MAX))
			INSFLAGBIT.IC_SCROut = 1;			//C��
	}
	else
	{
		INSFLAGBIT.IA_TotOut = 0;
		INSFLAGBIT.IB_TotOut = 0;
		INSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)		//ʹ��DCĸ�߼��ģ��
	{
		if((myScr.U_DC < myFaultDetect.RangeIns.U_DC.minValue)|(myScr.U_DC > myFaultDetect.RangeIns.U_DC.maxValue))
			INSFLAGBIT.U_DC = 1;			//DCĸ�߳�������
	}
	else
		INSFLAGBIT.U_DC = 0;

	if(SUB_ENABLE_BIT.U_SCROut)		//ʹ�����������ѹ���
	{
		if((myScr.UA_SCROut < U_SCROUT_MIN)|(myScr.UA_SCROut > U_SCROUT_MAX))
			INSFLAGBIT.UA_SCROut = 1;			//������A�������ѹ˲ʱֵ������
		if((myScr.UB_SCROut < U_SCROUT_MIN)|(myScr.UB_SCROut > U_SCROUT_MAX))
			INSFLAGBIT.UB_SCROut = 1;			//B��
		if((myScr.UC_SCROut < U_SCROUT_MIN)|(myScr.UC_SCROut > U_SCROUT_MAX))
			INSFLAGBIT.UC_SCROut = 1;			//C��
	}
	else
	{
		INSFLAGBIT.UA_SCROut = 0;
		INSFLAGBIT.UB_SCROut = 0;
		INSFLAGBIT.UC_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)		//ʹ����·��ѹ������
	{
		if((myScr.UA_BP < U_BP_MIN)|(myScr.UA_BP > U_BP_MAX))
			INSFLAGBIT.UA_BP = 1;			//A����·�����ѹ˲ʱֵ������
		if((myScr.UB_BP < U_BP_MIN)|(myScr.UB_BP > U_BP_MAX))
			INSFLAGBIT.UB_BP = 1;			//B��
		if((myScr.UC_BP < U_BP_MIN)|(myScr.UC_BP > U_BP_MAX))
			INSFLAGBIT.UC_BP = 1;			//C��
	}
	else
	{
		INSFLAGBIT.UA_BP = 0;
		INSFLAGBIT.UB_BP = 0;
		INSFLAGBIT.UC_BP = 0;
	}
	Uint16 returnValue = 0;
	if(myFaultDetect.FlagIns.all)		//�Ƿ��д���
		returnValue = 1;

	InsFaultHandling();					//������

	return returnValue;
}
Uint16 OtherFaultDetScan(void)         //��Чֵ���ϼ��
{
    if((myFaultDetect.state == 0)|(myFaultDetect.EnableSubFD.all == 0))     //û��ʹ���ܿ��ػ�������ģ�鶼�ر�
    {
        myFaultDetect.FlagOther.all = 0;      //������б�־λ
        return 2;
    }
    if((myScr.FrA_BP<myFaultDetect.Range_FreBP.minValue)|(myScr.FrA_BP>myFaultDetect.Range_FreBP.maxValue))
    {
        FlagOtherBIT.FrA_BP = 1;
    }
    else
    {
        FlagOtherBIT.FrA_BP = 0;
    }

    return 0;

}
//-----------------------------------------------------------------------------
// ��������void InsFaultHandling(void)
//-----------------------------------------------------------------------------
// �������ܣ� �ú���ֱ���ж�����˲ʱֵ��־λ��λ����������Ӧ�Ĵ���
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
void InsFaultHandling(void)
{}

//-----------------------------------------------------------------------------
// ��������Uint16 RmsFaultDetectScan(void)
//-----------------------------------------------------------------------------
// �������ܣ� �����Чֵ�Ƿ񳬳���Χ
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> û�д���1 -> ���ִ���,2 -> û��ʹ�ܹ��ϼ��
//-----------------------------------------------------------------------------
Uint16 RmsFaultDetectScan(void)			//��Чֵ���ϼ��
{

	if((myFaultDetect.state == 0)|(myFaultDetect.EnableSubFD.all == 0))		//û��ʹ���ܿ��ػ�������ģ�鶼�ر�
	{
		myFaultDetect.FlagRms.all = 0;		//������б�־λ
		return 2;
	}

	if(SUB_ENABLE_BIT.I_SCROut)		//ʹ������������������
	{
		if((myScr.IA_SCROutRMS<IRMS_SCROUT_MIN)|(myScr.IA_SCROutRMS>IRMS_SCROUT_MAX))
			RMSFLAGBIT.IA_SCROut = 1;		//�������A�������Чֵ������
		if((myScr.IB_SCROutRMS<IRMS_SCROUT_MIN)|(myScr.IB_SCROutRMS>IRMS_SCROUT_MAX))
			RMSFLAGBIT.IB_SCROut = 1;		//�������B�������Чֵ������
		if((myScr.IC_SCROutRMS<IRMS_SCROUT_MIN)|(myScr.IC_SCROutRMS>IRMS_SCROUT_MAX))
			RMSFLAGBIT.IC_SCROut = 1;		//�������C�������Чֵ������
	}
	else
	{
		RMSFLAGBIT.IA_SCROut = 0;
		RMSFLAGBIT.IB_SCROut = 0;
		RMSFLAGBIT.IB_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)			//ʹ���ܵ���������
	{
		if((myScr.IA_TotalOutRMS<IRMS_TOTOUT_MIN)|(myScr.IA_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IA_TotOut = 1;		//���������A�������Чֵ������
		if((myScr.IB_TotalOutRMS<IRMS_TOTOUT_MIN)|(myScr.IB_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IB_TotOut = 1;		//���������B�������Чֵ������
		if((myScr.IC_TotalOutRMS<IRMS_TOTOUT_MIN)|(myScr.IC_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IC_TotOut = 1;		//���������C�������Чֵ������
	}
	else
	{
		RMSFLAGBIT.IA_TotOut = 0;
		RMSFLAGBIT.IB_TotOut = 0;
		RMSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)			//ʹ��ֱ��ĸ�߼��
	{
		if((myScr.U_DCRMS< IRMS_DC_MIN)|(myScr.U_DCRMS>IRMS_DC_MAX))
			RMSFLAGBIT.U_DC = 1;		//���������A�������Чֵ������
	}
	else
	{
		RMSFLAGBIT.U_DC = 0;
	}

	if(SUB_ENABLE_BIT.U_SCROut)			//ʹ�����������ѹ���
	{
		if((myScr.UA_SCROutRMS<URMS_SCROUT_MIN)|(myScr.UA_SCROutRMS>URMS_SCROUT_MAX))
			RMSFLAGBIT.UA_SCROut = 1;		//����A�������ѹ��Чֵ������
		if((myScr.UB_SCROutRMS<URMS_SCROUT_MIN)|(myScr.UB_SCROutRMS>URMS_SCROUT_MAX))
			RMSFLAGBIT.UB_SCROut = 1;		//����B�������ѹ��Чֵ������
		if((myScr.UC_SCROutRMS<URMS_SCROUT_MIN)|(myScr.UC_SCROutRMS>URMS_SCROUT_MAX))
			RMSFLAGBIT.UC_SCROut = 1;		//����C�������ѹ��Чֵ������
	}
	else
	{
		RMSFLAGBIT.UA_SCROut = 0;
		RMSFLAGBIT.UB_SCROut = 0;
		RMSFLAGBIT.UC_SCROut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)			//ʹ����·�����ѹ���
	{
		if((myScr.UA_BPRMS<URMS_BP_MIN)|(myScr.UA_BPRMS>URMS_BP_MAX))
			RMSFLAGBIT.UA_BP = 1;		//��·A�������ѹ��Чֵ������
		else
            RMSFLAGBIT.UA_BP = 0;       //��·A�������ѹ��Чֵ������
		//if((myScr.UB_BPRMS<URMS_BP_MIN)|(myScr.UB_BPRMS>URMS_BP_MAX))
		//	RMSFLAGBIT.UB_BP = 1;		//��·B�������ѹ��Чֵ������
	//	if((myScr.UC_BPRMS<URMS_BP_MIN)|(myScr.UC_BPRMS>URMS_BP_MAX))
		//	RMSFLAGBIT.UC_BP = 1;		//��·C�������ѹ��Чֵ������
	}
	else
	{
		RMSFLAGBIT.UA_BP = 0;
	//	RMSFLAGBIT.UB_BP = 0;
		//RMSFLAGBIT.UC_BP = 0;
	}

	return 0;
}

Uint16 FaultHandlingScan(void)			//���ϴ���ɨ��
{
    //if(ComCnt)
   // {

  //  ComCnt = 0;
   // }
  //  if(ComCnt)
  //  {
    //    if(FPGA_FLAG.bit.InvRun)

    //}
/*
    if(ComCnt)
    {
        myControlSystem.Sample.Scan();
        myPhaseLock.Scan();
      //  CAN.SendDataWithPtc(2,AutoSendMailBox);   //��������
       // DELAY_US(1000000);
        if( FIRST_START )
        {
            if(myFaultDetect.FlagOther.all||myFaultDetect.FlagRms.all)
            {
                scr_number_switch(scr_4,0);
                scr_number_switch(scr_1,0);
            }
            else
            {
                scr_number_switch(scr_4,0);
                scr_number_switch(scr_1,1);
            }
        }
        else if(FIRST_START == 0)
        {
            scr_number_switch(scr_4,0);
            scr_number_switch(scr_1,0);
        }
        ComCnt = ComCnt + 1;
        if(ComCnt == 10)
        {   ComCnt = 0;}
    }
*/
	return 0;
}

