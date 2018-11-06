-- Copyright (C) 1991-2011 Altera Corporation
-- Your use of Altera Corporation's design tools, logic functions 
-- and other software and tools, and its AMPP partner logic 
-- functions, and any output files from any of the foregoing 
-- (including device programming or simulation files), and any 
-- associated documentation or information are expressly subject 
-- to the terms and conditions of the Altera Program License 
-- Subscription Agreement, Altera MegaCore Function License 
-- Agreement, or other applicable license agreement, including, 
-- without limitation, that your use is for the sole purpose of 
-- programming logic devices manufactured by Altera and sold by 
-- Altera or its authorized distributors.  Please refer to the 
-- applicable agreement for further details.

-- VENDOR "Altera"
-- PROGRAM "Quartus II 64-Bit"
-- VERSION "Version 11.0 Build 157 04/27/2011 SJ Full Version"

-- DATE "07/09/2018 09:55:11"

-- 
-- Device: Altera EP2C5T144C8 Package TQFP144
-- 

-- 
-- This VHDL file should be used for ModelSim-Altera (VHDL) only
-- 

LIBRARY CYCLONEII;
LIBRARY IEEE;
USE CYCLONEII.CYCLONEII_COMPONENTS.ALL;
USE IEEE.STD_LOGIC_1164.ALL;

ENTITY 	FpgaProA_1 IS
    PORT (
	TRIP1 : OUT std_logic;
	TRIP2 : OUT std_logic;
	DR : OUT std_logic_vector(6 DOWNTO 1);
	ENABLE : OUT std_logic;
	INT1 : OUT std_logic;
	YULIU : IN std_logic;
	Q2_ON : IN std_logic;
	BP_ON : IN std_logic;
	WG1 : IN std_logic;
	CLK0 : IN std_logic;
	CLK4 : IN std_logic;
	PWM : IN std_logic_vector(6 DOWNTO 1);
	XA : IN std_logic_vector(3 DOWNTO 0);
	XRDn : IN std_logic;
	XWE0n : IN std_logic;
	XZCS7n : IN std_logic;
	SO4 : IN std_logic;
	SO5 : IN std_logic;
	SO6 : IN std_logic;
	XD : INOUT std_logic_vector(7 DOWNTO 0)
	);
END FpgaProA_1;

-- Design Ports Information
-- XD[0]	=>  Location: PIN_92,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[1]	=>  Location: PIN_96,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[2]	=>  Location: PIN_97,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[3]	=>  Location: PIN_99,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[4]	=>  Location: PIN_100,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[5]	=>  Location: PIN_101,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[6]	=>  Location: PIN_103,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- XD[7]	=>  Location: PIN_104,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- CLK4	=>  Location: PIN_91,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- TRIP1	=>  Location: PIN_136,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- TRIP2	=>  Location: PIN_135,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- DR[1]	=>  Location: PIN_144,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- DR[2]	=>  Location: PIN_143,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- DR[3]	=>  Location: PIN_142,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- DR[4]	=>  Location: PIN_141,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- DR[5]	=>  Location: PIN_139,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- DR[6]	=>  Location: PIN_137,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- ENABLE	=>  Location: PIN_4,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- INT1	=>  Location: PIN_93,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
-- PWM[1]	=>  Location: PIN_72,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- PWM[2]	=>  Location: PIN_71,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- PWM[3]	=>  Location: PIN_70,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- PWM[4]	=>  Location: PIN_69,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XWE0n	=>  Location: PIN_87,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XA[1]	=>  Location: PIN_75,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XA[2]	=>  Location: PIN_74,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XA[3]	=>  Location: PIN_73,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XZCS7n	=>  Location: PIN_81,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XA[0]	=>  Location: PIN_80,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- CLK0	=>  Location: PIN_17,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- Q2_ON	=>  Location: PIN_113,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- YULIU	=>  Location: PIN_112,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- WG1	=>  Location: PIN_115,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- BP_ON	=>  Location: PIN_114,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- SO5	=>  Location: PIN_133,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- SO4	=>  Location: PIN_132,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- SO6	=>  Location: PIN_134,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- PWM[6]	=>  Location: PIN_65,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- PWM[5]	=>  Location: PIN_67,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
-- XRDn	=>  Location: PIN_86,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default


