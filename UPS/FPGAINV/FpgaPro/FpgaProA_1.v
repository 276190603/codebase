//�Ե�ַ���к궨��
`define FPGAADD1 4'h0	//��λ��ʾ���£�0:�������ʹ��,1:���������,2:����ʻ����բ,DSP���
`define FPGAADD2 4'h1	//��λ��ʾ���£�0:��������״̬,1:���������״̬,2:��Ҫת��·״̬,3:�¶ȹ���״̬
								//4-6:�����ű۴���״̬,DSP����

module FpgaProA_1(
// input
input YULIU ,  		//���ؼ�⣬0 -> �������غ���
input Q2_ON ,			//��俪�������ź�,0 -> ���������
input BP_ON ,			//0 -> ��Ҫת��·
input WG1 ,				//0 -> �¶ȹ���
input CLK0 ,			//CLK0
input CLK4 ,			//CLK4
input [6:1] PWM  ,	//PWM����
input [3:0] XA  ,		//XA��ַ����
input XRDn,				//DSP��ʹ��
input XWE0n,			//DSPдʹ��
input XZCS7n ,			//DSPƬѡ
input SO4 ,				//�ű�A�Ƿ����, 1 -> ����
input SO5 ,				//---
input SO6 ,				//---
// output
output TRIP1 ,			//1 -> �������뿪����բ
output TRIP2 ,			//0 -> �������Ӵ�������
output [6:1] DR  ,	//�������
output ENABLE ,		//�������ʹ��, 0 -> ʹ��
output INT1 ,			//DSP�ж�����
//inout
inout [7:0] XD 		//��������

);

reg [7:0] XD_reg;		//�������߻���Ĵ���
//����DSP����������ź�(ȫ���Ͷ�Ӧ�����෴)
reg YULIU_reg;		//�������ؼĴ���,1 -> ����
reg Q2_ON_reg;		//��������ؼĴ���,1 -> ����
reg BP_ON_reg;		//��Ҫת��·�Ĵ���,1 -> ��Ҫ
reg WG1_reg;		//�¶ȹ��߼Ĵ���,1 -> �¶ȹ���
reg SO4_reg;		//�ű�A�Ƿ����, 1 -> ����
reg SO5_reg;		//---
reg SO6_reg;		//---

//���ڴ���DSP�жϵ��м��źŶ���
wire [6:0] Signal;		//����Ҫ�͸�DSP���ź����һ��
reg [6:0] SignalDelay;	//�����źŵ��ӳ�
wire [6:0] SignalPulse;	//�����źŵ�����,�����ź�״̬�ı�ʱ����Ӧ��λ�ϻز���һ������
wire SignalPulseTotal;	//�����źŻ�һ���ź�����
reg [15:0] IntCnt;		//�����ж�������ڵļ�����
reg INT1_reg;				//DSP�ж�����

//�͵�DSP���������ź�
reg TRIP1_reg;		//�������뿪����բ�Ĵ�����1 -> ��բ
reg TRIP2_reg;		//�������Ӵ������ϼĴ���, 1 -> �Ӵ�������(������ź��෴)
reg ENABLE_reg;	//�������ʹ�ܼĴ���,1 -> ʹ���������

//PWM�����־λ������źŶ���
reg EnableFlag_reg;				//Ϊ0ʱ��ֹ���
reg [6:1] Pwm_reg;				//���浱ǰʱ�̵�PWM�ź�
reg [6:1] PwmLast_reg;			//������һʱ�̵�PWM�ź�
reg [15:0] EnCnt_reg;			//���Խ�IGBT��ͨʱ�����ʱ�����Զ����EnableFlag_reg,����Ϊ������ʹ��

//Initial���������ǲ����ۺϵ���䣬�������ڴ󲿷�FPGA��֧��
//Ҳ����Quatus
initial
begin
		IntCnt = 16'b0;
		INT1_reg = 0;
		TRIP1_reg = 0;
		TRIP2_reg = 0;
		ENABLE_reg = 0;
		
		EnableFlag_reg = 0;
		EnCnt_reg = 16'b0;
end

