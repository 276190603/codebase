//----------------------------------------------------------------------------
// �ļ�����SampleAndControl.c
//----------------------------------------------------------------------------
// ����:  ���ļ������˿���ϵͳ�п�����������ģ�顢ִ�л�����������ģ��
//
//----------------------------------------------------------------------------
// ˵���� �õ���Ӳ��������ADC��Epwm1-4,��ʱ��1
//----------------------------------------------------------------------------
// ���Ŷ��壺
//			ADCINA0-7	->	Aͨ������
//			ADCINB0-7	->	Bͨ������
//			GPIO0-5		->	PWM��������˿�(����ֻ��4·)
// оƬ��	TMS230F28335
//----------------------------------------------------------------------------
// ���ߣ� ���Ǳ�
// ��������: 2017��1��4��
// �޸����ڣ�
// �汾��v0.1
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//include
//-----------------------------------------------------------------------------
#include "SampleAndControl.h"

//-----------------------------------------------------------------------------
//��������
//-----------------------------------------------------------------------------
Uint16 IsAddControl(void);
Uint16 Control(void);
float CalRMS(float insValue,float* fifo,int* ptr,float* uSa,float* curPh);

Uint16 SACInit(void);
Uint16 ChangeSampleState(State state);
Uint16 SAC_Scan(void);
Uint16 CalRMSCmd(Bool cmd);

Uint16 SoftStartRun(void);

Uint16 ActuatorInit(void);
Uint16 ActuatorChangeState(State state);
float ActuatorGetOutputPhase(void);

Uint16 ControlSystemInit(void);

//-----------------------------------------------------------------------------
//variables
//-----------------------------------------------------------------------------
ControlSystem myControlSystem;

float KiTs = 0;			//Ki*Ts
int16 SampleN;			//һ�����ڲ���������������ȡ����
int iCalRMS = 0;		//������Чֵ��ǰͳ�Ʋ�������
float SamplePhase = 0;	//����ʱ���������λ
int16 RemainStep = 0;	//������ʣ�ಽ��
float OutPerStep = 0;	//�������������
float PhasePerStep;		//ÿ��ִ�л�������ƶ��ĵ�Ƕ�(����0-2��)
float CurPhase;			//ִ�л�����ǰ����ĵ�Ƕ�

float COPRD = 0;		//�����������������ֵ
float C_OUTPUT1 = 0;
int16 RemainStep1 = 0;   //������ʣ�ಽ��
float OutPerStep1 = 0;   //�������������

float C_OUTPUT2 = 0;
int16 RemainStep2 = 0;   //������ʣ�ಽ��
float OutPerStep2 = 0;   //�������������
//���������ӵ���Чֵ����

float SNumRec = 1.0/SAMPLE_NUM;		//���������ĵ���
float DeltaPh = _2PI/SAMPLE_NUM;	//�������������������λ

float UaFifo[SAMPLE_NUM];	//����ʹ��FIFIO��������Чֵ������
int UaPtr = 0;				//ָ����һ����Ҫд���Fifoλ��
float UaSa = 0;				//���淽��ֵ
float UaPh = 0;				//���浱ǰʱ��Ua��˲ʱֵ��λ

float UA[150];			//���ڲ���ʱ�����������������
float UAi[150];			//����˲ʱ��Чֵ