ARCHITECTURE structure OF FpgaProA_1 IS
SIGNAL gnd : std_logic := '0';
SIGNAL vcc : std_logic := '1';
SIGNAL unknown : std_logic := 'X';
SIGNAL devoe : std_logic := '1';
SIGNAL devclrn : std_logic := '1';
SIGNAL devpor : std_logic := '1';
SIGNAL ww_devoe : std_logic;
SIGNAL ww_devclrn : std_logic;
SIGNAL ww_devpor : std_logic;
SIGNAL ww_TRIP1 : std_logic;
SIGNAL ww_TRIP2 : std_logic;
SIGNAL ww_DR : std_logic_vector(6 DOWNTO 1);
SIGNAL ww_ENABLE : std_logic;
SIGNAL ww_INT1 : std_logic;
SIGNAL ww_YULIU : std_logic;
SIGNAL ww_Q2_ON : std_logic;
SIGNAL ww_BP_ON : std_logic;
SIGNAL ww_WG1 : std_logic;
SIGNAL ww_CLK0 : std_logic;
SIGNAL ww_CLK4 : std_logic;
SIGNAL ww_PWM : std_logic_vector(6 DOWNTO 1);
SIGNAL ww_XA : std_logic_vector(3 DOWNTO 0);
SIGNAL ww_XRDn : std_logic;
SIGNAL ww_XWE0n : std_logic;
SIGNAL ww_XZCS7n : std_logic;
SIGNAL ww_SO4 : std_logic;
SIGNAL ww_SO5 : std_logic;
SIGNAL ww_SO6 : std_logic;
SIGNAL \CLK0~clkctrl_INCLK_bus\ : std_logic_vector(3 DOWNTO 0);
SIGNAL \XWE0n~clkctrl_INCLK_bus\ : std_logic_vector(3 DOWNTO 0);
SIGNAL \EnCnt_reg[0]~16_combout\ : std_logic;
SIGNAL \EnCnt_reg[3]~22_combout\ : std_logic;
SIGNAL \EnCnt_reg[5]~26_combout\ : std_logic;
SIGNAL \EnCnt_reg[6]~28_combout\ : std_logic;
SIGNAL \EnCnt_reg[8]~32_combout\ : std_logic;
SIGNAL \EnCnt_reg[10]~36_combout\ : std_logic;
SIGNAL \EnCnt_reg[12]~40_combout\ : std_logic;
SIGNAL \Add0~1\ : std_logic;
SIGNAL \Add0~0_combout\ : std_logic;
SIGNAL \Add0~3\ : std_logic;
SIGNAL \Add0~2_combout\ : std_logic;
SIGNAL \Add0~5\ : std_logic;
SIGNAL \Add0~4_combout\ : std_logic;
SIGNAL \Add0~7\ : std_logic;
SIGNAL \Add0~6_combout\ : std_logic;
SIGNAL \Add0~9\ : std_logic;
SIGNAL \Add0~8_combout\ : std_logic;
SIGNAL \Add0~11\ : std_logic;
SIGNAL \Add0~10_combout\ : std_logic;
SIGNAL \Add0~13\ : std_logic;
SIGNAL \Add0~12_combout\ : std_logic;
SIGNAL \Add0~15\ : std_logic;
SIGNAL \Add0~14_combout\ : std_logic;
SIGNAL \Add0~17\ : std_logic;
SIGNAL \Add0~16_combout\ : std_logic;
SIGNAL \Add0~19\ : std_logic;
SIGNAL \Add0~18_combout\ : std_logic;
SIGNAL \Add0~21\ : std_logic;
SIGNAL \Add0~20_combout\ : std_logic;
SIGNAL \Add0~23\ : std_logic;
SIGNAL \Add0~22_combout\ : std_logic;
SIGNAL \Add0~25\ : std_logic;
SIGNAL \Add0~24_combout\ : std_logic;
SIGNAL \Add0~27\ : std_logic;
SIGNAL \Add0~26_combout\ : std_logic;
SIGNAL \Add0~29\ : std_logic;
SIGNAL \Add0~28_combout\ : std_logic;
SIGNAL \Add0~30_combout\ : std_logic;
SIGNAL \XD_reg[0]~0_combout\ : std_logic;
SIGNAL \EnableFlag_reg~0_combout\ : std_logic;
SIGNAL \EnableFlag_reg~1_combout\ : std_logic;
SIGNAL \EnableFlag_reg~2_combout\ : std_logic;
SIGNAL \Equal0~0_combout\ : std_logic;
SIGNAL \Equal0~1_combout\ : std_logic;
SIGNAL \Equal0~2_combout\ : std_logic;
SIGNAL \Equal0~3_combout\ : std_logic;
SIGNAL \Equal0~4_combout\ : std_logic;
SIGNAL \IntCnt~0_combout\ : std_logic;
SIGNAL \IntCnt[14]~1_combout\ : std_logic;
SIGNAL \IntCnt[15]~2_combout\ : std_logic;
SIGNAL \XWE0n~combout\ : std_logic;
SIGNAL \PwmLast_reg[5]~feeder_combout\ : std_logic;
SIGNAL \XD[0]~0\ : std_logic;
SIGNAL \XD[1]~1\ : std_logic;
SIGNAL \XD[2]~2\ : std_logic;
SIGNAL \XRDn~combout\ : std_logic;
SIGNAL \YULIU~combout\ : std_logic;
SIGNAL \XD_reg[0]~2_combout\ : std_logic;
SIGNAL \XD_reg[0]~1_combout\ : std_logic;
SIGNAL \XZCS7n~combout\ : std_logic;
SIGNAL \XD~16_combout\ : std_logic;
SIGNAL \Q2_ON~combout\ : std_logic;
SIGNAL \XD_reg[1]~3_combout\ : std_logic;
SIGNAL \BP_ON~combout\ : std_logic;
SIGNAL \XD_reg[2]~4_combout\ : std_logic;
SIGNAL \WG1~combout\ : std_logic;
SIGNAL \XD_reg[3]~5_combout\ : std_logic;
SIGNAL \SO4~combout\ : std_logic;
SIGNAL \XD_reg[4]~feeder_combout\ : std_logic;
SIGNAL \SO5~combout\ : std_logic;
SIGNAL \XD_reg[5]~feeder_combout\ : std_logic;
SIGNAL \SO6~combout\ : std_logic;
SIGNAL \XWE0n~clk_delay_ctrl_clkout\ : std_logic;
SIGNAL \XWE0n~clkctrl_outclk\ : std_logic;
SIGNAL \TRIP1_reg~feeder_combout\ : std_logic;
SIGNAL \TRIP1_reg~0_combout\ : std_logic;
SIGNAL \TRIP1_reg~regout\ : std_logic;
SIGNAL \TRIP2_reg~feeder_combout\ : std_logic;
SIGNAL \TRIP2_reg~regout\ : std_logic;
SIGNAL \EnCnt_reg[0]~17\ : std_logic;
SIGNAL \EnCnt_reg[1]~18_combout\ : std_logic;
SIGNAL \Pwm_reg[1]~feeder_combout\ : std_logic;
SIGNAL \PwmLast_reg[1]~feeder_combout\ : std_logic;
SIGNAL \Equal1~0_combout\ : std_logic;
SIGNAL \Equal1~2_combout\ : std_logic;
SIGNAL \Pwm_reg[3]~feeder_combout\ : std_logic;
SIGNAL \PwmLast_reg[3]~feeder_combout\ : std_logic;
SIGNAL \Equal1~1_combout\ : std_logic;
SIGNAL \Equal1~3_combout\ : std_logic;
SIGNAL \EnCnt_reg[1]~19\ : std_logic;
SIGNAL \EnCnt_reg[2]~20_combout\ : std_logic;
SIGNAL \EnCnt_reg[2]~21\ : std_logic;
SIGNAL \EnCnt_reg[3]~23\ : std_logic;
SIGNAL \EnCnt_reg[4]~24_combout\ : std_logic;
SIGNAL \EnCnt_reg[4]~25\ : std_logic;
SIGNAL \EnCnt_reg[5]~27\ : std_logic;
SIGNAL \EnCnt_reg[6]~29\ : std_logic;
SIGNAL \EnCnt_reg[7]~30_combout\ : std_logic;
SIGNAL \EnCnt_reg[7]~31\ : std_logic;
SIGNAL \EnCnt_reg[8]~33\ : std_logic;
SIGNAL \EnCnt_reg[9]~34_combout\ : std_logic;
SIGNAL \EnCnt_reg[9]~35\ : std_logic;
SIGNAL \EnCnt_reg[10]~37\ : std_logic;
SIGNAL \EnCnt_reg[11]~38_combout\ : std_logic;
SIGNAL \EnCnt_reg[11]~39\ : std_logic;
SIGNAL \EnCnt_reg[12]~41\ : std_logic;
SIGNAL \EnCnt_reg[13]~42_combout\ : std_logic;
SIGNAL \EnCnt_reg[13]~43\ : std_logic;
SIGNAL \EnCnt_reg[14]~44_combout\ : std_logic;
SIGNAL \EnCnt_reg[14]~45\ : std_logic;
SIGNAL \EnCnt_reg[15]~46_combout\ : std_logic;
SIGNAL \EnableFlag_reg~3_combout\ : std_logic;
SIGNAL \EnableFlag_reg~regout\ : std_logic;
SIGNAL \DR~0_combout\ : std_logic;
SIGNAL \DR~1_combout\ : std_logic;
SIGNAL \DR~2_combout\ : std_logic;
SIGNAL \DR~3_combout\ : std_logic;
SIGNAL \ENABLE_reg~feeder_combout\ : std_logic;
SIGNAL \ENABLE_reg~regout\ : std_logic;
SIGNAL \CLK0~combout\ : std_logic;
SIGNAL \CLK0~clkctrl_outclk\ : std_logic;
SIGNAL \SignalDelay[0]~feeder_combout\ : std_logic;
SIGNAL \INT1_reg~0_combout\ : std_logic;
SIGNAL \INT1_reg~1_combout\ : std_logic;
SIGNAL \INT1_reg~2_combout\ : std_logic;
SIGNAL \SignalDelay[4]~feeder_combout\ : std_logic;
SIGNAL \INT1_reg~3_combout\ : std_logic;
SIGNAL \INT1_reg~4_combout\ : std_logic;
SIGNAL \INT1_reg~5_combout\ : std_logic;
SIGNAL \INT1_reg~regout\ : std_logic;
SIGNAL \PWM~combout\ : std_logic_vector(6 DOWNTO 1);
SIGNAL \XA~combout\ : std_logic_vector(3 DOWNTO 0);
SIGNAL EnCnt_reg : std_logic_vector(15 DOWNTO 0);
SIGNAL IntCnt : std_logic_vector(15 DOWNTO 0);
SIGNAL PwmLast_reg : std_logic_vector(6 DOWNTO 1);
SIGNAL Pwm_reg : std_logic_vector(6 DOWNTO 1);
SIGNAL SignalDelay : std_logic_vector(6 DOWNTO 0);
SIGNAL XD_reg : std_logic_vector(7 DOWNTO 0);
SIGNAL \ALT_INV_TRIP2_reg~regout\ : std_logic;
SIGNAL \ALT_INV_ENABLE_reg~regout\ : std_logic;
SIGNAL \ALT_INV_XD~16_combout\ : std_logic;
SIGNAL \ALT_INV_XRDn~combout\ : std_logic;

BEGIN

TRIP1 <= ww_TRIP1;
TRIP2 <= ww_TRIP2;
DR <= ww_DR;
ENABLE <= ww_ENABLE;
INT1 <= ww_INT1;
ww_YULIU <= YULIU;
ww_Q2_ON <= Q2_ON;
ww_BP_ON <= BP_ON;
ww_WG1 <= WG1;
ww_CLK0 <= CLK0;
ww_CLK4 <= CLK4;
ww_PWM <= PWM;
ww_XA <= XA;
ww_XRDn <= XRDn;
ww_XWE0n <= XWE0n;
ww_XZCS7n <= XZCS7n;
ww_SO4 <= SO4;
ww_SO5 <= SO5;
ww_SO6 <= SO6;
ww_devoe <= devoe;
ww_devclrn <= devclrn;
ww_devpor <= devpor;

\CLK0~clkctrl_INCLK_bus\ <= (gnd & gnd & gnd & \CLK0~combout\);