//����DSP֮��������źţ�ȫ������Ϊ�ߵ�ƽ��Ч
always@(YULIU,Q2_ON,BP_ON,WG1,SO4,SO5,SO6)
begin
	YULIU_reg = ~YULIU;
	Q2_ON_reg = ~Q2_ON;
	BP_ON_reg = ~BP_ON;
	WG1_reg = ~WG1;
	SO4_reg =  SO4;
	SO5_reg =  SO5;
	SO6_reg =  SO6;
end

//ÿ���źŸı䣬����һ��������(10us)���ж�����
assign Signal[6:0] = {SO6,SO5,SO4,WG1,BP_ON,Q2_ON,YULIU};	//�����ź�����
always@(posedge CLK0)
begin
	SignalDelay[6:0] <= Signal[6:0];		//�����źŵ��ӳ�
end
assign SignalPulse[6:0] = Signal[6:0]^SignalDelay[6:0];	//ÿ���źű仯��Ӧλ����һ������
assign SignalPulseTotal =| SignalPulse[6:0];					//���������尴λ��һ���ź�����
always@(posedge CLK0)
begin
	if(SignalPulseTotal)
	begin
			INT1_reg <= 1'b1;
			IntCnt <= IntCnt+16'd1;	//����ֵ��Ϊ���ʾ��ʼ�����ж�����
	end
	else if(IntCnt != 16'd0)
	begin
			IntCnt <= IntCnt + 16'd1;
			INT1_reg <= 1'b0;			//״̬��λ
	end
	else if(IntCnt >= 16'd500)		//��Ϊʱ��Ƶ��Ϊ50Mhz 5
	begin
			IntCnt <= 16'd0;
			INT1_reg <= 1'b0;			//״̬��λ
	end
end
assign INT1 = INT1_reg;

//�͵�DSP֮�������źţ�ȫ������Ϊ�ߵ�ƽ��Ч
assign ENABLE = ~ENABLE_reg;
assign TRIP1 = TRIP1_reg;
assign TRIP2 = ~TRIP2_reg;

//PWM����ź�
assign DR[1] = EnableFlag_reg?PWM[1]:1'h0;
assign DR[2] = EnableFlag_reg?PWM[2]:1'h0;
//assign DR[3] = EnableFlag_reg?PWM[3]:1'h0;
//assign DR[4] = EnableFlag_reg?PWM[4]:1'h0;
assign DR[5] = EnableFlag_reg?PWM[3]:1'h0;
assign DR[6] = EnableFlag_reg?PWM[4]:1'h0;


//����PWM��ʧһ��ʱ����Զ���ֹPWM���
always@(posedge CLK0)
begin
	Pwm_reg <= PWM;
	if(Pwm_reg==PwmLast_reg)		//����һ��PWM�����ͬ
	begin
		if(EnableFlag_reg == 1'b1)
			EnCnt_reg <= EnCnt_reg + 16'b1;
		if(EnCnt_reg >= 16'd50000)
		begin
			EnableFlag_reg <= 1'b0;
		end
		else
		begin
			EnableFlag_reg <= 1'b1;
		end
	end
	else
	begin
		EnableFlag_reg <= 1'b1;
		EnCnt_reg <= 16'b0;
	end
	PwmLast_reg <= Pwm_reg;
end

//DSP���ź�����
always@(negedge XRDn)
begin
	if(XZCS7n == 1'b0)		//Ƭѡѡ��
	begin
		case (XA)
			`FPGAADD2:
			begin
				XD_reg[6:0] <= {SO6_reg,SO5_reg,SO4_reg,WG1_reg,BP_ON_reg,Q2_ON_reg,YULIU_reg};	//��״̬����DSP��
			end
			default:
			begin
//				XD_reg <= 8'bz;
			end
		endcase
	end
end
//�����ź���ʱ,FPGA������������
assign XD[7:0] = ((XZCS7n==1'b0)&(XRDn == 1'b0))?XD_reg[7:0]:8'hz;

//DSPд�ź�����
always@(posedge XWE0n)
begin
	if(XZCS7n == 1'b0)			//Ƭѡѡ��
	begin
		case (XA)
			`FPGAADD1:
			begin
				{TRIP1_reg,TRIP2_reg,ENABLE_reg} <= XD[2:0];	//��ʹ���ź�����FPGA
			end
			default:
			begin
				
			end
		endcase
	end
end

endmodule