Uint16 I_KiEnable = 0;
unsigned int SOFT_START=1;
Uint16 SOFT_START_STATE = 0;
//-----------------------------------------------------------------------------
// ��������void UseControlSystem(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ʼ��myControlSystem��Ա����
//-----------------------------------------------------------------------------
// ����˵�������������еı�������֧����ѡ��ĳ�ʼ����Ա��������˶���һ������ר����
//			�ڶ�myControlSystem�Ĳ��ֳ�Ա�������г�ʼ����ʹ��myControlSystem֮ǰ����Ҫ
//			���ô˺�����
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
 void UseControlSystem(void)
{
	//������ʼ��
	//���Գ�ʼ��

	//����ģ��
	myControlSystem.Controller.ControlKi = 0.39;	//����ϵ����0
	myControlSystem.Controller.ControlKp = 0.0025;	//����ϵ����1
    myControlSystem.Controller.ControlKp_I = 0.005;   //����ϵ����1
	myControlSystem.Controller.Error[0] = 0;	//e(k) = 0
	myControlSystem.Controller.Error[1] = 0;	//e(k-1) = 0
	myControlSystem.Controller.Error[2] = 0;	//e(k-2) = 0
    myControlSystem.Controller.Error[3] = 0;    //e(k-2) = 0
    myControlSystem.Controller.Error[4] = 0;    //e(k-2) = 0
	myControlSystem.Controller.WeightErr[0] = 1;//0.5714286;	//e(k)ʱ��Ȩֵ
	myControlSystem.Controller.WeightErr[1] = 0;//0.2857143;	//e(k-1)ʱ��Ȩֵ
	myControlSystem.Controller.WeightErr[2] = 0;//0.1428571;	//e(k-2)ʱ��Ȩֵ�����뱣֤��Ϊ1
	myControlSystem.Controller.Output = 0.05;	//������ƶȣ�0.7
	myControlSystem.Controller.State = Disable;	//��ֹ������
	myControlSystem.Controller.Target = 220;	//Ŀ��ֵ��220

	//������ģ��
	myControlSystem.SoftStart.SS_Stairs = 10;	//��4���׶�����
	myControlSystem.SoftStart.SS_Target = 0.7;	//����������������0.7
	myControlSystem.SoftStart.SS_Time = 400;	//��������ʱ��1000 ms

	//����ģ��
	myControlSystem.Sample.K_Sample[0] = 0;			//A0б��
	myControlSystem.Sample.B_Sample[0] = 0;			//A0�ؾ�

	myControlSystem.Sample.K_Sample[1] = 0.00194465;			//A1б��
	myControlSystem.Sample.B_Sample[1] = -4.35740715;			//A1�ؾ�

	myControlSystem.Sample.K_Sample[2] = 0.00194465;			//A2б��
	myControlSystem.Sample.B_Sample[2] = -4.35740715;			//A2�ؾ�

	myControlSystem.Sample.K_Sample[3] = 0.00194465*75;			//A3б��
	myControlSystem.Sample.B_Sample[3] = -4.35740715*75;			//A3�ؾ�

	myControlSystem.Sample.K_Sample[4] = 0.00194465*75;			//A4б��
	myControlSystem.Sample.B_Sample[4] = -4.35740715*75;			//A4�ؾ�

	myControlSystem.Sample.K_Sample[5] = 0.00194465;			//A5б��
	myControlSystem.Sample.B_Sample[5] = -4.35740715;			//A5�ؾ�

	myControlSystem.Sample.K_Sample[6] = 0.00194465*75;			//A6б��
	myControlSystem.Sample.B_Sample[6] = -4.35740715*75;			//A6�ؾ�

	myControlSystem.Sample.K_Sample[7] = 0;			//A7б��
	myControlSystem.Sample.B_Sample[7] = 0;			//A7�ؾ�

	myControlSystem.Sample.K_Sample[8] = 0.0658;		//B0б��(ֱ��ĸ�߲���)
	myControlSystem.Sample.B_Sample[8] = 1.5748;	//B0�ؾ�

	myControlSystem.Sample.K_Sample[9] = 0.2133;			//B1б��
	myControlSystem.Sample.B_Sample[9] = -444.0385;			//B1�ؾ�(UC_Inv)

	myControlSystem.Sample.K_Sample[10] = 0.2133;		//B2б��
	myControlSystem.Sample.B_Sample[10] = -444.0385;		//B2�ؾ�(UB_Inv)

	myControlSystem.Sample.K_Sample[11] = 0.2133;		//B4б��
	myControlSystem.Sample.B_Sample[11] = -444.0385;		//B4�ؾ�	(UA_Inv)

	myControlSystem.Sample.K_Sample[12] = 0.2462;				//B3б��(BP_C)
	myControlSystem.Sample.B_Sample[12] = -514.6332;				//B3�ؾ�

	myControlSystem.Sample.K_Sample[13] = 0.2462;		//B5б��(BP_A)
	myControlSystem.Sample.B_Sample[13] = -514.6332;		//B5�ؾ�

	myControlSystem.Sample.K_Sample[14] = 0.2462;		//B6б��(BP_B)
	myControlSystem.Sample.B_Sample[14] = -514.6332;		//B6�ؾ�

	myControlSystem.Sample.K_Sample[15] = 0;		//B7б��
	myControlSystem.Sample.B_Sample[15] = 0;		//B7�ؾ�

	myControlSystem.Sample.FlagRMS = False;			//��������Чֵ
	myControlSystem.Sample.SampleFlag = 0;			//��־λ����
	myControlSystem.Sample.SampleRate = 1500;		//����Ƶ�ʣ�1500 Hz
	myControlSystem.Sample.State = Disable;			//��ֹ����ģ��

	//ִ�л���
	myControlSystem.Actuator.CarFre = 20;		//�ز���20k
	myControlSystem.Actuator.DBTime = 3.5;		//������0us
	myControlSystem.Actuator.State = Disable;	//��ִֹ�л���

	//������λ
	myControlSystem.Init = ControlSystemInit;

	myControlSystem.Controller.Control = Control;
	myControlSystem.Controller.IsAddControl = IsAddControl;

	myControlSystem.Sample.Init = SACInit;
	myControlSystem.Sample.ChangeState = ChangeSampleState;
	myControlSystem.Sample.Scan = SAC_Scan;
	myControlSystem.Sample.CalRMSCmd = CalRMSCmd;

	myControlSystem.SoftStart.Run = SoftStartRun;

	myControlSystem.Actuator.Init = ActuatorInit;
	myControlSystem.Actuator.ChangeState = ActuatorChangeState;
	myControlSystem.Actuator.GetOutputPhase = ActuatorGetOutputPhase;

	int i = 0;
	for(i = 0;i < SAMPLE_NUM;i++)	//��ʼ������
		UaFifo[i] = 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 ControlSystemInit(void)
//-----------------------------------------------------------------------------
// �������ܣ� ����ϵͳ��ʼ��
//-----------------------------------------------------------------------------
// ����˵������ʼ������ϵͳ����Ӳ������������ADC��ִ�л���EPWM
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ����ϵͳ��ʼ���ɹ��� 1 -> ����ϵͳ��ʼ��ʧ��
//-----------------------------------------------------------------------------
Uint16 ControlSystemInit(void)
{
	Uint16 returnValue = 0;

	returnValue = SACInit();
	if(returnValue)
		return 1;
	returnValue = ActuatorInit();
	if(returnValue)
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 IsAddControl(void)
//-----------------------------------------------------------------------------
// �������ܣ� �жϵ�ǰ״̬�Ƿ����Ͷ�������
//-----------------------------------------------------------------------------
// ����˵��������������ʱ�Զ�Ͷ�������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ������״̬�ı��ˣ� 1 -> ������״̬û�ı�
//-----------------------------------------------------------------------------
Uint16 IsAddControl(void)				//�ж��Ƿ����Ͷ������������ı���״̬
{
	State temp = myControlSystem.Controller.State;

	if((myInverter.U_DCRMS >= 176)&(myInverter.U_DCRMS <= 264))	//���ֵ�ڴ����ڿ�ʼ���� 176 264
		myControlSystem.Controller.State = Enable;

	else
		myControlSystem.Controller.State = Disable;
	   // myInverter.SOFTSTART = 0;
         C_ERROR[3] = myInverter.IA_InvOutRMS;
         C_ERROR[4] = C_ERROR[3];
	   // myControlSystem.Controller.Output = 0.1;

	if(temp == myControlSystem.Controller.State)
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 Control(void)
//-----------------------------------------------------------------------------
// �������ܣ� ����������
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> �������ڣ�4 -> ִ�л���δ���У� ��0λ -> �Ƿ��ֹ�˻��ֻ��ڣ���1λ -> �Ƿ��ڹ���㸽��
//-----------------------------------------------------------------------------
Uint16 Control(void)						//���ƹ���
{
	int16 returnValue = 0;		//����ֵ
	int16 _KiEnable = 1;		//����ʹ��λ

	if(myControlSystem.Actuator.State == Disable)		//ִ�л���δ����
		return 4;

	static int ui = 0;

	myControlSystem.Controller.InsUA_Out = CalRMS(myInverter.UA_InvOut,UaFifo,&UaPtr,&UaSa,&UaPh);	//����˲ʱ��Чֵ

	UA[ui] = myInverter.UA_InvOut;
	UAi[ui] = myControlSystem.Controller.InsUA_Out;
	ui++;
	if(ui >= 150)
		ui = 0;

	//C_ERROR[0] = C_TARGET - myControlSystem.Controller.InsUA_Out;

	C_ERROR[0] = C_TARGET - myInverter.UA_InvOutRMS;
	C_ERROR[0] = C_WEIGHT[0]*C_ERROR[0] + C_WEIGHT[1]*C_ERROR[1] + C_WEIGHT[2]*C_ERROR[2];	//��Ȩ�˲�
	C_ERROR[3] = myInverter.IA_InvOutRMS;
	//C_ERROR[5] =  C_ERROR[3] - C_ERROR[4];

	if((C_ERROR[0] <= -100)|(C_ERROR[0] >= 100))		//���ֵ���󣬽�ֹ����
	{
		_KiEnable = 0;
		returnValue |= 1;
	}
	else if((C_OUTPUT >= 0.95)|(C_OUTPUT <= 0.05))		//������ͣ���ֹ����
	{
		_KiEnable = 0;
		returnValue |= 1;
	}
	else
		_KiEnable = 1;
	float _deltaOut = C_KP*(C_ERROR[0]-C_ERROR[1]) + _KiEnable*KiTs*C_ERROR[0] + I_KP*(C_ERROR[3]-C_ERROR[4]);
    //float _deltaOut = I_KP*(C_ERROR[3]-C_ERROR[4]) ;
	C_OUTPUT = C_OUTPUT + _deltaOut;

	if(C_OUTPUT >= 0.95)			//�����ֵ����
		C_OUTPUT = 0.95;
	else if(C_OUTPUT <= 0.05)
		C_OUTPUT = 0.05;

	COPRD = C_OUTPUT*EPwm1Regs.TBPRD;		//���������ֵ������ڿ��ټ���
	//COPRD = C_OUTPUT*EPwm1Regs.TBPRD;       //���������ֵ������ڿ��ټ���

	C_ERROR[1] = C_ERROR[0];
	C_ERROR[2] = C_ERROR[1];		//������
	C_ERROR[4] = C_ERROR[3];
	//C_ERROR[6] = C_ERROR[5];
	return returnValue;
}

//-----------------------------------------------------------------------------
// ��������float CalRMS(float insValue,float* fifo,int* ptr)
//-----------------------------------------------------------------------------
// �������ܣ� ���ô����������㷨������Чֵ
//-----------------------------------------------------------------------------
// ����˵������
//-----------------------------------------------------------------------------
// ���������	insValue -> ˲ʱֵ
//				fifo -> ��Ӧ��fifo��ָ��
//				ptr -> ָ��fifo����һ����Ҫд���λ�õ�ָ��
//				uSa -> ����ֵ��ָ��
//
//				curPh -> ��ǰ����ֵ����λ
// ��������� 	��Чֵ
//-----------------------------------------------------------------------------
float CalRMS(float insValue,float* fifo,int* ptr,float* uSa,float* curPh)
{
	float lastU = 0;		//ǰһʱ�̵Ĳ���ֵ
	float uKi = 0;
	float sinPh = 0;

	//����FIFO�������㷨���㷽����
	if(*ptr < 0)
	{
		*ptr = SAMPLE_NUM - 1;
		lastU = fifo[0];
	}
	else
		lastU = fifo[*ptr+1];
	*uSa = *uSa + (insValue*insValue-fifo[*ptr]*fifo[*ptr])*SNumRec;
	fifo[*ptr] = insValue;
	float uKh = sqrt(*uSa);

	//����˲ʱֵ������Чֵ DeltaPh
	if((lastU < 0)&(insValue >= 0))
		*curPh = DeltaPh*insValue/(insValue - lastU);
	else
		*curPh = *curPh + DeltaPh;
	if(*curPh == 0)
		uKi = 0;
	else
	{
		sinPh = sin(*curPh);
		uKi = insValue*SQRT2_INV/sinPh;
		if(uKi > 2*uKh)
			uKi = 2*uKh;
		else if(uKi < -2*uKh)
			uKi = -2*uKh;
	}
	(*ptr)--;		//ָ����һ����дλ��

	//������������
	if(sinPh < 0)
		sinPh = -sinPh;
	return (1-sinPh)*uKh+sinPh*uKi;
}

//-----------------------------------------------------------------------------
// ��������Uint16 SACInit(void)
//-----------------------------------------------------------------------------
// �������ܣ� �Բ�������ģ����г�ʼ��
//-----------------------------------------------------------------------------
// ����˵���� 1.��ADCģ����г�ʼ��
//			 2.���ݲ���Ƶ�ʶ�Epwm4��ʼ��
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ���óɹ�,1 -> ����Ƶ�ʹ���,2 -> ����Ƶ�ʹ���,3 -> Ӳ����Դ�ѱ�ʹ��
//-----------------------------------------------------------------------------
Uint16 SACInit(void)						//�������Ƴ�ʼ��
{
	if((myHardwarePool.bit.Adc)|(myHardwarePool.bit.EPWM4))
		return 3;

	if(SAMPLE_FRE > 5722.05)		//������̼�ParameterCalculation.m�в������ƶ�
		return 1;					//����Ƶ�ʹ���
	else if(SAMPLE_FRE < 9.93)
		return 2;				//����Ƶ�ʹ���

	//��ֹ����ʱ��
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;		// ADC
    SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;  	// ePWM4
	EDIS;

	//ADC����
	InitAdc();									//ʹ��ʱ�Ӳ��ϵ�

	AdcRegs.ADCTRL1.bit.SUSMOD = 1;				//�������ģʽ��ת����ɺ��������������߼�ֹͣ���������ս�����棬״̬������
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xf;			//�������ڴ�С��SOC������Ϊ16��ADCLK	SampleTime = 1/ADCCLK*ACQ_PS = 3.2us
	AdcRegs.ADCTRL1.bit.CPS=1;			//��ʱ��Ԥ������������1����ʱ�ӽ���2��Ƶ
	AdcRegs.ADCTRL1.bit.CONT_RUN=0;		//����������/ֹͣģʽ
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1;		//�����з�����ģʽ


	AdcRegs.ADCTRL3.bit.ADCCLKPS=0x4;	//ADCCLK=HSPCLK/(4��2��(CPS+1))=75/16=4.6875M
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;	//˳�����

	AdcRegs.ADCMAXCONV.bit.MAX_CONV1=0xf;	//�ܹ�����16·


	AdcRegs.ADCCHSELSEQ1.bit.CONV00=0x0; 	//����ADCINA0
	AdcRegs.ADCCHSELSEQ1.bit.CONV01=0x1; 	//����ADCINA1
	AdcRegs.ADCCHSELSEQ1.bit.CONV02=0x2; 	//����ADCINA2
	AdcRegs.ADCCHSELSEQ1.bit.CONV03=0x3; 	//����ADCINA3
	AdcRegs.ADCCHSELSEQ2.bit.CONV04=0x4; 	//����ADCINA4
	AdcRegs.ADCCHSELSEQ2.bit.CONV05=0x5; 	//����ADCINA5
	AdcRegs.ADCCHSELSEQ2.bit.CONV06=0x6;	//����ADCINA6
	AdcRegs.ADCCHSELSEQ2.bit.CONV07=0x7;	//����ADCINA7
	AdcRegs.ADCCHSELSEQ3.bit.CONV08=0x8;	//����ADCINB0
	AdcRegs.ADCCHSELSEQ3.bit.CONV09=0x9; 	//����ADCINB1
	AdcRegs.ADCCHSELSEQ3.bit.CONV10=0xa; 	//����ADCINB2
	AdcRegs.ADCCHSELSEQ3.bit.CONV11=0xb; 	//����ADCINB3
	AdcRegs.ADCCHSELSEQ4.bit.CONV12=0xc; 	//����ADCINB4
	AdcRegs.ADCCHSELSEQ4.bit.CONV13=0xd; 	//����ADCINB5
	AdcRegs.ADCCHSELSEQ4.bit.CONV14=0xe; 	//����ADCINB6
	AdcRegs.ADCCHSELSEQ4.bit.CONV15=0xf;	//����ADCINB7

	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;		//���SEQ1�е��жϱ�־λINT_SEQ1
	AdcRegs.ADCST.bit.INT_SEQ2_CLR=1;		//���SEQ2�е��жϱ�־λINT_SEQ2

	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;		//ʹ��SEQ1�ж�
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ=1;	//����EPWM��������SEQ

	//ADC�ж�����
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;		//ʹ��ADC PIE�ж�
	IER |= M_INT1;

	//EPWM4������
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 1;		//ģ��2812�е�HSPCLK
	//����ͨ�����ʵķ�Ƶϵ�������ڼĴ�����ֵ�趨��40%(26214)-80%(52429)֮��
	if(SAMPLE_FRE >= 715.26)
		DIV_FAC = 1;				//2��Ƶ
	else if(SAMPLE_FRE >= 357.63)
		DIV_FAC = 2;				//4��Ƶ
	else if(SAMPLE_FRE >= 178.81)
		DIV_FAC = 3;				//8��Ƶ
	else if(SAMPLE_FRE >= 89.41)
		DIV_FAC = 4;				//16��Ƶ
	else if(SAMPLE_FRE >= 44.70)
		DIV_FAC = 5;				//32��Ƶ
	else if(SAMPLE_FRE >= 22.35)
		DIV_FAC = 6;				//64��Ƶ
	else
		DIV_FAC = 7;				//128��Ƶ

	myControlSystem.Sample.SampleRatio = SAMPLE_FRE/myInverter.FrOut;	//���������
	SampleN = (int16)myControlSystem.Sample.SampleRatio;	//һ�����ڲ�������
	KiTs = myControlSystem.Controller.ControlKi/myControlSystem.Sample.SampleRate;		//Ki*Ts

	EPwm4Regs.TBCTL.bit.SYNCOSEL = 0x3;		//��ֹͬ������ź�

	EPwm4Regs.ETSEL.bit.SOCBEN = 1;			//ʹ��epwm4����adc
	EPwm4Regs.ETSEL.bit.SOCBSEL =2;			//�����¼�����adc
	EPwm4Regs.ETPS.bit.SOCBPRD = 1;			//ÿ���¼�������
	EPwm4Regs.ETCLR.bit.SOCA = 1;			//�����־λ

	EPwm4Regs.TBPRD = (Uint16)(HSPCLK28xx/SAMPLE_FRE/(1<<DIV_FAC)*1000 - 1);
	EPwm4Regs.TBCTR = 0;					//����ֵ����

	return 0;
}

//-----------------------------------------------------------------------------
// ��������int16 ChangeSampleState(State state)
//-----------------------------------------------------------------------------
// �������ܣ� �ı����ģ��״̬
//-----------------------------------------------------------------------------
// ����˵���� ʹ��(��ֹ)epwm4��ʱ��
//-----------------------------------------------------------------------------
// ���������	state -> ����ģ��״̬
// ��������� 	0 -> ���óɹ�,1 -> �����������2 -> �����δʹ��,3 -> Ӳ����Դ�ѱ�ʹ��
//-----------------------------------------------------------------------------
Uint16 ChangeSampleState(State state)	//�ı����״̬��������ֹͣ��
{
	if(state == Enable)
	{
		if(myInverter.State == Disable)
			return 2;			//�����δʹ��
		if((myHardwarePool.bit.Adc)|(myHardwarePool.bit.EPWM4))
			return 3;
		EPwm4Regs.TBCTR = 0;					//����ֵ����
		EPwm4Regs.TBCTL.bit.CTRMODE = 0;		//��������ʼ���ϼ���

		myHardwarePool.bit.EPWM4 = 1;
		myHardwarePool.bit.Adc = 1;
		myControlSystem.Sample.State = Enable;
	}
	else if((state == Disable)&(myControlSystem.Sample.State == Enable))
	{
		EPwm4Regs.TBCTL.bit.CTRMODE = 3;		//ֹͣ������
		EPwm4Regs.TBCTR = 0;					//����ֵ����

		myHardwarePool.bit.EPWM4 = 0;
		myHardwarePool.bit.Adc = 0;
		myControlSystem.Sample.State = Disable;
	}
	else
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// ��������int16 SAC_Scan(void)
//-----------------------------------------------------------------------------
// �������ܣ� ѭ��ɨ���Ƿ���adcת�����������Ҫ�������������д���
//-----------------------------------------------------------------------------
// ����˵���� 1.��ģ���������һ�β����ж�֮ǰִ��һ�Σ�����ᶪʧ�ϴβ�������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> ������һ�����ݴ���,1 -> û�������ݲɼ�,����
//-----------------------------------------------------------------------------
Uint16 SAC_Scan(void)						//������������ѭ��ɨ�裬��һ�����������ڸú�������ִ������һ�Σ�����ᶪʧ�������
{
	static float sumIAInvOut = 0;
	static float sumIBInvOut = 0;
	static float sumICInvOut = 0;
	static float sumIATotalOut = 0;
	static float sumIBTotalOut = 0;
	static float sumICTotalOut = 0;
	static float sumUDC = 0;
	static float sumUAInvOut = 0;
	static float sumUBInvOut = 0;
	static float sumUCInvOut = 0;
	static float sumUABP = 0;
	static float sumUBBP = 0;
	static float sumUCBP = 0;

	if(!(myControlSystem.Sample.SampleFlag&0x2))		//û�����ݸ��£�����
	{
		return 1;
	}
	myControlSystem.Sample.SampleFlag &= ~(1<<1);		//��1λ����

	myInverter.IA_InvOut = SAMPLE_OV[6]*SAMPLE_K[6]+SAMPLE_B[6];		//A6	������������
	myInverter.IB_InvOut = SAMPLE_OV[4]*SAMPLE_K[4]+SAMPLE_B[4];		//A4
	myInverter.IC_InvOut = SAMPLE_OV[3]*SAMPLE_K[3]+SAMPLE_B[3];		//A3

	myInverter.IB_TotalOut = SAMPLE_OV[2]*SAMPLE_K[2]+SAMPLE_B[2];		//A2	���������
	myInverter.IA_TotalOut = SAMPLE_OV[1]*SAMPLE_K[1]+SAMPLE_B[1];		//A1
	myInverter.IC_TotalOut = SAMPLE_OV[5]*SAMPLE_K[5]+SAMPLE_B[5];		//A5

	myInverter.U_DC = SAMPLE_OV[8]*SAMPLE_K[8]+SAMPLE_B[8];				//A0	ֱ��ĸ��

	myInverter.UA_InvOut = -(SAMPLE_OV[11]*SAMPLE_K[11]+SAMPLE_B[11]);		//B4	����������ѹ
	myInverter.UB_InvOut = -(SAMPLE_OV[10]*SAMPLE_K[10]+SAMPLE_B[10]);		//B2
	myInverter.UC_InvOut = -(SAMPLE_OV[9]*SAMPLE_K[9]+SAMPLE_B[9]);		//B1

	myInverter.UC_BP = -(SAMPLE_OV[12]*SAMPLE_K[12]+SAMPLE_B[12]);		//B3		��·��ѹ
	myInverter.UB_BP = -(SAMPLE_OV[14]*SAMPLE_K[14]+SAMPLE_B[14]);		//B6
	myInverter.UA_BP = -(SAMPLE_OV[13]*SAMPLE_K[13]+SAMPLE_B[13]);		//B5

	myControlSystem.Sample.SampleFlag = myControlSystem.Sample.SampleFlag^0x4;		//������־��2λȡ��
	myControlSystem.Sample.SampleFlag |= 0x8;										//������־��3λ��λ

    if(myControlSystem.Controller.State == Enable)          //���ʹ�ܿ�����������п���
    { myControlSystem.Controller.Control();}

	if(myControlSystem.Sample.FlagRMS)		//ʹ����Чֵ����
	{
		if(iCalRMS < SampleN)		//�Ƿ����һ������ƽ���ͼ���
		{
			sumIAInvOut += myInverter.IA_InvOut*myInverter.IA_InvOut;
			sumIBInvOut += myInverter.IB_InvOut*myInverter.IB_InvOut;
			sumICInvOut += myInverter.IC_InvOut*myInverter.IC_InvOut;

			sumIATotalOut += myInverter.IA_TotalOut*myInverter.IA_TotalOut;
			sumIBTotalOut += myInverter.IB_TotalOut*myInverter.IB_TotalOut;
			sumICTotalOut += myInverter.IC_TotalOut*myInverter.IC_TotalOut;

			sumUDC += myInverter.U_DC*myInverter.U_DC;

			sumUAInvOut += myInverter.UA_InvOut*myInverter.UA_InvOut;
			sumUBInvOut += myInverter.UB_InvOut*myInverter.UB_InvOut;
			sumUCInvOut += myInverter.UC_InvOut*myInverter.UC_InvOut;

			sumUABP += myInverter.UA_BP*myInverter.UA_BP;
			sumUBBP += myInverter.UB_BP*myInverter.UB_BP;
			sumUCBP += myInverter.UC_BP*myInverter.UC_BP;
		    iCalRMS++;
		}
		else
		{
			float temp1 = 0;

			temp1 = sumIAInvOut/SampleN;						//sum^2/n
			myInverter.IA_InvOutRMS = sqrt(temp1);				//����������A����Чֵ
			sumIAInvOut = 0;

			temp1 = sumIBInvOut/SampleN;						//sum^2/n
			myInverter.IB_InvOutRMS = sqrt(temp1);				//����������B����Чֵ
			sumIBInvOut = 0;

			temp1 = sumICInvOut/SampleN;						//sum^2/n
			myInverter.IC_InvOutRMS = sqrt(temp1);				//����������C����Чֵ
			sumICInvOut = 0;

			temp1 = sumIATotalOut/SampleN;
			myInverter.IA_TotalOutRMS = sqrt(temp1);			//������������A����Чֵ
			sumIATotalOut = 0;

			temp1 = sumIBTotalOut/SampleN;
			myInverter.IB_TotalOutRMS = sqrt(temp1);			//������������B����Чֵ
			sumIBTotalOut = 0;

			temp1 = sumICTotalOut/SampleN;
			myInverter.IC_TotalOutRMS = sqrt(temp1);			//������������C����Чֵ
			sumICTotalOut = 0;

			temp1 = sumUDC/SampleN;
			myInverter.U_DCRMS = sqrt(temp1);					//DCĸ�ߵ�ѹ��Чֵ
			sumUDC = 0;

			temp1 = sumUAInvOut/SampleN;
			myInverter.UA_InvOutRMS = sqrt(temp1);				//������A���ѹ��Чֵ
			sumUAInvOut = 0;

			temp1 = sumUBInvOut/SampleN;
			myInverter.UB_InvOutRMS = sqrt(temp1);				//������B���ѹ��Чֵ
			sumUBInvOut = 0;

			temp1 = sumUCInvOut/SampleN;
			myInverter.UC_InvOutRMS = sqrt(temp1);				//������C���ѹ��Чֵ
			sumUCInvOut = 0;

			temp1 = sumUABP/SampleN;
			myInverter.UA_BPRMS = sqrt(temp1);					//��·A���ѹ��Чֵ
			sumUABP = 0;

			temp1 = sumUBBP/SampleN;
			myInverter.UB_BPRMS = sqrt(temp1);					//��·B���ѹ��Чֵ
			sumUBBP = 0;

			temp1 = sumUCBP/SampleN;
			myInverter.UC_BPRMS = sqrt(temp1);					//��·C���ѹ��Чֵ
			sumUCBP = 0;

			if(SOFT_START == 0)
			{
			    myControlSystem.Controller.IsAddControl();
			}
			else if(SOFT_START == 1)
			{
			    myControlSystem.Controller.State = Disable;
			}
		    //if(myControlSystem.Controller.State == Enable)          //���ʹ�ܿ�����������п���
		       // myControlSystem.Controller.Control();

			iCalRMS = 0;
			myControlSystem.Sample.SampleFlag = myControlSystem.Sample.SampleFlag^0x10;		//������־��4λȡ��
			myControlSystem.Sample.SampleFlag |= 0x20;										//������־��5λ��λ
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 CalRMSCmd(Bool cmd)
//-----------------------------------------------------------------------------
// �������ܣ� ʹ��(��ֹ)��Чֵ����
//-----------------------------------------------------------------------------
// ����˵���� ��
//-----------------------------------------------------------------------------
// ���������	cmd -> �Ƿ�ʹ����Чֵ����
// ��������� 	0 -> �������,1 -> �����ȹ�С,2 -> �����ȹ���
//				4 -> �����������
//-----------------------------------------------------------------------------
Uint16 CalRMSCmd(Bool cmd)
{
	if(cmd == True)
	{
		if(myControlSystem.Sample.SampleRatio < 4)		//�����ȹ�С��������Чֵ
		{
			myControlSystem.Sample.FlagRMS = False;
			return 1;
		}
		else if(myControlSystem.Sample.SampleRatio > 60)		//�����ȹ��󲻼�����Чֵ
		{
			myControlSystem.Sample.FlagRMS = False;
			return 2;
		}
		else
			myControlSystem.Sample.FlagRMS = True;
	}
	else if(cmd == False)
	{
		iCalRMS = 0;		//������Чֵͳ�Ʋ���������
	}
	else
		return 4;

	return 0;
}

//-----------------------------------------------------------------------------
// ��������Uint16 SoftStartRun(void)
//-----------------------------------------------------------------------------
// �������ܣ� ������
//-----------------------------------------------------------------------------
// ����˵���� 1.��Ҫʹ��CPU��ʱ��1
//			 2.��������в���Ͷ�������������������״̬������ǰһ��
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> �������,1 -> ��ʱ��1�ѱ�ʹ��,2 -> ��ǰֵ��Ŀ��ֵ�������
//				3 -> �趨ֵ������Χ
//-----------------------------------------------------------------------------
Uint16 SoftStartRun(void)					//������
{
	float deltaUout = myControlSystem.SoftStart.SS_Target - C_OUTPUT;
	if((deltaUout > -0.1)&(deltaUout < 0.1))		//��ǰֵ��Ŀ��ֵ���������������
		return 2;
	if((myControlSystem.SoftStart.SS_Target<0)|(myControlSystem.SoftStart.SS_Target>1))
		return 3;
	if(myHardwarePool.bit.CpuTimer1)		//Cpu��ʱ��1�ѱ�ʹ��
		return 1;
	myHardwarePool.bit.CpuTimer1 = 1;		//Cpu��ʱ��1���ڱ�ʹ��

	Uint16 _TperStep = myControlSystem.SoftStart.SS_Time/myControlSystem.SoftStart.SS_Stairs;	//ms
	ConfigCpuTimer(&CpuTimer1,150,_TperStep*1000.0);
	RemainStep = myControlSystem.SoftStart.SS_Stairs;		//ʱ�䲽��
	OutPerStep = deltaUout/myControlSystem.SoftStart.SS_Stairs;	//�������
	XIntruptRegs.XNMICR.bit.SELECT = 0;			//Timer1����int13

	IER |= M_INT13; 		/*��ʱ��1���ж�*/
	StartCpuTimer1();		//������ʱ��1

	while(RemainStep > 0)				//�ȴ����������
	{
		myControlSystem.Sample.Scan();	//����ȴ������л���в���
		myPhaseLock.Scan();				//�����������
        //myCommunication.RS485Com.Scan();    //485扫描函数
        //mySystemTick.ScanCallback();        //滴答时钟回调函数
	}
   	StopCpuTimer1();					//ֹͣ��ʱ��1
	myHardwarePool.bit.CpuTimer1 = 0;		//Cpu��ʱ��1ʹ�����
	return 0;
}


Uint16 StartRun(void)                   //������
{
    float deltaUout1 = 0.7 - C_OUTPUT1;
    if((deltaUout1 > -0.1)&(deltaUout1 < 0.1))        //��ǰֵ��Ŀ��ֵ���������������
        return 2;
    if(myHardwarePool.bit.CpuTimer1)        //Cpu��ʱ��1�ѱ�ʹ��
        return 1;
    myHardwarePool.bit.CpuTimer1 = 1;       //Cpu��ʱ��1���ڱ�ʹ��

 //   Uint16 _TperStep1 = myControlSystem.SoftStart.SS_Time/myControlSystem.SoftStart.SS_Stairs;   //ms
    ConfigCpuTimer(&CpuTimer1,150,300*1000.0);
    RemainStep1 = 10;       //ʱ�䲽��
    OutPerStep1 = deltaUout1/10; //�������
    XIntruptRegs.XNMICR.bit.SELECT = 0;         //Timer1����int13

    IER |= M_INT13;         /*��ʱ��1���ж�*/
    StartCpuTimer1();       //������ʱ��1

    while(RemainStep1 > 0)               //�ȴ����������
    {
        myControlSystem.Sample.Scan();  //����ȴ������л���в���
        myPhaseLock.Scan();  //�����������

        //myCommunication.RS485Com.Scan();    //485扫描函数
        //mySystemTick.ScanCallback();        //滴答时钟回调函数
    }
    if(myFaultDetect.FlagOther.all||myFaultDetect.FlagRms.all)
    {
        myInverter.SOFTSTART = 0;
    }
    else
    {
        myInverter.SOFTSTART = 1;
    }
    StopCpuTimer1();                    //ֹͣ��ʱ��1
    myHardwarePool.bit.CpuTimer1 = 0;       //Cpu��ʱ��1ʹ�����
    return 0;
}

Uint16 StartRun2(void)                   //������
{
    float deltaUout2 = 0.7 - C_OUTPUT2;
    if((deltaUout2 > -0.1)&(deltaUout2 < 0.1))        //��ǰֵ��Ŀ��ֵ���������������
        return 2;
    if(myHardwarePool.bit.CpuTimer1)        //Cpu��ʱ��1�ѱ�ʹ��
        return 1;
    myHardwarePool.bit.CpuTimer1 = 1;       //Cpu��ʱ��1���ڱ�ʹ��

 //   Uint16 _TperStep1 = myControlSystem.SoftStart.SS_Time/myControlSystem.SoftStart.SS_Stairs;   //ms
    ConfigCpuTimer(&CpuTimer1,150,300*1000.0);
    RemainStep2 = 10;       //ʱ�䲽��
    OutPerStep2 = deltaUout2/10; //�������
    XIntruptRegs.XNMICR.bit.SELECT = 0;         //Timer1����int13

    IER |= M_INT13;         /*��ʱ��1���ж�*/
    StartCpuTimer1();       //������ʱ��1

    while(RemainStep2 > 0)               //�ȴ����������
    {
        myControlSystem.Sample.Scan();  //����ȴ������л���в���
        myPhaseLock.Scan();  //�����������
    }
    if((myPhaseLock.PSD_BYP == NoShift)&&(myFaultDetect.FlagOther.all==0)&&(myFaultDetect.FlagRms.all==0))
    {
        myFpgaDriver.InvSwitchOn(Enable);
        CAN.SendDataWithPtc2(0,AutoSendMailBox);   //��������·
    }
    else
    {
        myFpgaDriver.InvSwitchOn(Disable);
    }
    StopCpuTimer1();                    //ֹͣ��ʱ��1
    myHardwarePool.bit.CpuTimer1 = 0;       //Cpu��ʱ��1ʹ�����
    return 0;
}
//-----------------------------------------------------------------------------
// ��������int16 ActuatorInit(void)
//-----------------------------------------------------------------------------
// �������ܣ� ��ִ�л������г�ʼ��
//-----------------------------------------------------------------------------
// ����˵���� 1.����GPIO0-3�˿�ΪPWM���ܿ�
//			 2.����Epwm1,2����
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	0 -> �������,1 -> �ز�Ƶ�ʹ���,2 -> �ز�Ƶ�ʹ���,3 -> ����ʱ�����
//				4 -> EMPW�ѱ�ռ��
//-----------------------------------------------------------------------------
Uint16 ActuatorInit(void)					//ִ�л�����ʼ��
{
	if(myControlSystem.Actuator.CarFre >25)
		return 1;
	if(myControlSystem.Actuator.CarFre <10)
		return 2;
	if((myControlSystem.Actuator.DBTime >= 13.5)|(myControlSystem.Actuator.DBTime<0))
		return 3;
	if((myHardwarePool.bit.EPWM1)&(myHardwarePool.bit.EPWM2))
		return 4;

	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;			//��GPIO0-3����Ϊspwm���ܿ�
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;			//��GPIO0-3����Ϊspwm���ܿ�
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;			//��GPIO0-3����Ϊspwm���ܿ�
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;			//��GPIO0-3����Ϊspwm���ܿ�

	//ʹ������ʱ��
	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
	EDIS;

	//Epwm1����
	//ʱ����Ԫ����
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;
	EPwm1Regs.TBCTL.bit.CLKDIV = 0;		//ʱ�Ӳ���Ƶ��Epwm1Clk = HSPCLK = 75000k
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 0x01;	//ctr=0ʱ�����ͬ���ź�
	EPwm1Regs.TBPRD = (Uint16)(HSPCLK28xx/myControlSystem.Actuator.CarFre/2);		//�������ڼĴ���ֵ
	EPwm1Regs.TBCTR = 0;			
	//�Ƚϵ�Ԫ����
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0;		//ʹ����Ӱ�Ĵ�������cnt=0ʱ��װ��
	EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = 0;		//ʹ����Ӱ�Ĵ�������cnt=0ʱ��װ��
	//������Ԫ����
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;		//Aƥ��֮��Ϊ��
	//������Ԫ����
	EPwm1Regs.DBCTL.bit.IN_MODE = 0;		//S(4,5)=(0,0)
	EPwm1Regs.DBCTL.bit.POLSEL = 2;			//S(2,3)=(0,1)
	EPwm1Regs.DBCTL.bit.OUT_MODE = 3;		//S(0,1)=(1,1)
	EPwm1Regs.DBRED = (Uint16)(myControlSystem.Actuator.DBTime*75);
	EPwm1Regs.DBFED = EPwm1Regs.DBRED;		//��������ʱ��
	//ն��ģ������
	EPwm1Regs.PCCTL.bit.CHPEN = 0;			//��ֹPWMն��
	//�¼�����ģ��
	EPwm1Regs.ETSEL.bit.INTEN = 1;			//ʹ��EPWM�ж�
	EPwm1Regs.ETSEL.bit.INTSEL = 2;			//����ֵ��������ֵʱ�����ж�
	EPwm1Regs.ETPS.bit.INTPRD = 1;			//ÿ���¼��������ж�
	EPwm1Regs.ETCLR.bit.INT = 1;			//����жϱ�־λ

	//Epwm2����
	//ʱ����Ԫ����
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 1;
	EPwm2Regs.TBCTL.bit.CLKDIV = 0;			//ʱ�Ӳ���Ƶ��Epwm1Clk = HSPCLK = 75000k
	EPwm2Regs.TBCTL.bit.SYNCOSEL = 0x3;		//��ֹ���ͬ���ź�
	EPwm2Regs.TBCTL.bit.PHSEN = 1;			//ʹ����λͬ��
	EPwm2Regs.TBCTL.bit.PHSDIR = 1;			//ͬ��֮�����ϼ���
	EPwm2Regs.TBPRD = EPwm1Regs.TBPRD;		//�������ڼĴ���ֵ
	EPwm2Regs.TBCTR = 0;
	EPwm2Regs.TBPHS.half.TBPHS = 0;
	//�Ƚϵ�Ԫ����
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = 0;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = 0;		//ʹ����Ӱ�Ĵ�������cnt=0ʱ��װ��
	EPwm2Regs.CMPCTL.bit.LOADAMODE = 0;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = 0;		//ʹ����Ӱ�Ĵ�������cnt=0ʱ��װ��
	//������Ԫ����
	EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;		//Aƥ��֮��Ϊ��
	//������Ԫ����
	EPwm2Regs.DBCTL.bit.IN_MODE = 0;		//S(4,5)=(0,0)
	EPwm2Regs.DBCTL.bit.POLSEL = 2;			//S(2,3)=(0,1)
	EPwm2Regs.DBCTL.bit.OUT_MODE = 3;		//S(0,1)=(1,1)
	EPwm2Regs.DBRED = (Uint16)(myControlSystem.Actuator.DBTime*75);
	EPwm2Regs.DBFED = EPwm1Regs.DBRED;		//��������ʱ��
	//ն��ģ������
	EPwm2Regs.PCCTL.bit.CHPEN = 0;			//��ֹPWMն��

	myControlSystem.Actuator.CarRatio = 1000*myControlSystem.Actuator.CarFre/myInverter.FrOut;	//�ز���
	PhasePerStep = 2*PI/myControlSystem.Actuator.CarRatio;								//ÿ��һ���ز�,�����ƶ��ĵ�Ƕ�
	COPRD = C_OUTPUT*EPwm1Regs.TBPRD;		//���������ֵ������ڿ��ټ���

	//�жϼĴ�������
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;		//ʹ��epwm1 PIE�ж�
	IER |= M_INT3;						//ȫ���ж�3

	return 0;
}

//-----------------------------------------------------------------------------
// ��������int16 ChangeSampleState(State state)
//-----------------------------------------------------------------------------
// �������ܣ�  �ı�ִ�л���״̬
//-----------------------------------------------------------------------------
// ����˵���� ʹ��(��ֹ)T3��ʱ��
//-----------------------------------------------------------------------------
// ���������	state -> ִ�л���״̬
// ��������� 	0 -> ���óɹ�,1 -> �����������2 -> �����δʹ�ܣ�4 -> EPW�ѱ�ռ��
//-----------------------------------------------------------------------------
Uint16 ActuatorChangeState(State state)		//ִ�л����ı�״̬
{
	if(state == Enable)
	{
		if(myInverter.State == Disable)
			return 2;			//�����δʹ��
		if((myHardwarePool.bit.EPWM1)&(myHardwarePool.bit.EPWM2))
			return 4;
		EPwm2Regs.TBCTR = 0;
		EPwm2Regs.TBCTL.bit.CTRMODE = 2;		//��������ģʽ
		EPwm1Regs.TBCTR = 0;
		EPwm1Regs.TBCTL.bit.CTRMODE = 2;		//��������ģʽ

		myHardwarePool.bit.EPWM1 = 1;
		myHardwarePool.bit.EPWM2 = 1;
		myControlSystem.Actuator.State = Enable;
	}
	else if((state == Disable)&(myControlSystem.Actuator.State == Enable))
	{
		EPwm2Regs.TBCTR = 0;
		EPwm2Regs.TBCTL.bit.CTRMODE = 3;		//ֹͣ����ģʽ
		EPwm1Regs.TBCTR = 0;
		EPwm1Regs.TBCTL.bit.CTRMODE = 3;		//ֹͣ����ģʽ

		myHardwarePool.bit.EPWM1 = 0;
		myHardwarePool.bit.EPWM2 = 0;
		myControlSystem.Actuator.State = Disable;
	}
	else
		return 1;
	return 0;
}

//-----------------------------------------------------------------------------
// ��������float ActuatorGetOutputPhase(void)
//-----------------------------------------------------------------------------
// �������ܣ�  ��ȡִ�л�����ǰ�������λ
//-----------------------------------------------------------------------------
// ����˵���� ��
//-----------------------------------------------------------------------------
// ���������	state -> ִ�л���״̬
// ��������� 	0-2*�� -> ��ǰ�����λ,-1 -> ִ�л���δ����
//-----------------------------------------------------------------------------
float ActuatorGetOutputPhase(void)			//ִ�л�����ȡ�����λ
{
	if(myControlSystem.Actuator.State == Disable)
		return -1;
	if(CurPhase < 0)
		CurPhase = CurPhase + _2PI;
	if(CurPhase >= _2PI)
		CurPhase = CurPhase - _2PI;
	return CurPhase;
}

//-----------------------------------------------------------------------------
//����Ϊ���õ��Ķ�Ӧ�жϳ���
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ��������interrupt ADC_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ� ADC�����ж�
//-----------------------------------------------------------------------------
// ����˵���� 1.���Ĵ���ֵ�Ҷ�����б���
//			 2.�жϷ����в������ݽ��й��ദ��
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void ADC_ISR(void)
{
	SAMPLE_OV[8] = AdcRegs.ADCRESULT0>>4;
	SAMPLE_OV[1] = AdcRegs.ADCRESULT1>>4;
	SAMPLE_OV[2] = AdcRegs.ADCRESULT2>>4;
	SAMPLE_OV[3] = AdcRegs.ADCRESULT3>>4;
	SAMPLE_OV[4] = AdcRegs.ADCRESULT4>>4;
	SAMPLE_OV[5] = AdcRegs.ADCRESULT5>>4;
	SAMPLE_OV[6] = AdcRegs.ADCRESULT6>>4;
	SAMPLE_OV[7] = AdcRegs.ADCRESULT7>>4;
	SAMPLE_OV[0] = AdcRegs.ADCRESULT8>>4;
	SAMPLE_OV[9] = AdcRegs.ADCRESULT9>>4;
	SAMPLE_OV[10] = AdcRegs.ADCRESULT10>>4;
	SAMPLE_OV[12] = AdcRegs.ADCRESULT11>>4;
	SAMPLE_OV[11] = AdcRegs.ADCRESULT12>>4;
	SAMPLE_OV[13] = AdcRegs.ADCRESULT13>>4;
	SAMPLE_OV[14] = AdcRegs.ADCRESULT14>>4;
	SAMPLE_OV[15] = AdcRegs.ADCRESULT15>>4;

	SamplePhase = myControlSystem.Actuator.GetOutputPhase();	//��ȡ��ǰ�����λ

	myControlSystem.Sample.SampleFlag = myControlSystem.Sample.SampleFlag^0x1;		//������־��0λȡ��
	myControlSystem.Sample.SampleFlag |= 0x2;								//������־��1λ��λ

	PieCtrlRegs.PIEACK.bit.ACK1 = 1;		//��ӦPIEͬ���ж�
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;		//���AD�жϵı�־
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;		//��λSEQ1Ϊ��ʼ״̬CON00
	EINT;  									//ʹ��ȫ���ж�

}

//-----------------------------------------------------------------------------
// ��������interrupt T3Prd_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ� T3��ʱ�������ж�
//-----------------------------------------------------------------------------
// ����˵���� 1.���ݵ�ǰ���Ʋ������Ƕȸı�Ƚ�ֵ
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void Epwm1_ISR(void)
{
	static int flagNewPrd = 1;		//Ϊ1ʱ����ʾ��Ҫ�����µ�һ������

	if(flagNewPrd)		//�µ�һ������
	{
		flagNewPrd = 0;
		myPhaseLock.ZeroTimeAct = ECap2Regs.TSCTR;	//����������������ʱ�̣���������
		CurPhase = 0;
		if(myPhaseLock.State == Enable)					//ʹ������
		{
			if(myPhaseLock.PSD == LeftShift)	//�����λ����
				CurPhase = 0.02617993;			//�����1.5��(ʵ��Ϊһ���ز���Խ��Ƕȵ�������)
			else if(myPhaseLock.PSD == RightShift)	//����
				CurPhase = -0.02617993;			//�����1.5��(ͬ�ϣ����Ե�һ���ز���Խ��Ƕȴ���1.5ʱ���޷�����)
			else
				CurPhase = 0;
		}
		else
			CurPhase = 0;
	}

	if(CurPhase <= PI)		//ǰ������
	{
		EPwm2Regs.CMPA.half.CMPA = 0;		//V1ͨ,V2��
		EPwm1Regs.CMPA.half.CMPA = (Uint16)(C_OUTPUT*EPwm1Regs.TBPRD*sin(CurPhase));		//���������������ֵ��sin(CurPhase)
		//EPwm2Regs.CMPA.half.CMPA = (Uint16)(COPRD);       //���������������ֵ��sin(CurPhase)
	}
	else					//�������
	{
		EPwm1Regs.CMPA.half.CMPA = 0;		//V3ͨ,V4��
		EPwm2Regs.CMPA.half.CMPA = (Uint16)(C_OUTPUT*EPwm1Regs.TBPRD*sin(CurPhase-PI));	//���������������ֵ��sin(CurPhase)
		//EPwm1Regs.CMPA.half.CMPA = (Uint16)(COPRD);    //���������������ֵ��sin(CurPhase)
	}
	CurPhase = CurPhase + PhasePerStep;		//������һ�������λ
	if(CurPhase >= _2PI)
		flagNewPrd = 1;						//��ʼ�µ�һ������

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
	EPwm1Regs.ETCLR.bit.INT = 1;			//����жϱ�־λ
	EINT;
}

//-----------------------------------------------------------------------------
// ��������interrupt void CpuTimer1_ISR(void)
//-----------------------------------------------------------------------------
// �������ܣ� CPU��ʱ��1�����ж�
//-----------------------------------------------------------------------------
// ����˵��������������
//-----------------------------------------------------------------------------
// ���������	��
// ��������� 	��
//-----------------------------------------------------------------------------
interrupt void CpuTimer1_ISR(void)
{
    if(myInverter.SOFTSTART == 1)
    {
        if(SOFT_START_STATE == 0)
        {
          C_OUTPUT = C_OUTPUT + OutPerStep;
          RemainStep--;
        }
    }
    if(myInverter.SOFTSTART==0)
    {
        if(SOFT_START == 1)
        {
          C_OUTPUT1 = C_OUTPUT1 + OutPerStep1;
          RemainStep1--;
        }
    }
    if(myInverter.SOFTSTART == 1)
    {
        if(SOFT_START_STATE == 2)
        {
          C_OUTPUT2 = C_OUTPUT2 + OutPerStep2;
          RemainStep2--;
        }
    }
	CpuTimer1Regs.TCR.bit.TIF = 1;		//�����־λ
}