\XWE0n~clkctrl_INCLK_bus\ <= (gnd & gnd & gnd & \XWE0n~clk_delay_ctrl_clkout\);
\ALT_INV_TRIP2_reg~regout\ <= NOT \TRIP2_reg~regout\;
\ALT_INV_ENABLE_reg~regout\ <= NOT \ENABLE_reg~regout\;
\ALT_INV_XD~16_combout\ <= NOT \XD~16_combout\;
\ALT_INV_XRDn~combout\ <= NOT \XRDn~combout\;

-- Location: LCFF_X21_Y9_N21
\EnCnt_reg[10]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[10]~36_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(10));

-- Location: LCFF_X21_Y9_N25
\EnCnt_reg[12]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[12]~40_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(12));

-- Location: LCFF_X21_Y9_N11
\EnCnt_reg[5]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[5]~26_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(5));

-- Location: LCFF_X21_Y9_N13
\EnCnt_reg[6]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[6]~28_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(6));

-- Location: LCFF_X21_Y9_N17
\EnCnt_reg[8]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[8]~32_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(8));

-- Location: LCFF_X21_Y9_N7
\EnCnt_reg[3]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[3]~22_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(3));

-- Location: LCFF_X21_Y9_N1
\EnCnt_reg[0]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[0]~16_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(0));

