//-------------------------------------------------------------
// �ļ����� "FpgaDriver.h"
//-------------------------------------------------------------
//���ܣ�	1.����FpgaDriver.c����ͷ�ļ�
//		2.������غ궨��
//      3.����ȫ�ֱ���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2017��1��18��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------
#ifndef FPGADRIVER_H_
#define FPGADRIVER_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "GlobalDefine.h"

//-------------------------------------------------------------
// �궨��
//-------------------------------------------------------------
#define FPGA_REG0 myFpgaDriver.FpgaRegs.Reg0
#define FPGA_REG1 myFpgaDriver.FpgaRegs.Reg1
#define FPGA_FLAG myFpgaDriver.Flag

//-------------------------------------------------------------
// �ṹ��������ö������
//-------------------------------------------------------------

//FPGA�Ĵ���0
typedef struct FpgaReg0Bits		//FPGA�Ĵ���0����λ����
{								//	bit	description
	Uint16 SPWMEnable:1;		//	0	1->�������ʹ��
	Uint16 InvSwitchOn:1;		//	1	1->�������������
	Uint16 RecInputSW:1;		//	2	1->����������բ
}FpgaReg0Bits;
typedef union FpgaReg0			//FPGA���üĴ�����ֻд
{
	Uint16 all;
	FpgaReg0Bits bit;
}FpgaReg0;

//FPGA�Ĵ���1
typedef struct FpgaReg1Bits			//FPGA�Ĵ���1����λ����
{									//	bit	description
	Uint16 RecSWState:1;			//	0	1->�������뿪�رպ�
	Uint16 InvRunState:1;			//	1	1->���������
	Uint16 NeedTurnToBPState:1;		//	2	1->��Ҫת��·
	Uint16 TempHighState:1;			//	3	1->�¶ȹ���
	Uint16 ArmAState:1;				//	4	1->A���ű۹���
	Uint16 ArmBState:1;				//	5	1->B���ű۹���
	Uint16 ArmCState:1;				//	6	1->C���ű۹���
}FpgaReg1Bits;
typedef union FpgaReg1				//FPGA��ȡ״̬�Ĵ���,ֻ��
{
	Uint16 all;
	FpgaReg1Bits bit;
}FpgaReg1;

//FPGA�Ĵ�����
typedef struct FpgaRegs
{
	FpgaReg0 Reg0;
	FpgaReg1 Reg1;
}FpgaRegs;

typedef struct FpgaFlagBits
{							//	bit	description
	Uint16 Enable:1;		//	0	�Ƿ�ʹ��Fpga����
	//IN
	Uint16 RecSW:1;			//	1	���������Ƿ�պ� 1 -> �պ�
	Uint16 InvRun:1;		//	2	����Ƿ����� 1 -> ����
	Uint16 NeedTurnToBP:1;	//	3	��Ҫת��· 1 -> ��Ҫת��·
	Uint16 TempHigh:1;		//	4	�¶ȹ��� 1 -> �¶ȹ���
	Uint16 ArmAState:1;		//	5	�ű�A����״̬ 1 -> ����
	Uint16 ArmBState:1;		//	6	�ű�B����״̬ 1 -> ����
	Uint16 ArmCState:1;		//	7	�ű�C����״̬ 1 -> ����

	//OUT
	Uint16 SpwmOutEnable:1;	//	8	SPWM���ʹ��1 -> ʹ��SPWM���
	Uint16 InvSwitchOn:1;	//	9	�������Ӵ������� 1 -> ����(��FPGA����ź��෴)
	Uint16 RecInputSW:1;	//	10	�������뿪����բ���� 1 -> ������բ

}FpgaFlagBits;

typedef union FpgaFlag
{
	Uint16 all;
	FpgaFlagBits bit;
}FpgaFlag;


typedef struct FpgaDriver
{
	//����
	FpgaFlag Flag;
	FpgaRegs FpgaRegs;							//FPGA�ļĴ����������ֶ����ģ���Ҫ������Ӧ��������

	//����
	Uint16 (*Init)(void);						//��ʼ��
	Uint16 (*ChangeState)(State state);			//�ı�״̬
	Uint16 (*Scan)(void);						//ɨ���Ƿ����¼���Ҫ����
	Uint16 (*ReadFpgaRegs)(void);				//��ȡFPGA���пɶ��ļĴ���

	Uint16 (*ReadEndCB)(void);						//���ݶ�ȡ��ϻص�����
	Uint16 (*RecSW_StChangeCB)(void);				//��������״̬�ı�ص�����
	Uint16 (*InvRun_StChangeCB)(void);				//�������״̬�ı�ص�����
	Uint16 (*NeedTurnToBP_StChangeCB)(void);		//��Ҫת��·״̬�ı�ص�����
	Uint16 (*TempHigh_StChangeCB)(void);			//�¶ȹ���״̬�ı�ص�����
	Uint16 (*Arm_StChangeCB)(Uint16 isChange);		//�ű�״̬�ı�ص�����

	Uint16 (*SpwmOutEnable)(State state);		//�������ʹ��
	Uint16 (*InvSwitchOn)(State state);			//������������
	Uint16 (*RecInputSW)(State state);			//�������뿪��
}FpgaDriver;
//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern FpgaDriver myFpgaDriver;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void UseFpgaDriver(void);
interrupt void Xint3_ISR(void);


#endif /* FPGADRIVER_H_ */
