//-------------------------------------------------------------
// �ļ����� "Inverter.h"
//-------------------------------------------------------------
//���ܣ�	1.����Inverter.c����ͷ�ļ�
//		2.������غ궨��
//      3.����ȫ�ֱ���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2016��12��27��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------

#ifndef INVERTER_H
#define INVERTER_H

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "GlobalDefine.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "SampleAndControl.h"
#include "PhaseLock.h"
#include "FaultDetect.h"
#include "communication.h"
#include "SystemTick.h"
#include "FpgaDriver.h"

//-------------------------------------------------------------

//-------------------------------------------------------------
// �궨��
//-------------------------------------------------------------


//-------------------------------------------------------------
// ö����������
//-------------------------------------------------------------
typedef enum OutputSource
{
	BP = 0
	,Inv
}OutputSource;

//-------------------------------------------------------------
// �ṹ������
//-------------------------------------------------------------
typedef struct Inverter
{
	State State;					//��俪��
	float IA_InvOut;				//�����A���������
	float IB_InvOut;				//�����B���������
	float IC_InvOut;				//�����C���������
	float IA_TotalOut;				//A�����������
	float IB_TotalOut;				//B�����������
	float IC_TotalOut;				//C�����������
	float U_DC;						//DCĸ�ߵ�ѹ
	float UA_InvOut;				//�����A�������ѹ
	float UB_InvOut;				//�����B�������ѹ
	float UC_InvOut;				//�����C�������ѹ
	float UA_BP;					//��·A���ѹ
	float UB_BP;					//��·B���ѹ
	float UC_BP;					//��·C���ѹ

	float IA_InvOutRMS;				//�����A�����������Чֵ
	float IB_InvOutRMS;				//�����B�����������Чֵ
	float IC_InvOutRMS;				//�����C�����������Чֵ
	float IA_TotalOutRMS;			//A�������������Чֵ
	float IB_TotalOutRMS;			//B�������������Чֵ
	float IC_TotalOutRMS;			//C�������������Чֵ
	float U_DCRMS;					//DCĸ�ߵ�ѹ��Чֵ
	float UA_InvOutRMS;				//�����A�������ѹ��Чֵ
	float UB_InvOutRMS;				//�����B�������ѹ��Чֵ
	float UC_InvOutRMS;				//�����C�������ѹ��Чֵ
	float UA_BPRMS;					//��·A���ѹ��Чֵ
	float UB_BPRMS;					//��·B���ѹ��Чֵ
	float UC_BPRMS;					//��·C���ѹ��Чֵ

	float FrA_BP;					//��·A��Ƶ��
	float FrB_BP;					//��·B��Ƶ��
	float FrC_BP;					//��·C��Ƶ��
	float PhA_BP;					//��·A����λ
	float PhB_BP;					//��·B����λ
	float PhC_BP;					//��·C����λ
	float FrOut;					//���Ƶ������(Hz)

    float FrA_INV;                   //���A��Ƶ��
    float FrB_INV;                   //���B��Ƶ��
    float FrC_INV;                   //���C��Ƶ��
    Uint16 SOFTSTART;
	ModelState RunState;				//����״̬
	ModelState LastRunState;			//��һ������״̬
	OutputSource OutputSource;			//���Դ

	Uint16 (*Init)(void);						//��ʼ��
	Uint16 (*ChangeState)(State state);			//�ı������״̬
	Uint16 (*RunStateChangeCallback)(void);		//����״̬�ı�ص�����
}Inverter;

//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern Inverter myInverter;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void UseInverter(void);

#endif
