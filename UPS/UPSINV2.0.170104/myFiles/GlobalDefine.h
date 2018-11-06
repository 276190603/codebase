//-------------------------------------------------------------
// �ļ����� "SampleAndControl.h"
//-------------------------------------------------------------
//���ܣ�	1.��ȫ�����õ��ĺ��ö�ٽ��ж���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2016��12��20��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------

#ifndef GLOBALDEFINE_H_
#define GLOBALDEFINE_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define HSPCLK28xx 75000		//��������ʱ��(k)
#define LSPCLK28xx 37500		//����ʱ������(k)
#define PI 3.141593
#define _2PI 6.283185

//-------------------------------------------------------------
// �ṹ�塢�����塢ö������
//-------------------------------------------------------------
typedef enum State
{
	Disable = 0,
	Enable
} State;

typedef enum Bool
{
	False = 0,
	True
} Bool;

typedef enum ModelState
{
	Stop = 0
	,Inited
	,Run
	,Waring
	,Fault
}ModelState;

typedef enum PhaseShiftDir
{
	NotShift = 0,
	LeftShift,
	RightShift,
	NoneShift,
	NoShift
}PhaseShiftDir;

typedef struct HardwarePool1Bits		//�����Ӧ��Ӳ����Դ��ʹ������λ1������Ϊ0
{							//	bit	description
	Uint16 CpuTimer0:1;		//	0	Cpu��ʱ��0
	Uint16 CpuTimer1:1;		//	1	Cpu��ʱ��1
	Uint16 CpuTimer2:1;		//	2	Cpu��ʱ��2
	Uint16 EPWM1:1;			//	3	EPWM1
	Uint16 EPWM2:1;			//	4	EPWM2
	Uint16 EPWM3:1;			//	5	EPWM3
	Uint16 EPWM4:1;			//	6	EPWM4
	Uint16 EPWM5:1;			//	7	EPWM5
	Uint16 EPWM6:1;			//	8	EPWM6
	Uint16 Adc:1;			//	9	ADC
	Uint16 ECap1:1;			//	10	ECap1
	Uint16 ECap2:1;			//	11	ECap2
	Uint16 ECap3:1;			//	12	ECap3
	Uint16 ECap4:1;			//	13	ECap4
	Uint16 ECap5:1;			//	14	ECap4
	Uint16 ECap6:1;			//	15	ECap4
}HardwarePool1Bits;

typedef union HardwarePool1
{
	Uint16 all;
	HardwarePool1Bits bit;
}HardwarePool1;

typedef struct HardwarePool2Bits			//Ӳ����Դ��2
{								//	bit	description
	Uint16 Scia:1;				//	0	SCIAģ��
	Uint16 Scib:1;				//	1	SCIBģ��
	Uint16 Scic:1;				//	2	SCIC
	Uint16 Cana:1;				//	3	CANA
	Uint16 Canb:1;				//	4	CANB
	Uint16 XInt1:1;				//	5	�ⲿ�ж�1
	Uint16 Xint2:1;				//	6	�ⲿ�ж�2
	Uint16 Xint3:1;				//	7	�ⲿ�ж�3
	Uint16 Xint4:1;				//	8	�ⲿ�ж�4
	Uint16 Xint5:1;				//	9	�ⲿ�ж�5
	Uint16 Xint6:1;				//	10	�ⲿ�ж�6
	Uint16 Xint7:1;				//	11	�ⲿ�ж�7
	Uint16 Xnmi:1;				//	12	�ⲿ���������ж�
}HardwarePool2Bits;

typedef union HardwarePool2
{
	Uint16 all;
	HardwarePool2Bits bit;
}HardwarePool2;

//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern HardwarePool1 myHardwarePool;
extern HardwarePool2 myHardwarePool2;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void ResetHardwarePool(void);

#endif /* GLOBALDEFINE_H_ */