-- Location: LCCOMB_X21_Y9_N0
\EnCnt_reg[0]~16\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[0]~16_combout\ = (EnCnt_reg(0) & (\EnableFlag_reg~regout\ $ (VCC))) # (!EnCnt_reg(0) & (\EnableFlag_reg~regout\ & VCC))
-- \EnCnt_reg[0]~17\ = CARRY((EnCnt_reg(0) & \EnableFlag_reg~regout\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0110011010001000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(0),
	datab => \EnableFlag_reg~regout\,
	datad => VCC,
	combout => \EnCnt_reg[0]~16_combout\,
	cout => \EnCnt_reg[0]~17\);

-- Location: LCCOMB_X21_Y9_N6
\EnCnt_reg[3]~22\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[3]~22_combout\ = (EnCnt_reg(3) & (!\EnCnt_reg[2]~21\)) # (!EnCnt_reg(3) & ((\EnCnt_reg[2]~21\) # (GND)))
-- \EnCnt_reg[3]~23\ = CARRY((!\EnCnt_reg[2]~21\) # (!EnCnt_reg(3)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0101101001011111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(3),
	datad => VCC,
	cin => \EnCnt_reg[2]~21\,
	combout => \EnCnt_reg[3]~22_combout\,
	cout => \EnCnt_reg[3]~23\);

-- Location: LCCOMB_X21_Y9_N10
\EnCnt_reg[5]~26\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[5]~26_combout\ = (EnCnt_reg(5) & (!\EnCnt_reg[4]~25\)) # (!EnCnt_reg(5) & ((\EnCnt_reg[4]~25\) # (GND)))
-- \EnCnt_reg[5]~27\ = CARRY((!\EnCnt_reg[4]~25\) # (!EnCnt_reg(5)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0101101001011111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(5),
	datad => VCC,
	cin => \EnCnt_reg[4]~25\,
	combout => \EnCnt_reg[5]~26_combout\,
	cout => \EnCnt_reg[5]~27\);

-- Location: LCCOMB_X21_Y9_N12
\EnCnt_reg[6]~28\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[6]~28_combout\ = (EnCnt_reg(6) & (\EnCnt_reg[5]~27\ $ (GND))) # (!EnCnt_reg(6) & (!\EnCnt_reg[5]~27\ & VCC))
-- \EnCnt_reg[6]~29\ = CARRY((EnCnt_reg(6) & !\EnCnt_reg[5]~27\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(6),
	datad => VCC,
	cin => \EnCnt_reg[5]~27\,
	combout => \EnCnt_reg[6]~28_combout\,
	cout => \EnCnt_reg[6]~29\);

-- Location: LCCOMB_X21_Y9_N16
\EnCnt_reg[8]~32\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[8]~32_combout\ = (EnCnt_reg(8) & (\EnCnt_reg[7]~31\ $ (GND))) # (!EnCnt_reg(8) & (!\EnCnt_reg[7]~31\ & VCC))
-- \EnCnt_reg[8]~33\ = CARRY((EnCnt_reg(8) & !\EnCnt_reg[7]~31\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(8),
	datad => VCC,
	cin => \EnCnt_reg[7]~31\,
	combout => \EnCnt_reg[8]~32_combout\,
	cout => \EnCnt_reg[8]~33\);

-- Location: LCCOMB_X21_Y9_N20
\EnCnt_reg[10]~36\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[10]~36_combout\ = (EnCnt_reg(10) & (\EnCnt_reg[9]~35\ $ (GND))) # (!EnCnt_reg(10) & (!\EnCnt_reg[9]~35\ & VCC))
-- \EnCnt_reg[10]~37\ = CARRY((EnCnt_reg(10) & !\EnCnt_reg[9]~35\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(10),
	datad => VCC,
	cin => \EnCnt_reg[9]~35\,
	combout => \EnCnt_reg[10]~36_combout\,
	cout => \EnCnt_reg[10]~37\);

-- Location: LCCOMB_X21_Y9_N24
\EnCnt_reg[12]~40\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[12]~40_combout\ = (EnCnt_reg(12) & (\EnCnt_reg[11]~39\ $ (GND))) # (!EnCnt_reg(12) & (!\EnCnt_reg[11]~39\ & VCC))
-- \EnCnt_reg[12]~41\ = CARRY((EnCnt_reg(12) & !\EnCnt_reg[11]~39\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(12),
	datad => VCC,
	cin => \EnCnt_reg[11]~39\,
	combout => \EnCnt_reg[12]~40_combout\,
	cout => \EnCnt_reg[12]~41\);

-- Location: LCCOMB_X18_Y13_N0
\Add0~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~0_combout\ = IntCnt(0) $ (VCC)
-- \Add0~1\ = CARRY(IntCnt(0))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011001111001100",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(0),
	datad => VCC,
	combout => \Add0~0_combout\,
	cout => \Add0~1\);

-- Location: LCCOMB_X18_Y13_N2
\Add0~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~2_combout\ = (IntCnt(1) & (!\Add0~1\)) # (!IntCnt(1) & ((\Add0~1\) # (GND)))
-- \Add0~3\ = CARRY((!\Add0~1\) # (!IntCnt(1)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(1),
	datad => VCC,
	cin => \Add0~1\,
	combout => \Add0~2_combout\,
	cout => \Add0~3\);

-- Location: LCCOMB_X18_Y13_N4
\Add0~4\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~4_combout\ = (IntCnt(2) & (\Add0~3\ $ (GND))) # (!IntCnt(2) & (!\Add0~3\ & VCC))
-- \Add0~5\ = CARRY((IntCnt(2) & !\Add0~3\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(2),
	datad => VCC,
	cin => \Add0~3\,
	combout => \Add0~4_combout\,
	cout => \Add0~5\);

-- Location: LCCOMB_X18_Y13_N6
\Add0~6\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~6_combout\ = (IntCnt(3) & (!\Add0~5\)) # (!IntCnt(3) & ((\Add0~5\) # (GND)))
-- \Add0~7\ = CARRY((!\Add0~5\) # (!IntCnt(3)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0101101001011111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(3),
	datad => VCC,
	cin => \Add0~5\,
	combout => \Add0~6_combout\,
	cout => \Add0~7\);

-- Location: LCCOMB_X18_Y13_N8
\Add0~8\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~8_combout\ = (IntCnt(4) & (\Add0~7\ $ (GND))) # (!IntCnt(4) & (!\Add0~7\ & VCC))
-- \Add0~9\ = CARRY((IntCnt(4) & !\Add0~7\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(4),
	datad => VCC,
	cin => \Add0~7\,
	combout => \Add0~8_combout\,
	cout => \Add0~9\);

-- Location: LCCOMB_X18_Y13_N10
\Add0~10\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~10_combout\ = (IntCnt(5) & (!\Add0~9\)) # (!IntCnt(5) & ((\Add0~9\) # (GND)))
-- \Add0~11\ = CARRY((!\Add0~9\) # (!IntCnt(5)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0101101001011111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(5),
	datad => VCC,
	cin => \Add0~9\,
	combout => \Add0~10_combout\,
	cout => \Add0~11\);

-- Location: LCCOMB_X18_Y13_N12
\Add0~12\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~12_combout\ = (IntCnt(6) & (\Add0~11\ $ (GND))) # (!IntCnt(6) & (!\Add0~11\ & VCC))
-- \Add0~13\ = CARRY((IntCnt(6) & !\Add0~11\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(6),
	datad => VCC,
	cin => \Add0~11\,
	combout => \Add0~12_combout\,
	cout => \Add0~13\);

-- Location: LCCOMB_X18_Y13_N14
\Add0~14\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~14_combout\ = (IntCnt(7) & (!\Add0~13\)) # (!IntCnt(7) & ((\Add0~13\) # (GND)))
-- \Add0~15\ = CARRY((!\Add0~13\) # (!IntCnt(7)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(7),
	datad => VCC,
	cin => \Add0~13\,
	combout => \Add0~14_combout\,
	cout => \Add0~15\);

-- Location: LCCOMB_X18_Y13_N16
\Add0~16\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~16_combout\ = (IntCnt(8) & (\Add0~15\ $ (GND))) # (!IntCnt(8) & (!\Add0~15\ & VCC))
-- \Add0~17\ = CARRY((IntCnt(8) & !\Add0~15\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(8),
	datad => VCC,
	cin => \Add0~15\,
	combout => \Add0~16_combout\,
	cout => \Add0~17\);

-- Location: LCCOMB_X18_Y13_N18
\Add0~18\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~18_combout\ = (IntCnt(9) & (!\Add0~17\)) # (!IntCnt(9) & ((\Add0~17\) # (GND)))
-- \Add0~19\ = CARRY((!\Add0~17\) # (!IntCnt(9)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(9),
	datad => VCC,
	cin => \Add0~17\,
	combout => \Add0~18_combout\,
	cout => \Add0~19\);

-- Location: LCCOMB_X18_Y13_N20
\Add0~20\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~20_combout\ = (IntCnt(10) & (\Add0~19\ $ (GND))) # (!IntCnt(10) & (!\Add0~19\ & VCC))
-- \Add0~21\ = CARRY((IntCnt(10) & !\Add0~19\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(10),
	datad => VCC,
	cin => \Add0~19\,
	combout => \Add0~20_combout\,
	cout => \Add0~21\);

-- Location: LCCOMB_X18_Y13_N22
\Add0~22\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~22_combout\ = (IntCnt(11) & (!\Add0~21\)) # (!IntCnt(11) & ((\Add0~21\) # (GND)))
-- \Add0~23\ = CARRY((!\Add0~21\) # (!IntCnt(11)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(11),
	datad => VCC,
	cin => \Add0~21\,
	combout => \Add0~22_combout\,
	cout => \Add0~23\);

-- Location: LCCOMB_X18_Y13_N24
\Add0~24\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~24_combout\ = (IntCnt(12) & (\Add0~23\ $ (GND))) # (!IntCnt(12) & (!\Add0~23\ & VCC))
-- \Add0~25\ = CARRY((IntCnt(12) & !\Add0~23\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010010100001010",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(12),
	datad => VCC,
	cin => \Add0~23\,
	combout => \Add0~24_combout\,
	cout => \Add0~25\);

-- Location: LCCOMB_X18_Y13_N26
\Add0~26\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~26_combout\ = (IntCnt(13) & (!\Add0~25\)) # (!IntCnt(13) & ((\Add0~25\) # (GND)))
-- \Add0~27\ = CARRY((!\Add0~25\) # (!IntCnt(13)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(13),
	datad => VCC,
	cin => \Add0~25\,
	combout => \Add0~26_combout\,
	cout => \Add0~27\);

-- Location: LCCOMB_X18_Y13_N28
\Add0~28\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~28_combout\ = (IntCnt(14) & (\Add0~27\ $ (GND))) # (!IntCnt(14) & (!\Add0~27\ & VCC))
-- \Add0~29\ = CARRY((IntCnt(14) & !\Add0~27\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => IntCnt(14),
	datad => VCC,
	cin => \Add0~27\,
	combout => \Add0~28_combout\,
	cout => \Add0~29\);

-- Location: LCCOMB_X18_Y13_N30
\Add0~30\ : cycloneii_lcell_comb
-- Equation(s):
-- \Add0~30_combout\ = \Add0~29\ $ (IntCnt(15))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000111111110000",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datad => IntCnt(15),
	cin => \Add0~29\,
	combout => \Add0~30_combout\);

-- Location: LCCOMB_X25_Y10_N24
\XD_reg[0]~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[0]~0_combout\ = (!\XA~combout\(2) & (!\XA~combout\(1) & (!\XA~combout\(3) & !\XZCS7n~combout\)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000000000001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \XA~combout\(2),
	datab => \XA~combout\(1),
	datac => \XA~combout\(3),
	datad => \XZCS7n~combout\,
	combout => \XD_reg[0]~0_combout\);

-- Location: LCCOMB_X22_Y9_N28
\EnableFlag_reg~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnableFlag_reg~0_combout\ = (!EnCnt_reg(10) & (!EnCnt_reg(12) & (!EnCnt_reg(13) & !EnCnt_reg(11))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000000000001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(10),
	datab => EnCnt_reg(12),
	datac => EnCnt_reg(13),
	datad => EnCnt_reg(11),
	combout => \EnableFlag_reg~0_combout\);

-- Location: LCCOMB_X22_Y9_N20
\EnableFlag_reg~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnableFlag_reg~1_combout\ = (!EnCnt_reg(7) & (((!EnCnt_reg(4) & !EnCnt_reg(5))) # (!EnCnt_reg(6))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000010100010101",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(7),
	datab => EnCnt_reg(4),
	datac => EnCnt_reg(6),
	datad => EnCnt_reg(5),
	combout => \EnableFlag_reg~1_combout\);

-- Location: LCCOMB_X22_Y9_N6
\EnableFlag_reg~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnableFlag_reg~2_combout\ = (\EnableFlag_reg~0_combout\ & (((\EnableFlag_reg~1_combout\) # (!EnCnt_reg(8))) # (!EnCnt_reg(9))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111011100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => EnCnt_reg(9),
	datab => EnCnt_reg(8),
	datac => \EnableFlag_reg~1_combout\,
	datad => \EnableFlag_reg~0_combout\,
	combout => \EnableFlag_reg~2_combout\);

-- Location: LCFF_X22_Y9_N11
\PwmLast_reg[5]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \PwmLast_reg[5]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => PwmLast_reg(5));

-- Location: LCFF_X19_Y13_N19
\IntCnt[0]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \IntCnt~0_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(0));

-- Location: LCFF_X18_Y13_N3
\IntCnt[1]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~2_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(1));

-- Location: LCFF_X18_Y13_N5
\IntCnt[2]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~4_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(2));

-- Location: LCFF_X18_Y13_N7
\IntCnt[3]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~6_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(3));

-- Location: LCCOMB_X19_Y13_N12
\Equal0~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal0~0_combout\ = (IntCnt(1)) # ((IntCnt(2)) # ((IntCnt(3)) # (IntCnt(0))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111111111110",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(1),
	datab => IntCnt(2),
	datac => IntCnt(3),
	datad => IntCnt(0),
	combout => \Equal0~0_combout\);

-- Location: LCFF_X18_Y13_N9
\IntCnt[4]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~8_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(4));

-- Location: LCFF_X18_Y13_N11
\IntCnt[5]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~10_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(5));

-- Location: LCFF_X18_Y13_N13
\IntCnt[6]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~12_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(6));

-- Location: LCFF_X18_Y13_N15
\IntCnt[7]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~14_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(7));

-- Location: LCCOMB_X19_Y13_N22
\Equal0~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal0~1_combout\ = (IntCnt(7)) # ((IntCnt(4)) # ((IntCnt(5)) # (IntCnt(6))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111111111110",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(7),
	datab => IntCnt(4),
	datac => IntCnt(5),
	datad => IntCnt(6),
	combout => \Equal0~1_combout\);

-- Location: LCFF_X18_Y13_N17
\IntCnt[8]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(8));

-- Location: LCFF_X18_Y13_N19
\IntCnt[9]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~18_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(9));

-- Location: LCFF_X18_Y13_N21
\IntCnt[10]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~20_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(10));

-- Location: LCFF_X18_Y13_N23
\IntCnt[11]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~22_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(11));

