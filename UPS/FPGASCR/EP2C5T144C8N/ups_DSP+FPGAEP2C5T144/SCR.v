module SCR(
SCR1,
SCR2,
SCR3,
SCR4,
SCR5,
SCR6,
CLK,
ctrl,
ctrl1,
YULIU ,  		//���ؼ�⣬0 -> �������غ���
Q2_ON ,			//��俪�������ź�,0 -> ���������
BP_ON ,			//0 -> ��Ҫת��·
WG1 ,				//0 -> �¶ȹ���
//[3:0] XA  ,		//XA��ַ����
XRDn,				//DSP��ʹ��
XWE0n,			//DSPдʹ��
XZCS7n ,			//DSPƬѡ
SO4 ,				//�ű�A�Ƿ����, 0 -> ����
SO5 ,				//---
SO6 ,				//---
//[3:0] XA  ,		//XA��ַ����
XA0,
XA1,
XA2,
XA3,
//OUTPUT
ENABLE,
TRIP1 ,			//1 -> �������뿪����բ
TRIP2 ,			//0 -> �������Ӵ�������


INT1 ,			//DSP�ж�����
//inout
XD0 	,	//��������
XD1 	,	//��������
XD2 	,	//��������
XD3 	,	//��������
XD4 	,	//��������XD0 	,	//��������
XD5 	,	//��������
XD6 	,	//��������
XD7 		//��������


);

output TRIP1 ;			//1 -> �������뿪����բ
output TRIP2 ;			//0 -> �������Ӵ�������
output ENABLE ;		//�������ʹ��, 0 -> ʹ��
output INT1 ;			//DSP�ж�����


output SCR1;
output SCR2;
output SCR3;
output SCR4;
output SCR5;
output SCR6;


input CLK;
input ctrl;
input ctrl1;
input YULIU;
input Q2_ON ;			//��俪�������ź�,0 -> ���������
input BP_ON ;			//0 -> ��Ҫת��·
input WG1 ;				//0 -> �¶ȹ���
input XA0  ;		//XA��ַ����
input XA1  ;		//XA��ַ����
input XA2  ;		//XA��ַ����
input XA3  ;		//XA��ַ����
input XRDn;				//DSP��ʹ��
input XWE0n;			//DSPдʹ��
input XZCS7n ;			//DSPƬѡ
input SO4 ;				//�ű�A�Ƿ����, 0 -> ����
input SO5 ;				//---
input SO6 ;				//---

//inout
inout XD0 	;	//��������
inout XD1 	;	//��������
inout XD2 	;	//��������inout XD0 		//��������
inout XD3 	;	//��������
inout XD4 	;	//��������
inout XD5 	;//��������
inout XD6 	;	//��������
inout XD7 	;	//��������

reg ENABLE;
reg SCR1 = 0;
reg SCR2 = 0;
reg SCR3 = 0;
reg SCR4 = 0;
reg SCR5 = 0;
reg SCR6 = 0;

reg [23:0] BUFFER_SCR;
reg [3:0] STATUS;
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
	  ENABLE = 1'b1;
	  BUFFER_SCR = 24'd1050000;
     STATUS = 1'b0;
end




	always @ (posedge CLK )
	if(ctrl )
	begin
	BUFFER_SCR <= 24'd0;
	end
	else if(STATUS == 1'b1) 
	begin
	BUFFER_SCR <= BUFFER_SCR+1'b1;	  //ENABLE <= 1'b1;  
	end

	  
always @ (posedge CLK )
if(ctrl1 == 0)
begin
begin
		if(BUFFER_SCR == 24'd0)
		begin
		STATUS = 1'b1;
		ENABLE = 1'b0;
		end

		if(BUFFER_SCR == 24'd10000)
		begin
		SCR1<=1'b1;
		SCR6<=1'b1;
		end

      else if(BUFFER_SCR == 24'd20000)
		begin
		SCR1<=1'b0;SCR6<=1'b0;
      end
		
		else if(BUFFER_SCR == 24'd176667)
		begin
		SCR1<=1'b1;
		SCR2<=1'b1;
		end

		else if(BUFFER_SCR == 24'd186667)
		begin		
		SCR1<=1'b0;
		SCR2<=1'b0;
		end

		else if(BUFFER_SCR == 24'd343333)
		begin
		SCR2<=1'b1;
		SCR3<=1'b1;
		end

		else if(BUFFER_SCR == 24'd353333)
		begin
		SCR2<=1'b0;
		SCR3<=1'b0;
		end
		
		else if(BUFFER_SCR == 24'd510000)
		begin
		SCR3<=1'b1;
		SCR4<=1'b1;
		end

		else if(BUFFER_SCR == 24'd520000)
		begin
		SCR3<=1'b0;
		SCR4<=1'b0;
		end
		
		else if(BUFFER_SCR == 24'd676667)
		begin
		SCR4<=1'b1;
		SCR5<=1'b1;
		end

		else if(BUFFER_SCR == 24'd686667)
		begin
		SCR4<=1'b0;
		SCR5<=1'b0;
		end
		
		else if(BUFFER_SCR == 24'd843333)
		begin
		SCR5<=1'b1;
		SCR6<=1'b1;
		end

		else if(BUFFER_SCR == 24'd853333)
		begin
		SCR5<=1'b0;
		SCR6<=1'b0;		
		end	
		
		else if(BUFFER_SCR == 24'd1010000)
		begin
		SCR1<=1'b1;
		SCR6<=1'b1;
		end	
		
		else if(BUFFER_SCR == 24'd1020000)
		begin
		SCR1<=1'b0;
		SCR6<=1'b0;
		end
		
		else if(BUFFER_SCR >= 24'd1020000)
		begin
		SCR1<=1'b0;
		SCR2<=1'b0;
		SCR3<=1'b0;
		SCR4<=1'b0;
		SCR5<=1'b0;
		SCR6<=1'b0;
		ENABLE = 1'b1;
		STATUS = 1'b0;
		end	
		
end  
end


endmodule 