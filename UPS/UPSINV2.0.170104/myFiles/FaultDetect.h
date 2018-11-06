//-------------------------------------------------------------
// �ļ����� "FaultDetect.h"
//-------------------------------------------------------------
//���ܣ�	1.�Թ��ϼ�����õ��ĺ��ö�ٽ��ж���
//		2.������ؽṹ��
//		3.������غ���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2017��1��3��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------

#ifndef FAULTDETECT_H_
#define FAULTDETECT_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "GlobalDefine.h"
#include "Inverter.h"

//-------------------------------------------------------------
// �궨��
//-------------------------------------------------------------
#define I_INVOUT_MIN myFaultDetect.RangeIns.I_InvOut.minValue			//������������Сֵ
#define I_INVOUT_MAX myFaultDetect.RangeIns.I_InvOut.maxValue			//�������������ֵ
#define I_TOTOUT_MIN myFaultDetect.RangeIns.I_TotalOut.minValue			//�����������Сֵ
#define I_TOTOUT_MAX myFaultDetect.RangeIns.I_TotalOut.maxValue			//������������ֵ
#define U_INVOUT_MIN myFaultDetect.RangeIns.U_InvOut.minValue			//��������Ԫ��Сֵ
#define U_INVOUT_MAX myFaultDetect.RangeIns.U_InvOut.maxValue			//��������Ԫ��Сֵ
#define U_BP_MIN myFaultDetect.RangeIns.U_BP.minValue					//��·�����Сֵ
#define U_BP_MAX myFaultDetect.RangeIns.U_BP.maxValue					//��·������ֵ

#define IRMS_INVOUT_MIN myFaultDetect.RangeRms.I_InvOut.minValue		//������������Чֵ��Сֵ
#define IRMS_INVOUT_MAX myFaultDetect.RangeRms.I_InvOut.maxValue		//������������Чֵ��Сֵ
#define IRMS_TOTOUT_MIN myFaultDetect.RangeRms.I_TotalOut.minValue		//�����������Чֵ��Сֵ
#define IRMS_TOTOUT_MAX myFaultDetect.RangeRms.I_TotalOut.maxValue		//�����������Чֵ���ֵ
#define IRMS_DC_MIN myFaultDetect.RangeRms.U_DC.minValue				//ֱ��ĸ����Чֵ��Сֵ
#define IRMS_DC_MAX myFaultDetect.RangeRms.U_DC.maxValue				//ֱ��ĸ����Чֵ���ֵ
#define URMS_INVOUT_MIN myFaultDetect.RangeRms.U_InvOut.minValue		//��������ѹ��Чֵ��Сֵ
#define URMS_INVOUT_MAX myFaultDetect.RangeRms.U_InvOut.maxValue		//��������ѹ��Чֵ���ֵ
#define URMS_BP_MIN myFaultDetect.RangeRms.U_BP.minValue				//��·��ѹ��Чֵ��Сֵ
#define URMS_BP_MAX myFaultDetect.RangeRms.U_BP.maxValue				//��·��ѹ��Чֵ���ֵ

#define INSFLAGBIT myFaultDetect.FlagIns.bit							//˲ʱֵ��־λ��λ��
#define RMSFLAGBIT myFaultDetect.FlagRms.bit							//��Чֵ��־λ��λ��
#define FlagOtherBIT myFaultDetect.FlagOther.bit                            //���������־λ��λ��
#define SUB_ENABLE_BIT myFaultDetect.EnableSubFD.bit					//��ģ��ʹ�ܣ�λ��

//-------------------------------------------------------------
// �ṹ������
//-------------------------------------------------------------

typedef struct Range		//��ʾһ����Χ
{
	float minValue;
	float maxValue;
}Range;

typedef struct SubFDBit			//��ģ�����ʹ��λ
{								//	bit		description
	Uint16 I_InvOut:1;			//	0		�������������ϼ��ʹ��λ
	Uint16 I_TotalOut:1;		//	1		���������������ϼ��ʹ��λ
	Uint16 U_DC:1;				//	2		ֱ��ĸ�߹��ϼ��ʹ��λ
	Uint16 U_InvOut:1;			//	3		��������ѹ���ϼ��ʹ��λ
	Uint16 U_BP:1;				//	4		��·��ѹ����ʹ�ܼ��λ
	Uint16 Fr_BP:1;				//	5		��·Ƶ�ʹ��ϼ��ʹ��λ
	Uint16 rsvd:10;				//	6:15	����
} SubFDBit;