-- Location: LCCOMB_X19_Y13_N8
\Equal0~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal0~2_combout\ = (IntCnt(11)) # ((IntCnt(9)) # ((IntCnt(10)) # (IntCnt(8))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111111111110",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(11),
	datab => IntCnt(9),
	datac => IntCnt(10),
	datad => IntCnt(8),
	combout => \Equal0~2_combout\);

-- Location: LCFF_X18_Y13_N25
\IntCnt[12]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~24_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(12));

-- Location: LCFF_X18_Y13_N27
\IntCnt[13]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Add0~26_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(13));

-- Location: LCFF_X19_Y13_N11
\IntCnt[14]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \IntCnt[14]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(14));

-- Location: LCFF_X19_Y13_N21
\IntCnt[15]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \IntCnt[15]~2_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => IntCnt(15));

-- Location: LCCOMB_X19_Y13_N14
\Equal0~3\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal0~3_combout\ = (IntCnt(14)) # ((IntCnt(13)) # ((IntCnt(15)) # (IntCnt(12))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111111111110",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => IntCnt(14),
	datab => IntCnt(13),
	datac => IntCnt(15),
	datad => IntCnt(12),
	combout => \Equal0~3_combout\);

-- Location: LCCOMB_X19_Y13_N16
\Equal0~4\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal0~4_combout\ = (\Equal0~0_combout\) # ((\Equal0~2_combout\) # ((\Equal0~3_combout\) # (\Equal0~1_combout\)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111111111110",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \Equal0~0_combout\,
	datab => \Equal0~2_combout\,
	datac => \Equal0~3_combout\,
	datad => \Equal0~1_combout\,
	combout => \Equal0~4_combout\);

-- Location: LCCOMB_X19_Y13_N18
\IntCnt~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \IntCnt~0_combout\ = (\Add0~0_combout\ & ((\Equal0~4_combout\) # ((!\INT1_reg~4_combout\) # (!\INT1_reg~2_combout\))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1011000011110000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \Equal0~4_combout\,
	datab => \INT1_reg~2_combout\,
	datac => \Add0~0_combout\,
	datad => \INT1_reg~4_combout\,
	combout => \IntCnt~0_combout\);

-- Location: LCCOMB_X19_Y13_N10
\IntCnt[14]~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \IntCnt[14]~1_combout\ = (\Add0~28_combout\ & (((\Equal0~4_combout\) # (!\INT1_reg~2_combout\)) # (!\INT1_reg~4_combout\)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111011100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \INT1_reg~4_combout\,
	datab => \INT1_reg~2_combout\,
	datac => \Equal0~4_combout\,
	datad => \Add0~28_combout\,
	combout => \IntCnt[14]~1_combout\);

-- Location: LCCOMB_X19_Y13_N20
\IntCnt[15]~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \IntCnt[15]~2_combout\ = (\Add0~30_combout\ & ((\Equal0~4_combout\) # ((!\INT1_reg~4_combout\) # (!\INT1_reg~2_combout\))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1011000011110000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \Equal0~4_combout\,
	datab => \INT1_reg~2_combout\,
	datac => \Add0~30_combout\,
	datad => \INT1_reg~4_combout\,
	combout => \IntCnt[15]~2_combout\);

-- Location: PIN_87,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XWE0n~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XWE0n,
	combout => \XWE0n~combout\);

-- Location: PIN_75,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XA[1]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XA(1),
	combout => \XA~combout\(1));

-- Location: PIN_74,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XA[2]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XA(2),
	combout => \XA~combout\(2));

-- Location: PIN_73,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XA[3]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XA(3),
	combout => \XA~combout\(3));

-- Location: LCCOMB_X22_Y9_N10
\PwmLast_reg[5]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \PwmLast_reg[5]~feeder_combout\ = Pwm_reg(5)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => Pwm_reg(5),
	combout => \PwmLast_reg[5]~feeder_combout\);

-- Location: PIN_92,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[0]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(0),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(0),
	combout => \XD[0]~0\);

-- Location: PIN_96,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[1]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(1),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(1),
	combout => \XD[1]~1\);

-- Location: PIN_97,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[2]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(2),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(2),
	combout => \XD[2]~2\);

-- Location: PIN_86,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XRDn~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XRDn,
	combout => \XRDn~combout\);

-- Location: PIN_112,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\YULIU~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_YULIU,
	combout => \YULIU~combout\);

-- Location: LCCOMB_X25_Y10_N6
\XD_reg[0]~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[0]~2_combout\ = !\YULIU~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000011111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \YULIU~combout\,
	combout => \XD_reg[0]~2_combout\);

-- Location: PIN_80,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XA[0]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XA(0),
	combout => \XA~combout\(0));

-- Location: LCCOMB_X25_Y10_N30
\XD_reg[0]~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[0]~1_combout\ = (\XD_reg[0]~0_combout\ & \XA~combout\(0))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010000010100000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \XD_reg[0]~0_combout\,
	datac => \XA~combout\(0),
	combout => \XD_reg[0]~1_combout\);

-- Location: LCFF_X25_Y10_N7
\XD_reg[0]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	datain => \XD_reg[0]~2_combout\,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(0));

-- Location: PIN_81,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\XZCS7n~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_XZCS7n,
	combout => \XZCS7n~combout\);

-- Location: LCCOMB_X25_Y10_N16
\XD~16\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD~16_combout\ = (\XRDn~combout\) # (\XZCS7n~combout\)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111110101010",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \XRDn~combout\,
	datad => \XZCS7n~combout\,
	combout => \XD~16_combout\);

-- Location: PIN_113,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\Q2_ON~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_Q2_ON,
	combout => \Q2_ON~combout\);

-- Location: LCCOMB_X25_Y10_N18
\XD_reg[1]~3\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[1]~3_combout\ = !\Q2_ON~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000111100001111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => \Q2_ON~combout\,
	combout => \XD_reg[1]~3_combout\);

-- Location: LCFF_X25_Y10_N19
\XD_reg[1]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	datain => \XD_reg[1]~3_combout\,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(1));

-- Location: PIN_114,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\BP_ON~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_BP_ON,
	combout => \BP_ON~combout\);

-- Location: LCCOMB_X25_Y10_N12
\XD_reg[2]~4\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[2]~4_combout\ = !\BP_ON~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000000011111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \BP_ON~combout\,
	combout => \XD_reg[2]~4_combout\);

-- Location: LCFF_X25_Y10_N13
\XD_reg[2]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	datain => \XD_reg[2]~4_combout\,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(2));

-- Location: PIN_115,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\WG1~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_WG1,
	combout => \WG1~combout\);

-- Location: LCCOMB_X25_Y10_N22
\XD_reg[3]~5\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[3]~5_combout\ = !\WG1~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000111100001111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => \WG1~combout\,
	combout => \XD_reg[3]~5_combout\);

-- Location: LCFF_X25_Y10_N23
\XD_reg[3]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	datain => \XD_reg[3]~5_combout\,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(3));

-- Location: PIN_132,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\SO4~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_SO4,
	combout => \SO4~combout\);

-- Location: LCCOMB_X25_Y10_N0
\XD_reg[4]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[4]~feeder_combout\ = \SO4~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \SO4~combout\,
	combout => \XD_reg[4]~feeder_combout\);

-- Location: LCFF_X25_Y10_N1
\XD_reg[4]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	datain => \XD_reg[4]~feeder_combout\,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(4));

-- Location: PIN_133,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\SO5~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_SO5,
	combout => \SO5~combout\);

-- Location: LCCOMB_X25_Y10_N26
\XD_reg[5]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \XD_reg[5]~feeder_combout\ = \SO5~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \SO5~combout\,
	combout => \XD_reg[5]~feeder_combout\);

-- Location: LCFF_X25_Y10_N27
\XD_reg[5]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	datain => \XD_reg[5]~feeder_combout\,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(5));

-- Location: PIN_134,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\SO6~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_SO6,
	combout => \SO6~combout\);

-- Location: LCFF_X25_Y10_N21
\XD_reg[6]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \ALT_INV_XRDn~combout\,
	sdata => \SO6~combout\,
	sload => VCC,
	ena => \XD_reg[0]~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => XD_reg(6));

