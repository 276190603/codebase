//�Ե�ַ���к궨��
`define FPGAADD1 4'h0	//��λ��ʾ���£�0:�������ʹ��,1:���������,2:����ʻ����բ,DSP���
`define FPGAADD2 4'h1	//��λ��ʾ���£�0:��������״̬,1:���������״̬,2:��Ҫת��·״̬,3:�¶ȹ���״̬
module SCR(
input CLK  ,
input ctrl,
input ctrl1,
input YULIU,
input Q2_ON ,			//��俪�������ź�,0 -> ���������
input BP_ON ,			//0 -> ��Ҫת��·
input WG1 ,				//0 -> �¶ȹ���

input [3:0] XA  ,		//XA��ַ����
input XRDn,				//DSP��ʹ��
input XWE0n,			//DSPдʹ��
input XZCS7n ,			//DSPƬѡ
input SO4 ,				//�ű�A�Ƿ����, 0 -> ����
input SO5 ,				//---
input SO6 ,				//---

output TRIP1 ,			//1 -> �������뿪����բ
output TRIP2 ,			//0 -> �������Ӵ�������
output ENABLE ,		//�������ʹ��, 0 -> ʹ��
output INT1 ,			//DSP�ж�����
output SCR1,


//inout
inout [7:0] XD 		//��������
);


reg SCR1_reg ;



reg [23:0] Cnt_SCR;

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
reg [2:1] Pwm13_reg;				//���浱ǰʱ�̵�PWM1,3�ź�
reg [2:1] Pwm13Last_reg;		//������һʱ�̵�PWM1,3�ź�
reg [15:0] EnCnt_reg;			//���Խ�IGBT��ͨʱ�����ʱ�����Զ����En





initial

begin
		IntCnt = 16'b0;
		INT1_reg = 0;
		TRIP1_reg = 0;
		TRIP2_reg = 0;
		EnableFlag_reg = 0;
		EnCnt_reg = 16'b0;
	   ENABLE_reg = 1'b1;
	   SCR1_reg = 0;
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
always@(posedge CLK)
begin
	SignalDelay[6:0] <= Signal[6:0];		//�����źŵ��ӳ�
end
assign SignalPulse[6:0] = Signal[6:0]^SignalDelay[6:0];	//ÿ���źű仯��Ӧλ����һ������
assign SignalPulseTotal =| SignalPulse[6:0];					//���������尴λ��һ���ź�����
always@(posedge CLK)
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


//PWM����ź�EnableFlag_reg
/*
always@(posedge CLK)
if(ctrl1 == 1'b0)
begin
EnableFlag_reg = 1'b1;

end
*/
always@(posedge CLK)
if(ctrl1 == 1'b0)
begin
if(ctrl == 1'b1)
begin
SCR1_reg = 1'b1;
EnableFlag_reg = 1'b1;
end
else
begin
SCR1_reg =1'b0;
EnableFlag_reg = 1'b1;
end

end
else
begin
SCR1_reg =1'b0;
EnableFlag_reg = 1'b0;
end
assign SCR1 = EnableFlag_reg?SCR1_reg:1'h0;
//assign SCR1 = ctrl?1'h1:1'h0;

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