typedef union SubFD				//��ģ�����ʹ��λ������
{
	Uint16 all;
	SubFDBit bit;
}SubFD;

typedef struct RangeSet			//������ⷶΧ
{
	Range I_InvOut;				//������������Χ
	Range I_TotalOut;			//���������˲ʱֵ��Χ
	Range U_DC;					//ֱ��ĸ�������Χ
	Range U_InvOut;				//����������ѹ��Χ
	Range U_BP;					//��·��ѹ��Χ
}RangeSet;

typedef struct FlagFaultBitSMP		//��������־λ
{								//	bit		description
	Uint16 IA_InvOut:1;			//	0		�����A�����������־λ
	Uint16 IB_InvOut:1;			//	1		�����B���������
	Uint16 IC_InvOut:1;			//	2		�����C���������
	Uint16 IA_TotOut:1;			//	3		�����A�������������־λ
	Uint16 IB_TotOut:1;			//	4		B��
	Uint16 IC_TotOut:1;			//	5		C��
	Uint16 U_DC:1;				//	6		ֱ��ĸ�߱�־λ
	Uint16 UA_InvOut:1;			//	7		�����A�������ѹ
	Uint16 UB_InvOut:1;			//	8		B��
	Uint16 UC_InvOut:1;			//	9		C��
	Uint16 UA_BP:1;				//	10		��·A���ѹ
	Uint16 UB_BP:1;				//	11		��·B���ѹ
	Uint16 UC_BP:1;				//	12		��·C���ѹ
	Uint16 rsvd:3;				//	13:15	����
}FlagFaultBitSMP;

typedef union FlagFaultSMP		//��������־λ������
{
	Uint16 all;
	FlagFaultBitSMP bit;
}FlagFaultSMP;

typedef struct FaultFlagBit		//���������־λ
{								//	bit		description
	Uint16 FrA_BP:1;			//	0		��·A��Ƶ�ʱ�־λ
	Uint16 FrB_BP:1;			//	1		B��
	Uint16 FrC_BP:1;			//	2		C��
	Uint16 PhaseSeq:1;			//	3		��������־λ
	Uint16 PSD:1;               //  4                     ����
	Uint16 UA_BP:1;            //  0       ��·A��Ƶ�ʱ�־λ
	Uint16 rsvd:10;				//	5:15	����
}FaultFlagBit;

typedef union FaultFlag			//���������־λ������
{
	Uint16 all;
	FaultFlagBit bit;
}FaultFlag;

typedef struct FaultDetect				//���ϼ�ⵥԪ
{
	//����
	State state;						//���ϼ����ʹ��
	SubFD EnableSubFD;					//�ӹ��ϼ��ʹ��
	RangeSet RangeIns;					//˲ʱֵ��Χ
	RangeSet RangeRms;					//��Чֵ��Χ
	RangeSet BackWinRms;				//��Чֵ�ش���Χ
	Range Range_FreBP;					//��·Ƶ�ʷ�Χ
	Range BackWin_FreBP;				//��·Ƶ�ʻش�
	FlagFaultSMP FlagIns;				//˲ʱֵ���ϱ�־λ
	FlagFaultSMP FlagRms;				//��Чֵ���ϱ�־λ
	FaultFlag FlagOther;				//�������ϱ�־λ

	//����
	Uint16 (*InsFaultDetAndHandScan)(void);		//˲ʱֵ���ϼ�⼰����ɨ��
	Uint16 (*RmsFaultDetectScan)(void);			//��Чֵ���ϼ��ɨ��
	Uint16 (*FaultHandlingScan)(void);			//���ϴ���ɨ��
	Uint16 (*OtherFaultDetScan)(void);

} FaultDetect;


//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern FaultDetect myFaultDetect;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void UsemyFaultDetect(void);

#endif /* GLOBALDEFINE_H_ */