-- Location: CLKDELAYCTRL_G5
\XWE0n~clk_delay_ctrl\ : cycloneii_clk_delay_ctrl
-- pragma translate_off
GENERIC MAP (
	delay_chain_mode => "none",
	use_new_style_dq_detection => "false")
-- pragma translate_on
PORT MAP (
	clk => \XWE0n~combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	clkout => \XWE0n~clk_delay_ctrl_clkout\);

-- Location: CLKCTRL_G5
\XWE0n~clkctrl\ : cycloneii_clkctrl
-- pragma translate_off
GENERIC MAP (
	clock_type => "global clock",
	ena_register_mode => "none")
-- pragma translate_on
PORT MAP (
	inclk => \XWE0n~clkctrl_INCLK_bus\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	outclk => \XWE0n~clkctrl_outclk\);

-- Location: LCCOMB_X6_Y13_N0
\TRIP1_reg~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \TRIP1_reg~feeder_combout\ = \XD[2]~2\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \XD[2]~2\,
	combout => \TRIP1_reg~feeder_combout\);

-- Location: LCCOMB_X25_Y10_N10
\TRIP1_reg~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \TRIP1_reg~0_combout\ = (\XD_reg[0]~0_combout\ & !\XA~combout\(0))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000101000001010",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \XD_reg[0]~0_combout\,
	datac => \XA~combout\(0),
	combout => \TRIP1_reg~0_combout\);

-- Location: LCFF_X6_Y13_N1
TRIP1_reg : cycloneii_lcell_ff
PORT MAP (
	clk => \XWE0n~clkctrl_outclk\,
	datain => \TRIP1_reg~feeder_combout\,
	ena => \TRIP1_reg~0_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => \TRIP1_reg~regout\);

-- Location: LCCOMB_X6_Y13_N10
\TRIP2_reg~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \TRIP2_reg~feeder_combout\ = \XD[1]~1\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \XD[1]~1\,
	combout => \TRIP2_reg~feeder_combout\);

-- Location: LCFF_X6_Y13_N11
TRIP2_reg : cycloneii_lcell_ff
PORT MAP (
	clk => \XWE0n~clkctrl_outclk\,
	datain => \TRIP2_reg~feeder_combout\,
	ena => \TRIP1_reg~0_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => \TRIP2_reg~regout\);

-- Location: PIN_72,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\PWM[1]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_PWM(1),
	combout => \PWM~combout\(1));

-- Location: LCCOMB_X21_Y9_N2
\EnCnt_reg[1]~18\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[1]~18_combout\ = (EnCnt_reg(1) & (!\EnCnt_reg[0]~17\)) # (!EnCnt_reg(1) & ((\EnCnt_reg[0]~17\) # (GND)))
-- \EnCnt_reg[1]~19\ = CARRY((!\EnCnt_reg[0]~17\) # (!EnCnt_reg(1)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(1),
	datad => VCC,
	cin => \EnCnt_reg[0]~17\,
	combout => \EnCnt_reg[1]~18_combout\,
	cout => \EnCnt_reg[1]~19\);

-- Location: LCCOMB_X22_Y9_N2
\Pwm_reg[1]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \Pwm_reg[1]~feeder_combout\ = \PWM~combout\(1)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \PWM~combout\(1),
	combout => \Pwm_reg[1]~feeder_combout\);

-- Location: LCFF_X22_Y9_N3
\Pwm_reg[1]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Pwm_reg[1]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => Pwm_reg(1));

-- Location: PIN_71,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\PWM[2]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_PWM(2),
	combout => \PWM~combout\(2));

-- Location: LCFF_X22_Y9_N17
\Pwm_reg[2]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \PWM~combout\(2),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => Pwm_reg(2));

-- Location: LCFF_X22_Y9_N27
\PwmLast_reg[2]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => Pwm_reg(2),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => PwmLast_reg(2));

-- Location: LCCOMB_X22_Y9_N22
\PwmLast_reg[1]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \PwmLast_reg[1]~feeder_combout\ = Pwm_reg(1)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => Pwm_reg(1),
	combout => \PwmLast_reg[1]~feeder_combout\);

-- Location: LCFF_X22_Y9_N23
\PwmLast_reg[1]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \PwmLast_reg[1]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => PwmLast_reg(1));

-- Location: LCCOMB_X22_Y9_N26
\Equal1~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal1~0_combout\ = (Pwm_reg(2) & (PwmLast_reg(2) & (Pwm_reg(1) $ (!PwmLast_reg(1))))) # (!Pwm_reg(2) & (!PwmLast_reg(2) & (Pwm_reg(1) $ (!PwmLast_reg(1)))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000010000100001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => Pwm_reg(2),
	datab => Pwm_reg(1),
	datac => PwmLast_reg(2),
	datad => PwmLast_reg(1),
	combout => \Equal1~0_combout\);

-- Location: PIN_65,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\PWM[6]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_PWM(6),
	combout => \PWM~combout\(6));

-- Location: LCFF_X22_Y9_N5
\Pwm_reg[6]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \PWM~combout\(6),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => Pwm_reg(6));

-- Location: LCFF_X22_Y9_N9
\PwmLast_reg[6]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => Pwm_reg(6),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => PwmLast_reg(6));

-- Location: PIN_67,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\PWM[5]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_PWM(5),
	combout => \PWM~combout\(5));

-- Location: LCFF_X22_Y9_N19
\Pwm_reg[5]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \PWM~combout\(5),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => Pwm_reg(5));

-- Location: LCCOMB_X22_Y9_N8
\Equal1~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal1~2_combout\ = (PwmLast_reg(5) & (Pwm_reg(5) & (Pwm_reg(6) $ (!PwmLast_reg(6))))) # (!PwmLast_reg(5) & (!Pwm_reg(5) & (Pwm_reg(6) $ (!PwmLast_reg(6)))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000001001000001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => PwmLast_reg(5),
	datab => Pwm_reg(6),
	datac => PwmLast_reg(6),
	datad => Pwm_reg(5),
	combout => \Equal1~2_combout\);

-- Location: PIN_70,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\PWM[3]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_PWM(3),
	combout => \PWM~combout\(3));

-- Location: LCCOMB_X22_Y9_N0
\Pwm_reg[3]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \Pwm_reg[3]~feeder_combout\ = \PWM~combout\(3)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \PWM~combout\(3),
	combout => \Pwm_reg[3]~feeder_combout\);

-- Location: LCFF_X22_Y9_N1
\Pwm_reg[3]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \Pwm_reg[3]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => Pwm_reg(3));

-- Location: PIN_69,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\PWM[4]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_PWM(4),
	combout => \PWM~combout\(4));

-- Location: LCFF_X22_Y9_N25
\Pwm_reg[4]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \PWM~combout\(4),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => Pwm_reg(4));

-- Location: LCFF_X22_Y9_N13
\PwmLast_reg[4]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => Pwm_reg(4),
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => PwmLast_reg(4));

-- Location: LCCOMB_X22_Y10_N8
\PwmLast_reg[3]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \PwmLast_reg[3]~feeder_combout\ = Pwm_reg(3)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => Pwm_reg(3),
	combout => \PwmLast_reg[3]~feeder_combout\);

-- Location: LCFF_X22_Y10_N9
\PwmLast_reg[3]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \PwmLast_reg[3]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => PwmLast_reg(3));

-- Location: LCCOMB_X22_Y9_N12
\Equal1~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal1~1_combout\ = (Pwm_reg(4) & (PwmLast_reg(4) & (Pwm_reg(3) $ (!PwmLast_reg(3))))) # (!Pwm_reg(4) & (!PwmLast_reg(4) & (Pwm_reg(3) $ (!PwmLast_reg(3)))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000010000100001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => Pwm_reg(4),
	datab => Pwm_reg(3),
	datac => PwmLast_reg(4),
	datad => PwmLast_reg(3),
	combout => \Equal1~1_combout\);

-- Location: LCCOMB_X22_Y9_N30
\Equal1~3\ : cycloneii_lcell_comb
-- Equation(s):
-- \Equal1~3_combout\ = ((!\Equal1~1_combout\) # (!\Equal1~2_combout\)) # (!\Equal1~0_combout\)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011111111111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datab => \Equal1~0_combout\,
	datac => \Equal1~2_combout\,
	datad => \Equal1~1_combout\,
	combout => \Equal1~3_combout\);

