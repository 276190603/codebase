//-------------------------------------------------------------
// �ļ����� "Scr.h"
//-------------------------------------------------------------
//���ܣ�	1.����Scr.c����ͷ�ļ�
//		2.������غ궨��
//      3.����ȫ�ֱ���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2016��12��27��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------

#ifndef Scr_H
#define Scr_H

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
	,SCR
}OutputSource;

//-------------------------------------------------------------
// �ṹ������
//-------------------------------------------------------------
typedef struct Scr
{
	State State;					//��������
	float IA_SCROut;				//������A���������
	float IB_SCROut;				//������B���������
	float IC_SCROut;				//������C���������
	float IA_TotalOut;				//A�����������
	float IB_TotalOut;				//B�����������
	float IC_TotalOut;				//C�����������
	float U_DC;						//DCĸ�ߵ�ѹ
	float UA_SCROut;				//������A�������ѹ
	float UB_SCROut;				//������B�������ѹ
	float UC_SCROut;				//������C�������ѹ
	float UA_BP;					//��·A���ѹ
	float UB_BP;					//��·B���ѹ
	float UC_BP;					//��·C���ѹ

	float IA_SCROutRMS;				//������A�����������Чֵ
	float IB_SCROutRMS;				//������B�����������Чֵ
	float IC_SCROutRMS;				//������C�����������Чֵ
	float IA_TotalOutRMS;			//A�������������Чֵ
	float IB_TotalOutRMS;			//B�������������Чֵ
	float IC_TotalOutRMS;			//C�������������Чֵ
	float U_DCRMS;					//DCĸ�ߵ�ѹ��Чֵ
	float UA_SCROutRMS;				//������A�������ѹ��Чֵ
	float UB_SCROutRMS;				//������B�������ѹ��Чֵ
	float UC_SCROutRMS;				//������C�������ѹ��Чֵ
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
    float Phase_AB;                   //��·A��Ƶ��

    float FrA_SCR;                   //����A��Ƶ��
    float FrB_SCR;                   //����B��Ƶ��
    float FrC_SCR;                   //����C��Ƶ��

    Uint16 SOFTSTART;                       //��ʼ��
	ModelState RunState;				//����״̬
	ModelState LastRunState;			//��һ������״̬
	OutputSource OutputSource;			//���Դ

	Uint16 (*Init)(void);						//��ʼ��
	Uint16 (*ChangeState)(State state);			//�ı�������״̬
	Uint16 (*RunStateChangeCallback)(void);		//����״̬�ı�ص�����
}Scr;

//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern Scr myScr;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void UseScr(void);

#endif
