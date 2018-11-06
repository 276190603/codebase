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
YULIU ,  		//开关检测，0 -> 整流开关合上
Q2_ON ,			//逆变开关输入信号,0 -> 逆变器工作
BP_ON ,			//0 -> 需要转旁路
WG1 ,				//0 -> 温度过高
//[3:0] XA  ,		//XA地址总线
XRDn,				//DSP读使能
XWE0n,			//DSP写使能
XZCS7n ,			//DSP片选
SO4 ,				//桥臂A是否故障, 0 -> 故障
SO5 ,				//---
SO6 ,				//---
//[3:0] XA  ,		//XA地址总线
XA0,
XA1,
XA2,
XA3,
//OUTPUT
ENABLE,
TRIP1 ,			//1 -> 整流输入开关跳闸
TRIP2 ,			//0 -> 逆变输出接触器吸合


INT1 ,			//DSP中断输入
//inout
XD0 	,	//数据总线
XD1 	,	//数据总线
XD2 	,	//数据总线
XD3 	,	//数据总线
XD4 	,	//数据总线XD0 	,	//数据总线
XD5 	,	//数据总线
XD6 	,	//数据总线
XD7 		//数据总线


);

output TRIP1 ;			//1 -> 整流输入开关跳闸
output TRIP2 ;			//0 -> 逆变输出接触器吸合
output ENABLE ;		//输出驱动使能, 0 -> 使能
output INT1 ;			//DSP中断输入


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
input Q2_ON ;			//逆变开关输入信号,0 -> 逆变器工作
input BP_ON ;			//0 -> 需要转旁路
input WG1 ;				//0 -> 温度过高
input XA0  ;		//XA地址总线
input XA1  ;		//XA地址总线
input XA2  ;		//XA地址总线
input XA3  ;		//XA地址总线
input XRDn;				//DSP读使能
input XWE0n;			//DSP写使能
input XZCS7n ;			//DSP片选
input SO4 ;				//桥臂A是否故障, 0 -> 故障
input SO5 ;				//---
input SO6 ;				//---

//inout
inout XD0 	;	//数据总线
inout XD1 	;	//数据总线
inout XD2 	;	//数据总线inout XD0 		//数据总线
inout XD3 	;	//数据总线
inout XD4 	;	//数据总线
inout XD5 	;//数据总线
inout XD6 	;	//数据总线
inout XD7 	;	//数据总线

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

reg [7:0] XD_reg;		//数据总线缓冲寄存器
//来自DSP以外的输入信号(全部和对应输入相反)
reg YULIU_reg;		//整流开关寄存器,1 -> 合上
reg Q2_ON_reg;		//逆变器开关寄存器,1 -> 工作
reg BP_ON_reg;		//需要转旁路寄存器,1 -> 需要
reg WG1_reg;		//温度过高寄存器,1 -> 温度过高
reg SO4_reg;		//桥臂A是否故障, 1 -> 故障
reg SO5_reg;		//---
reg SO6_reg;		//---

//用于触发DSP中断的中间信号定义
wire [6:0] Signal;		//将需要送给DSP的信号组成一个
reg [6:0] SignalDelay;	//输入信号的延迟
wire [6:0] SignalPulse;	//输入信号的脉冲,输入信号状态改变时，对应的位上回产生一个脉冲
wire SignalPulseTotal;	//脉冲信号或到一条信号线上
reg [15:0] IntCnt;		//用于中断脉宽调节的计数器
reg INT1_reg;				//DSP中断输入

//送到DSP以外的输出信号
reg TRIP1_reg;		//整流输入开关跳闸寄存器，1 -> 跳闸
reg TRIP2_reg;		//逆变输出接触器吸合寄存器, 1 -> 接触器吸合(和输出信号相反)
reg ENABLE_reg;	//输出驱动使能寄存器,1 -> 使能输出驱动


//PWM输出标志位及相关信号定义
reg EnableFlag_reg;				//为0时禁止输出
reg [2:1] Pwm13_reg;				//保存当前时刻的PWM1,3信号
reg [2:1] Pwm13Last_reg;		//保存上一时刻的PWM1,3信号
reg [15:0] EnCnt_reg;			//当对角IGBT开通时间过长时，会自动清除En





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