-- Location: LCFF_X21_Y9_N3
\EnCnt_reg[1]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[1]~18_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(1));

-- Location: LCCOMB_X21_Y9_N4
\EnCnt_reg[2]~20\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[2]~20_combout\ = (EnCnt_reg(2) & (\EnCnt_reg[1]~19\ $ (GND))) # (!EnCnt_reg(2) & (!\EnCnt_reg[1]~19\ & VCC))
-- \EnCnt_reg[2]~21\ = CARRY((EnCnt_reg(2) & !\EnCnt_reg[1]~19\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(2),
	datad => VCC,
	cin => \EnCnt_reg[1]~19\,
	combout => \EnCnt_reg[2]~20_combout\,
	cout => \EnCnt_reg[2]~21\);

-- Location: LCFF_X21_Y9_N5
\EnCnt_reg[2]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[2]~20_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(2));

-- Location: LCCOMB_X21_Y9_N8
\EnCnt_reg[4]~24\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[4]~24_combout\ = (EnCnt_reg(4) & (\EnCnt_reg[3]~23\ $ (GND))) # (!EnCnt_reg(4) & (!\EnCnt_reg[3]~23\ & VCC))
-- \EnCnt_reg[4]~25\ = CARRY((EnCnt_reg(4) & !\EnCnt_reg[3]~23\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(4),
	datad => VCC,
	cin => \EnCnt_reg[3]~23\,
	combout => \EnCnt_reg[4]~24_combout\,
	cout => \EnCnt_reg[4]~25\);

-- Location: LCFF_X21_Y9_N9
\EnCnt_reg[4]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[4]~24_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(4));

-- Location: LCCOMB_X21_Y9_N14
\EnCnt_reg[7]~30\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[7]~30_combout\ = (EnCnt_reg(7) & (!\EnCnt_reg[6]~29\)) # (!EnCnt_reg(7) & ((\EnCnt_reg[6]~29\) # (GND)))
-- \EnCnt_reg[7]~31\ = CARRY((!\EnCnt_reg[6]~29\) # (!EnCnt_reg(7)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(7),
	datad => VCC,
	cin => \EnCnt_reg[6]~29\,
	combout => \EnCnt_reg[7]~30_combout\,
	cout => \EnCnt_reg[7]~31\);

-- Location: LCFF_X21_Y9_N15
\EnCnt_reg[7]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[7]~30_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(7));

-- Location: LCCOMB_X21_Y9_N18
\EnCnt_reg[9]~34\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[9]~34_combout\ = (EnCnt_reg(9) & (!\EnCnt_reg[8]~33\)) # (!EnCnt_reg(9) & ((\EnCnt_reg[8]~33\) # (GND)))
-- \EnCnt_reg[9]~35\ = CARRY((!\EnCnt_reg[8]~33\) # (!EnCnt_reg(9)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(9),
	datad => VCC,
	cin => \EnCnt_reg[8]~33\,
	combout => \EnCnt_reg[9]~34_combout\,
	cout => \EnCnt_reg[9]~35\);

-- Location: LCFF_X21_Y9_N19
\EnCnt_reg[9]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[9]~34_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(9));

-- Location: LCCOMB_X21_Y9_N22
\EnCnt_reg[11]~38\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[11]~38_combout\ = (EnCnt_reg(11) & (!\EnCnt_reg[10]~37\)) # (!EnCnt_reg(11) & ((\EnCnt_reg[10]~37\) # (GND)))
-- \EnCnt_reg[11]~39\ = CARRY((!\EnCnt_reg[10]~37\) # (!EnCnt_reg(11)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(11),
	datad => VCC,
	cin => \EnCnt_reg[10]~37\,
	combout => \EnCnt_reg[11]~38_combout\,
	cout => \EnCnt_reg[11]~39\);

-- Location: LCFF_X21_Y9_N23
\EnCnt_reg[11]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[11]~38_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(11));

-- Location: LCCOMB_X21_Y9_N26
\EnCnt_reg[13]~42\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[13]~42_combout\ = (EnCnt_reg(13) & (!\EnCnt_reg[12]~41\)) # (!EnCnt_reg(13) & ((\EnCnt_reg[12]~41\) # (GND)))
-- \EnCnt_reg[13]~43\ = CARRY((!\EnCnt_reg[12]~41\) # (!EnCnt_reg(13)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0011110000111111",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(13),
	datad => VCC,
	cin => \EnCnt_reg[12]~41\,
	combout => \EnCnt_reg[13]~42_combout\,
	cout => \EnCnt_reg[13]~43\);

-- Location: LCFF_X21_Y9_N27
\EnCnt_reg[13]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[13]~42_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(13));

-- Location: LCCOMB_X21_Y9_N28
\EnCnt_reg[14]~44\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[14]~44_combout\ = (EnCnt_reg(14) & (\EnCnt_reg[13]~43\ $ (GND))) # (!EnCnt_reg(14) & (!\EnCnt_reg[13]~43\ & VCC))
-- \EnCnt_reg[14]~45\ = CARRY((EnCnt_reg(14) & !\EnCnt_reg[13]~43\))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100001100",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datab => EnCnt_reg(14),
	datad => VCC,
	cin => \EnCnt_reg[13]~43\,
	combout => \EnCnt_reg[14]~44_combout\,
	cout => \EnCnt_reg[14]~45\);

-- Location: LCFF_X21_Y9_N29
\EnCnt_reg[14]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[14]~44_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(14));

-- Location: LCCOMB_X21_Y9_N30
\EnCnt_reg[15]~46\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnCnt_reg[15]~46_combout\ = \EnCnt_reg[14]~45\ $ (EnCnt_reg(15))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0000111111110000",
	sum_lutc_input => "cin")
-- pragma translate_on
PORT MAP (
	datad => EnCnt_reg(15),
	cin => \EnCnt_reg[14]~45\,
	combout => \EnCnt_reg[15]~46_combout\);

-- Location: LCFF_X21_Y9_N31
\EnCnt_reg[15]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnCnt_reg[15]~46_combout\,
	sclr => \Equal1~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => EnCnt_reg(15));

-- Location: LCCOMB_X22_Y9_N14
\EnableFlag_reg~3\ : cycloneii_lcell_comb
-- Equation(s):
-- \EnableFlag_reg~3_combout\ = (\EnableFlag_reg~2_combout\) # (((\Equal1~3_combout\) # (!EnCnt_reg(15))) # (!EnCnt_reg(14)))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111110111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \EnableFlag_reg~2_combout\,
	datab => EnCnt_reg(14),
	datac => EnCnt_reg(15),
	datad => \Equal1~3_combout\,
	combout => \EnableFlag_reg~3_combout\);

-- Location: LCFF_X22_Y9_N15
EnableFlag_reg : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \EnableFlag_reg~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => \EnableFlag_reg~regout\);

-- Location: LCCOMB_X26_Y9_N0
\DR~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \DR~0_combout\ = (\PWM~combout\(1) & \EnableFlag_reg~regout\)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1010000010100000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \PWM~combout\(1),
	datac => \EnableFlag_reg~regout\,
	combout => \DR~0_combout\);

-- Location: LCCOMB_X26_Y9_N2
\DR~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \DR~1_combout\ = (\PWM~combout\(2) & \EnableFlag_reg~regout\)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100000011000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datab => \PWM~combout\(2),
	datac => \EnableFlag_reg~regout\,
	combout => \DR~1_combout\);

-- Location: LCCOMB_X26_Y9_N28
\DR~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \DR~2_combout\ = (\EnableFlag_reg~regout\ & \PWM~combout\(3))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => \EnableFlag_reg~regout\,
	datad => \PWM~combout\(3),
	combout => \DR~2_combout\);

-- Location: LCCOMB_X26_Y9_N6
\DR~3\ : cycloneii_lcell_comb
-- Equation(s):
-- \DR~3_combout\ = (\EnableFlag_reg~regout\ & \PWM~combout\(4))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111000000000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datac => \EnableFlag_reg~regout\,
	datad => \PWM~combout\(4),
	combout => \DR~3_combout\);

-- Location: LCCOMB_X6_Y13_N20
\ENABLE_reg~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \ENABLE_reg~feeder_combout\ = \XD[0]~0\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \XD[0]~0\,
	combout => \ENABLE_reg~feeder_combout\);

