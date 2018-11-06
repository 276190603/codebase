//对地址进行宏定义
`define FPGAADD1 4'h0	//各位表示如下：0:驱动输出使能,1:逆变器吸合,2:整流驶入跳闸,DSP输出
`define FPGAADD2 4'h1	//各位表示如下：0:整流开关状态,1:逆变器运行状态,2:需要转旁路状态,3:温度过高状态
								//4-6:三相桥臂错误状态,DSP输入

module FpgaProA_1(
// input
input YULIU ,  		//开关检测，0 -> 整流开关合上
input Q2_ON ,			//逆变开关输入信号,0 -> 逆变器工作
input BP_ON ,			//0 -> 需要转旁路
input WG1 ,				//0 -> 温度过高
input CLK0 ,			//CLK0
input CLK4 ,			//CLK4
input [6:1] PWM  ,	//PWM输入
input [3:0] XA  ,		//XA地址总线
input XRDn,				//DSP读使能
input XWE0n,			//DSP写使能
input XZCS7n ,			//DSP片选
input SO4 ,				//桥臂A是否故障, 1 -> 故障
input SO5 ,				//---
input SO6 ,				//---
// output
output TRIP1 ,			//1 -> 整流输入开关跳闸
output TRIP2 ,			//0 -> 逆变输出接触器吸合
output [6:1] DR  ,	//输出驱动
output ENABLE ,		//输出驱动使能, 0 -> 使能
output INT1 ,			//DSP中断输入
//inout
inout [7:0] XD 		//数据总线

);

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
reg [6:1] Pwm_reg;				//保存当前时刻的PWM信号
reg [6:1] PwmLast_reg;			//保存上一时刻的PWM信号
reg [15:0] EnCnt_reg;			//当对角IGBT开通时间过长时，会自动清除EnableFlag_reg,此作为计数器使用

//Initial块理论上是不可综合的语句，但是现在大部分FPGA都支持
//也包括Quatus
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

//来自DSP之外的输入信号，全部定义为高电平有效
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

//每次信号改变，产生一定脉冲宽度(10us)的中断脉冲
assign Signal[6:0] = {SO6,SO5,SO4,WG1,BP_ON,Q2_ON,YULIU};	//输入信号整合
always@(posedge CLK0)
begin
	SignalDelay[6:0] <= Signal[6:0];		//输入信号的延迟
end
assign SignalPulse[6:0] = Signal[6:0]^SignalDelay[6:0];	//每次信号变化对应位产生一个脉冲
assign SignalPulseTotal =| SignalPulse[6:0];					//将上面脉冲按位或到一条信号线上
always@(posedge CLK0)
begin
	if(SignalPulseTotal)
	begin
			INT1_reg <= 1'b1;
			IntCnt <= IntCnt+16'd1;	//计数值不为零表示开始产生中断脉冲
	end
	else if(IntCnt != 16'd0)
	begin
			IntCnt <= IntCnt + 16'd1;
			INT1_reg <= 1'b0;			//状态复位
	end
	else if(IntCnt >= 16'd500)		//认为时钟频率为50Mhz 5
	begin
			IntCnt <= 16'd0;
			INT1_reg <= 1'b0;			//状态复位
	end
end
assign INT1 = INT1_reg;

//送到DSP之外的输出信号，全部定义为高电平有效
assign ENABLE = ~ENABLE_reg;
assign TRIP1 = TRIP1_reg;
assign TRIP2 = ~TRIP2_reg;

//PWM输出信号
assign DR[1] = EnableFlag_reg?PWM[1]:1'h0;
assign DR[2] = EnableFlag_reg?PWM[2]:1'h0;
//assign DR[3] = EnableFlag_reg?PWM[3]:1'h0;
//assign DR[4] = EnableFlag_reg?PWM[4]:1'h0;
assign DR[5] = EnableFlag_reg?PWM[3]:1'h0;
assign DR[6] = EnableFlag_reg?PWM[4]:1'h0;


//输入PWM消失一段时间后自动禁止PWM输出
always@(posedge CLK0)
begin
	Pwm_reg <= PWM;
	if(Pwm_reg==PwmLast_reg)		//和上一次PWM输出相同
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

//DSP读信号来临
always@(negedge XRDn)
begin
	if(XZCS7n == 1'b0)		//片选选中
	begin
		case (XA)
			`FPGAADD2:
			begin
				XD_reg[6:0] <= {SO6_reg,SO5_reg,SO4_reg,WG1_reg,BP_ON_reg,Q2_ON_reg,YULIU_reg};	//将状态返回DSP中
			end
			default:
			begin
//				XD_reg <= 8'bz;
			end
		endcase
	end
end
//当读信号来时,FPGA驱动数据总线
assign XD[7:0] = ((XZCS7n==1'b0)&(XRDn == 1'b0))?XD_reg[7:0]:8'hz;

//DSP写信号来临
always@(posedge XWE0n)
begin
	if(XZCS7n == 1'b0)			//片选选中
	begin
		case (XA)
			`FPGAADD1:
			begin
				{TRIP1_reg,TRIP2_reg,ENABLE_reg} <= XD[2:0];	//将使能信号送入FPGA
			end
			default:
			begin
				
			end
		endcase
	end
end

endmodule