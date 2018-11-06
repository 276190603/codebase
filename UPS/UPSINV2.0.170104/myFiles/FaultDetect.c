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
Uint16 LastFaultFlag = 0;
Uint16 NowFaultFlag = 0;
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

	//��������˲ʱֵ��Χ����
	myFaultDetect.RangeIns.I_InvOut.minValue = -368;
	myFaultDetect.RangeIns.I_InvOut.maxValue = 368;
	//���������
	myFaultDetect.RangeIns.I_TotalOut.minValue = myFaultDetect.RangeIns.I_InvOut.minValue;
	myFaultDetect.RangeIns.I_TotalOut.maxValue = myFaultDetect.RangeIns.I_InvOut.maxValue;
	//ֱ��ĸ��
	myFaultDetect.RangeIns.U_DC.minValue = 280;
	myFaultDetect.RangeIns.U_DC.maxValue = 420;
	//��������ѹ
	myFaultDetect.RangeIns.U_InvOut.minValue = -404;
	myFaultDetect.RangeIns.U_InvOut.maxValue = 404;
	//��·��ѹ
	myFaultDetect.RangeIns.U_BP.minValue = myFaultDetect.RangeIns.U_InvOut.minValue;
	myFaultDetect.RangeIns.U_BP.maxValue = myFaultDetect.RangeIns.U_InvOut.maxValue;

	//����������Чֵ��Χ����
	myFaultDetect.RangeRms.I_InvOut.minValue = 150;
	myFaultDetect.RangeRms.I_InvOut.maxValue = 220;
	//���������
	myFaultDetect.RangeRms.I_TotalOut.minValue = myFaultDetect.RangeRms.I_InvOut.minValue;
	myFaultDetect.RangeRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_InvOut.maxValue;
	//ֱ��ĸ��
	myFaultDetect.RangeRms.U_DC.minValue = 176;//176
	myFaultDetect.RangeRms.U_DC.maxValue = 264;
	//��������ѹ
	myFaultDetect.RangeRms.U_InvOut.minValue = 187;//187
	myFaultDetect.RangeRms.U_InvOut.maxValue = 242;
	//��·��ѹ
	myFaultDetect.RangeRms.U_BP.minValue = myFaultDetect.RangeRms.U_InvOut.minValue;
	myFaultDetect.RangeRms.U_BP.maxValue = myFaultDetect.RangeRms.U_InvOut.maxValue;

	//����������Чֵ�ش���Χ����
	myFaultDetect.BackWinRms.I_InvOut.minValue = 170;
	myFaultDetect.BackWinRms.I_InvOut.maxValue = 200;
	//���������
	myFaultDetect.BackWinRms.I_TotalOut.minValue = myFaultDetect.RangeRms.I_InvOut.minValue;
	myFaultDetect.BackWinRms.I_TotalOut.maxValue = myFaultDetect.RangeRms.I_InvOut.maxValue;
	//ֱ��ĸ��
	myFaultDetect.BackWinRms.U_DC.minValue = 310;
	myFaultDetect.BackWinRms.U_DC.maxValue = 410;
	//��������ѹ
	myFaultDetect.BackWinRms.U_InvOut.minValue = 190;
	myFaultDetect.BackWinRms.U_InvOut.maxValue = 240;
	//��·��ѹ
	myFaultDetect.BackWinRms.U_BP.minValue = myFaultDetect.RangeRms.U_InvOut.minValue;
	myFaultDetect.BackWinRms.U_BP.maxValue = myFaultDetect.RangeRms.U_InvOut.maxValue;

	//��·Ƶ�ʷ�Χ
	myFaultDetect.Range_FreBP.minValue = 48;//48
	myFaultDetect.Range_FreBP.maxValue = 52;//52
	//��·Ƶ�ʻش�
	myFaultDetect.BackWin_FreBP.minValue = 49.8;
	myFaultDetect.BackWin_FreBP.maxValue = 50.2;

	//��־λ����
	myFaultDetect.FlagIns.all = 0;
	myFaultDetect.FlagRms.all = 0;
	myFaultDetect.FlagOther.all = 0;

	//������λ
	RMSFLAGBIT.U_DC = 0;
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

	if(SUB_ENABLE_BIT.I_InvOut)		//ʹ�ܵ������ģ��
	{
		if((myInverter.IA_InvOut < I_INVOUT_MIN)|(myInverter.IA_InvOut > I_INVOUT_MAX))
			INSFLAGBIT.IA_InvOut = 1;			//�����A�����˲ʱֵ������
		if((myInverter.IB_InvOut < I_INVOUT_MIN)|(myInverter.IB_InvOut > I_INVOUT_MAX))
			INSFLAGBIT.IB_InvOut = 1;			//B��
		if((myInverter.IC_InvOut < I_INVOUT_MIN)|(myInverter.IC_InvOut > I_INVOUT_MAX))
			INSFLAGBIT.IC_InvOut = 1;			//C��
	}
	else
	{
		INSFLAGBIT.IA_InvOut = 0;
		INSFLAGBIT.IB_InvOut = 0;
		INSFLAGBIT.IC_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)	//ʹ�ܵ��������ģ����
	{
		if((myInverter.IA_TotalOut < I_TOTOUT_MIN)|(myInverter.IA_InvOut > I_TOTOUT_MAX))
			INSFLAGBIT.IA_InvOut = 1;			//A���ܵ������˲ʱֵ������
		if((myInverter.IB_TotalOut < I_TOTOUT_MIN)|(myInverter.IB_InvOut > I_TOTOUT_MAX))
			INSFLAGBIT.IB_InvOut = 1;			//B��
		if((myInverter.IC_TotalOut < I_TOTOUT_MIN)|(myInverter.IC_InvOut > I_TOTOUT_MAX))
			INSFLAGBIT.IC_InvOut = 1;			//C��
	}
	else
	{
		INSFLAGBIT.IA_TotOut = 0;
		INSFLAGBIT.IB_TotOut = 0;
		INSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)		//ʹ��DCĸ�߼��ģ��
	{
		if((myInverter.U_DC < myFaultDetect.RangeIns.U_DC.minValue)|(myInverter.U_DC > myFaultDetect.RangeIns.U_DC.maxValue))
			INSFLAGBIT.U_DC = 1;			//DCĸ�߳�������
	}
	else
		INSFLAGBIT.U_DC = 0;

	if(SUB_ENABLE_BIT.U_InvOut)		//ʹ����������ѹ���
	{
		if((myInverter.UA_InvOut < U_INVOUT_MIN)|(myInverter.UA_InvOut > U_INVOUT_MAX))
			INSFLAGBIT.UA_InvOut = 1;			//�����A�������ѹ˲ʱֵ������
		if((myInverter.UB_InvOut < U_INVOUT_MIN)|(myInverter.UB_InvOut > U_INVOUT_MAX))
			INSFLAGBIT.UB_InvOut = 1;			//B��
		if((myInverter.UC_InvOut < U_INVOUT_MIN)|(myInverter.UC_InvOut > U_INVOUT_MAX))
			INSFLAGBIT.UC_InvOut = 1;			//C��
	}
	else
	{
		INSFLAGBIT.UA_InvOut = 0;
		INSFLAGBIT.UB_InvOut = 0;
		INSFLAGBIT.UC_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)		//ʹ����·��ѹ������
	{
		if((myInverter.UA_BP < U_BP_MIN)|(myInverter.UA_BP > U_BP_MAX))
			INSFLAGBIT.UA_BP = 1;			//A����·�����ѹ˲ʱֵ������
		if((myInverter.UB_BP < U_BP_MIN)|(myInverter.UB_BP > U_BP_MAX))
			INSFLAGBIT.UB_BP = 1;			//B��
		if((myInverter.UC_BP < U_BP_MIN)|(myInverter.UC_BP > U_BP_MAX))
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
    if((myInverter.FrA_BP<myFaultDetect.Range_FreBP.minValue)|(myInverter.FrA_BP>myFaultDetect.Range_FreBP.maxValue))
    {
        FlagOtherBIT.FrA_BP = 1;
    }
    else
    {
        FlagOtherBIT.FrA_BP = 0;
    }
        if((myInverter.UA_BPRMS<URMS_BP_MIN)|(myInverter.UA_BPRMS>URMS_BP_MAX))
            FlagOtherBIT.UA_BP = 1;       //��·A�������ѹ��Чֵ������
        else
            FlagOtherBIT.UA_BP = 0;
 //   if(myPhaseLock.PSD == NotShift)
  //  {FlagOtherBIT.PSD = 1;}
  //  else
    //{FlagOtherBIT.PSD = 0;}

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

	if(SUB_ENABLE_BIT.I_InvOut)		//ʹ�����������������
	{
		if((myInverter.IA_InvOutRMS<IRMS_INVOUT_MIN)|(myInverter.IA_InvOutRMS>IRMS_INVOUT_MAX))
			RMSFLAGBIT.IA_InvOut = 1;		//������A�������Чֵ������
		//if((myInverter.IB_InvOutRMS<IRMS_INVOUT_MIN)|(myInverter.IB_InvOutRMS>IRMS_INVOUT_MAX))
		//	RMSFLAGBIT.IB_InvOut = 1;		//������B�������Чֵ������
		//if((myInverter.IC_InvOutRMS<IRMS_INVOUT_MIN)|(myInverter.IC_InvOutRMS>IRMS_INVOUT_MAX))
		//	RMSFLAGBIT.IC_InvOut = 1;		//������C�������Чֵ������
	}
	else
	{
		RMSFLAGBIT.IA_InvOut = 0;
		//RMSFLAGBIT.IB_InvOut = 0;
		//RMSFLAGBIT.IB_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.I_TotalOut)			//ʹ���ܵ���������
	{
		if((myInverter.IA_TotalOutRMS<IRMS_TOTOUT_MIN)|(myInverter.IA_TotalOutRMS>IRMS_TOTOUT_MAX))
			RMSFLAGBIT.IA_TotOut = 1;		//��������A�������Чֵ������
		//if((myInverter.IB_TotalOutRMS<IRMS_TOTOUT_MIN)|(myInverter.IB_TotalOutRMS>IRMS_TOTOUT_MAX))
		//	RMSFLAGBIT.IB_TotOut = 1;		//��������B�������Чֵ������
		//if((myInverter.IC_TotalOutRMS<IRMS_TOTOUT_MIN)|(myInverter.IC_TotalOutRMS>IRMS_TOTOUT_MAX))
		//	RMSFLAGBIT.IC_TotOut = 1;		//��������C�������Чֵ������
	}
	else
	{
		RMSFLAGBIT.IA_TotOut = 0;
		//RMSFLAGBIT.IB_TotOut = 0;
		//RMSFLAGBIT.IC_TotOut = 0;
	}

	if(SUB_ENABLE_BIT.U_DC)			//ʹ��ֱ��ĸ�߼��
	{
		if((myInverter.U_DCRMS< IRMS_DC_MIN)|(myInverter.U_DCRMS>IRMS_DC_MAX))
		{
			RMSFLAGBIT.U_DC = 1;		//��������A�������Чֵ������
		}
		else
		{
		    RMSFLAGBIT.U_DC = 0;        //��������A�������Чֵ������

		}
	}
	else
	{
		RMSFLAGBIT.U_DC = 0;
	}

	if(SUB_ENABLE_BIT.U_InvOut)			//ʹ����������ѹ���
	{
		if((myInverter.UA_InvOutRMS<URMS_INVOUT_MIN)|(myInverter.UA_InvOutRMS>URMS_INVOUT_MAX))
			RMSFLAGBIT.UA_InvOut = 1;		//���A�������ѹ��Чֵ������
		//if((myInverter.UB_InvOutRMS<URMS_INVOUT_MIN)|(myInverter.UB_InvOutRMS>URMS_INVOUT_MAX))
		//	RMSFLAGBIT.UB_InvOut = 1;		//���B�������ѹ��Чֵ������
		//if((myInverter.UC_InvOutRMS<URMS_INVOUT_MIN)|(myInverter.UC_InvOutRMS>URMS_INVOUT_MAX))
		//	RMSFLAGBIT.UC_InvOut = 1;		//���C�������ѹ��Чֵ������
	}
	else
	{
		RMSFLAGBIT.UA_InvOut = 0;
		//RMSFLAGBIT.UB_InvOut = 0;
		//RMSFLAGBIT.UC_InvOut = 0;
	}

	if(SUB_ENABLE_BIT.U_BP)			//ʹ����·�����ѹ���
	{
		if((myInverter.UA_BPRMS<URMS_BP_MIN)|(myInverter.UA_BPRMS>URMS_BP_MAX))
			RMSFLAGBIT.UA_BP = 1;		//��·A�������ѹ��Чֵ������
		else
		    RMSFLAGBIT.UA_BP = 0;
		//if((myInverter.UB_BPRMS<URMS_BP_MIN)|(myInverter.UB_BPRMS>URMS_BP_MAX))
		//	RMSFLAGBIT.UB_BP = 1;		//��·B�������ѹ��Чֵ������
		//if((myInverter.UC_BPRMS<URMS_BP_MIN)|(myInverter.UC_BPRMS>URMS_BP_MAX))
		//	RMSFLAGBIT.UC_BP = 1;		//��·C�������ѹ��Чֵ������
	}
	else
	{
		RMSFLAGBIT.UA_BP = 0;
		//RMSFLAGBIT.UB_BP = 0;
		//RMSFLAGBIT.UC_BP = 0;
	}
  //  Uint16 returnValue = 0;
  //  if(myFaultDetect.FlagRms.all)       //�Ƿ��д���
   //     returnValue = 1;

	return 0;
}

Uint16 FaultHandlingScan(void)			//���ϴ���ɨ��
{
    if(myInverter.U_DCRMS<100)//170
    {

        myInverter.SOFTSTART = 0;
    }
    if(myFaultDetect.FlagOther.all )
    {myPhaseLock.StatePL = Disable;}
    else
    {myPhaseLock.StatePL = Enable;}
    if(SOFT_START == 0)
    {
        //if( ComCnt == 0)
       // {
            NowFaultFlag = myFaultDetect.FlagRms.all ;
             if(LastFaultFlag ^ NowFaultFlag)
             {
                 if(myPhaseLock.PSD_BYP == NoShift)
                 {
                     if(NowFaultFlag)
                     {CAN.SendDataWithPtc2(1,AutoSendMailBox);}//����·�����
                     else
                     {CAN.SendDataWithPtc2(0,AutoSendMailBox);}//��������·
                 }
             }
             else
             {
                 /*
                 if(NowFaultFlag == 0)
                 {
                     if(myPhaseLock.PSD_BYP == NoShift)
                     {
                       if(FPGA_FLAG.bit.InvRun == 0)
                       {CAN.SendDataWithPtc2(0,AutoSendMailBox);} //��������·
                     }
                 }
                 else if(NowFaultFlag == 1)
                 {
                     if(FPGA_FLAG.bit.InvRun )
                     {CAN.SendDataWithPtc2(1,AutoSendMailBox);}//����·�����

                 }
                 */
             }
          // }
             LastFaultFlag = NowFaultFlag;
         }
    /*
    if( ComCnt)
    {
        if(myFaultDetect.FlagOther.all||myFaultDetect.FlagRms.all)
        {
            myInverter.SOFTSTART = 0;
        }
        else
        {
            myInverter.SOFTSTART = 1;
        }
        ComCnt = 0;
    }
    */
    /*
       if( ComCnt)
       {

            if(myFaultDetect.FlagOther.all)
            {
                if(myFaultDetect.FlagRms.all == 0)
                {
                    CAN.SendDataWithPtc2(0,AutoSendMailBox);   //��������·
                }
                else
                {
                    CAN.SendDataWithPtc2(1,AutoSendMailBox);   //����·�����
                }
            }

            if(myFaultDetect.FlagOther.all)
            {
                if(myFaultDetect.FlagRms.all == 0)
                {myFpgaDriver.InvSwitchOn(Enable);}
                else
                {myFpgaDriver.InvSwitchOn(Disable);}
            }

            ComCnt = 0;
    }
*/

	return 0;
}