-- Location: LCFF_X6_Y13_N21
ENABLE_reg : cycloneii_lcell_ff
PORT MAP (
	clk => \XWE0n~clkctrl_outclk\,
	datain => \ENABLE_reg~feeder_combout\,
	ena => \TRIP1_reg~0_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => \ENABLE_reg~regout\);

-- Location: PIN_17,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\CLK0~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_CLK0,
	combout => \CLK0~combout\);

-- Location: CLKCTRL_G2
\CLK0~clkctrl\ : cycloneii_clkctrl
-- pragma translate_off
GENERIC MAP (
	clock_type => "global clock",
	ena_register_mode => "none")
-- pragma translate_on
PORT MAP (
	inclk => \CLK0~clkctrl_INCLK_bus\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	outclk => \CLK0~clkctrl_outclk\);

-- Location: LCCOMB_X19_Y13_N2
\SignalDelay[0]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \SignalDelay[0]~feeder_combout\ = \YULIU~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \YULIU~combout\,
	combout => \SignalDelay[0]~feeder_combout\);

-- Location: LCFF_X19_Y13_N3
\SignalDelay[0]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \SignalDelay[0]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(0));

-- Location: LCFF_X19_Y13_N5
\SignalDelay[1]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \Q2_ON~combout\,
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(1));

-- Location: LCCOMB_X19_Y13_N4
\INT1_reg~0\ : cycloneii_lcell_comb
-- Equation(s):
-- \INT1_reg~0_combout\ = (\Q2_ON~combout\ & (SignalDelay(1) & (SignalDelay(0) $ (!\YULIU~combout\)))) # (!\Q2_ON~combout\ & (!SignalDelay(1) & (SignalDelay(0) $ (!\YULIU~combout\))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000010000100001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \Q2_ON~combout\,
	datab => SignalDelay(0),
	datac => SignalDelay(1),
	datad => \YULIU~combout\,
	combout => \INT1_reg~0_combout\);

-- Location: LCFF_X19_Y13_N25
\SignalDelay[3]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \WG1~combout\,
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(3));

-- Location: LCFF_X19_Y13_N31
\SignalDelay[2]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \BP_ON~combout\,
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(2));

-- Location: LCCOMB_X19_Y13_N24
\INT1_reg~1\ : cycloneii_lcell_comb
-- Equation(s):
-- \INT1_reg~1_combout\ = (\BP_ON~combout\ & (SignalDelay(2) & (\WG1~combout\ $ (!SignalDelay(3))))) # (!\BP_ON~combout\ & (!SignalDelay(2) & (\WG1~combout\ $ (!SignalDelay(3)))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000001001000001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \BP_ON~combout\,
	datab => \WG1~combout\,
	datac => SignalDelay(3),
	datad => SignalDelay(2),
	combout => \INT1_reg~1_combout\);

-- Location: LCCOMB_X19_Y13_N26
\INT1_reg~2\ : cycloneii_lcell_comb
-- Equation(s):
-- \INT1_reg~2_combout\ = (\INT1_reg~0_combout\ & \INT1_reg~1_combout\)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100000011000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datab => \INT1_reg~0_combout\,
	datac => \INT1_reg~1_combout\,
	combout => \INT1_reg~2_combout\);

-- Location: LCFF_X19_Y13_N7
\SignalDelay[6]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \SO6~combout\,
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(6));

-- Location: LCFF_X19_Y13_N29
\SignalDelay[5]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	sdata => \SO5~combout\,
	sload => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(5));

-- Location: LCCOMB_X25_Y10_N4
\SignalDelay[4]~feeder\ : cycloneii_lcell_comb
-- Equation(s):
-- \SignalDelay[4]~feeder_combout\ = \SO4~combout\

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1111111100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datad => \SO4~combout\,
	combout => \SignalDelay[4]~feeder_combout\);

-- Location: LCFF_X25_Y10_N5
\SignalDelay[4]\ : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \SignalDelay[4]~feeder_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => SignalDelay(4));

-- Location: LCCOMB_X19_Y13_N28
\INT1_reg~3\ : cycloneii_lcell_comb
-- Equation(s):
-- \INT1_reg~3_combout\ = (\SO5~combout\ & (SignalDelay(5) & (\SO4~combout\ $ (!SignalDelay(4))))) # (!\SO5~combout\ & (!SignalDelay(5) & (\SO4~combout\ $ (!SignalDelay(4)))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1000010000100001",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \SO5~combout\,
	datab => \SO4~combout\,
	datac => SignalDelay(5),
	datad => SignalDelay(4),
	combout => \INT1_reg~3_combout\);

-- Location: LCCOMB_X19_Y13_N6
\INT1_reg~4\ : cycloneii_lcell_comb
-- Equation(s):
-- \INT1_reg~4_combout\ = (\INT1_reg~3_combout\ & (\SO6~combout\ $ (!SignalDelay(6))))

-- pragma translate_off
GENERIC MAP (
	lut_mask => "1100001100000000",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	datab => \SO6~combout\,
	datac => SignalDelay(6),
	datad => \INT1_reg~3_combout\,
	combout => \INT1_reg~4_combout\);

-- Location: LCCOMB_X19_Y13_N0
\INT1_reg~5\ : cycloneii_lcell_comb
-- Equation(s):
-- \INT1_reg~5_combout\ = (((!\Equal0~4_combout\ & \INT1_reg~regout\)) # (!\INT1_reg~4_combout\)) # (!\INT1_reg~2_combout\)

-- pragma translate_off
GENERIC MAP (
	lut_mask => "0111001111111111",
	sum_lutc_input => "datac")
-- pragma translate_on
PORT MAP (
	dataa => \Equal0~4_combout\,
	datab => \INT1_reg~2_combout\,
	datac => \INT1_reg~regout\,
	datad => \INT1_reg~4_combout\,
	combout => \INT1_reg~5_combout\);

-- Location: LCFF_X19_Y13_N1
INT1_reg : cycloneii_lcell_ff
PORT MAP (
	clk => \CLK0~clkctrl_outclk\,
	datain => \INT1_reg~5_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	regout => \INT1_reg~regout\);

-- Location: PIN_99,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[3]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(3),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(3));

-- Location: PIN_100,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[4]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(4),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(4));

-- Location: PIN_101,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[5]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(5),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(5));

-- Location: PIN_103,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[6]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => XD_reg(6),
	oe => \ALT_INV_XD~16_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(6));

-- Location: PIN_104,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\XD[7]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	open_drain_output => "true",
	operation_mode => "bidir",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \XD~16_combout\,
	oe => VCC,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	padio => XD(7));

-- Location: PIN_91,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: Default
\CLK4~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "input",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => GND,
	padio => ww_CLK4);

-- Location: PIN_136,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\TRIP1~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \TRIP1_reg~regout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_TRIP1);

-- Location: PIN_135,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\TRIP2~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \ALT_INV_TRIP2_reg~regout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_TRIP2);

-- Location: PIN_144,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\DR[1]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \DR~0_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_DR(1));

-- Location: PIN_143,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\DR[2]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \DR~1_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_DR(2));

-- Location: PIN_142,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\DR[3]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => GND,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_DR(3));

-- Location: PIN_141,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\DR[4]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => GND,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_DR(4));

-- Location: PIN_139,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\DR[5]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \DR~2_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_DR(5));

-- Location: PIN_137,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\DR[6]~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \DR~3_combout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_DR(6));

-- Location: PIN_4,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\ENABLE~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \ALT_INV_ENABLE_reg~regout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_ENABLE);

-- Location: PIN_93,	 I/O Standard: 3.3-V LVTTL,	 Current Strength: 24mA
\INT1~I\ : cycloneii_io
-- pragma translate_off
GENERIC MAP (
	input_async_reset => "none",
	input_power_up => "low",
	input_register_mode => "none",
	input_sync_reset => "none",
	oe_async_reset => "none",
	oe_power_up => "low",
	oe_register_mode => "none",
	oe_sync_reset => "none",
	operation_mode => "output",
	output_async_reset => "none",
	output_power_up => "low",
	output_register_mode => "none",
	output_sync_reset => "none")
-- pragma translate_on
PORT MAP (
	datain => \INT1_reg~regout\,
	devclrn => ww_devclrn,
	devpor => ww_devpor,
	devoe => ww_devoe,
	oe => VCC,
	padio => ww_INT1);
END structure;


