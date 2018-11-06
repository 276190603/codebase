//-------------------------------------------------------------
// �ļ����� "SampleAndControl.h"
//-------------------------------------------------------------
//���ܣ�	1.����SampleAndControl.c����ͷ�ļ�
//		2.������غ궨��
//      3.����ȫ�ֱ���
//-------------------------------------------------------------
// ���ߣ� ���Ǳ�
// �������ڣ�2016��12��20��
// �޸����ڣ�
// �汾��v0.1
//-------------------------------------------------------------

#ifndef SAMPLEANDCONTROL_H_
#define SAMPLEANDCONTROL_H_

//-------------------------------------------------------------
// INCLUDE
//-------------------------------------------------------------
#include "GlobalDefine.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "math.h"
#include "Scr.h"
#include "PhaseLock.h"
#include "communication.h"

//-------------------------------------------------------------

//-------------------------------------------------------------
// �궨��
//-------------------------------------------------------------
//�����궨��
#define MAXPHA 0.1736482			//10���Ӧ��sinֵ
#define MINPHA -0.1736482			//-10���Ӧ��sinֵ
#define SQRT2 1.414214				//����2
#define SQRT2_SCR 0.70710678		//����2����
#define SAMPLE_NUM 30				//��������һ�����ڵĲ�������������Ҫ�Ͳ�������ͬ

#define C_KP myControlSystem.Controller.ControlKp			//����ϵ��
#define I_KP myControlSystem.Controller.ControlKp_I           //����ϵ��
#define C_TARGET myControlSystem.Controller.Target			//������Ŀ��ֵ
#define C_ERROR myControlSystem.Controller.Error			//���������ֵ
#define C_WEIGHT myControlSystem.Controller.WeightErr		//���������Ȩֵ
#define C_OUTPUT myControlSystem.Controller.Output			//���������

#define SAMPLE_FRE myControlSystem.Sample.SampleRate		//����Ƶ��
#define SAMPLE_OV myControlSystem.Sample.OriginalValue		//����ԭʼֵ
#define SAMPLE_K myControlSystem.Sample.K_Sample			//��������ϵ��
#define SAMPLE_B myControlSystem.Sample.B_Sample			//�����ؾ�ϵ��

#define DIV_FAC EPwm4Regs.TBCTL.bit.CLKDIV						//����ʱ�ӷ�Ƶϵ�� sample_clk = HSPCLK/(2^DIV_FAC)


#define scr_1 0x0001
#define scr_2 0x0002
#define scr_3 0x0003
#define scr_4 0x0004
#define scr_5 0x0005
#define scr_6 0x0006

#define HIGH  0x0001
#define LOW   0x0000

//-------------------------------------------------------------
// �ṹ������
//-------------------------------------------------------------

typedef struct Controller        //����ʵ��
{
	//����
	State State;						//����ʹ��
	float Target;						//����ֵ
	float ControlKp;					//����������ϵ��
	float ControlKi;					//����������ϵ��
    float ControlKp_I;                    //����������ϵ��
	float Error[7];						//���������e(k-i) = Error[i];(i=0,1,2)
	float WeightErr[3];					//���Ȩֵ���������м�Ȩ��ֵ�˲������뱣֤Ȩֵ���������Һ�Ϊ1��
	float InsUA_Out;					//���浱ǰUA��˲ʱ��Чֵ
	float Output;						//���������
	//Y(k)=ControlKp*(U(k)-U(k-1)) + ControlKi*Ts*U(k)

	//����
	Uint16 (*IsAddControl)(void);				//�ж��Ƿ����Ͷ������������ı���״̬
	Uint16 (*Control)(void);					//���ƹ���
}Controller;

typedef struct SoftStart		//����ʵ��
{
	//����
	float SS_Target;					//��������soft-start��Ŀ��ֵ�������������
	Uint16 SS_Stairs;					//������������
	Uint16 SS_Time;						//�������ܹ���ʱ(ms)

	//����
	Uint16 (*Run)(void);					//������
}SoftStart;

typedef struct Sample			//����ʵ��
{
	//����
	State State;						//������sample and control��״̬
	float SampleRate;					//����Ƶ�ʣ�Hz��
	float SampleRatio;					//������
	Bool FlagRMS;						//��Чֵ�����־λ(ͨ������CalRMSCmd�����ı䣬����ֱ���޸�)
	Uint16 SampleFlag;					//������־λ��ÿ�β����ж����0λ��ת��1λ��λ
										//					˲ʱֵ����2λ��ת��3λ��λ
										//					��Чֵ����4λ��ת��5λ��λ

	Uint16 OriginalValue[16];			//����ԭʼ�����Ĵ���ֵ���Ҷ��룩
	float K_Sample[16];					//��������ϵ��
	float B_Sample[16];					//�����ؾ�
	//ʵ��ֵ[i]=K_Sample[i]*OriginalValue[i]+B_Sample[i]

	//����
	Uint16 (*Init)(void);							//�������Ƴ�ʼ��
	Uint16 (*ChangeState)(State state);				//�ı����״̬��������ֹͣ��
	Uint16 (*Scan)(void);							//������������ѭ��ɨ�裬��һ�����������ڸú�������
													//��ִ������һ�Σ�����ᶪʧ�������
	Uint16 (*CalRMSCmd)(Bool cmd);					//�ı���Чֵ����״̬
} Sample;

typedef struct Actuator			//ִ�л���
{
	//����
	State State;				//ִ�л���״̬
	float CarFre;				//�ز�Ƶ��(kHz)
	float CarRatio;				//�ز���
	float DBTime;				//����ʱ��(0-13.5)us

	//����
	Uint16 (*Init)(void);		//ִ�л�����ʼ��
	Uint16 (*ChangeState)(State state);		//�ı�״̬
	float (*GetOutputPhase)(void);			//��ȡ�����λ
} Actuator;

typedef struct ControlSystem		//����ϵͳ
{
	Controller Controller;
	Sample Sample;
	SoftStart SoftStart;
	Actuator Actuator;

	//����
	Uint16 (*Init)(void);		//ִ�л�����ʼ��
}ControlSystem;

//-------------------------------------------------------------
// ȫ�ֱ�������
//-------------------------------------------------------------
extern ControlSystem myControlSystem;

//extern unsigned int SOFT_START;
extern unsigned int SOFT_START;
extern unsigned int FIRST_START;
extern unsigned int DC_Target;
extern float PID;
extern unsigned int TIMER1_num;
extern unsigned int muxian_count;
void PID_change(float target);
void soft_start(void);
void scr_number_switch(unsigned int scr_number,unsigned int High_Low);
//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
void UseControlSystem(void);

interrupt void ADC_ISR(void);
interrupt void Epwm1_ISR(void);
interrupt void CpuTimer1_ISR(void);

#endif /* SAMPLEANDCONTROL_H_ */
