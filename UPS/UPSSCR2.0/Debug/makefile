################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./FpgaDriver/FpgaDriver.obj" \
"./communication/Can.obj" \
"./communication/RS232.obj" \
"./communication/RS485.obj" \
"./communication/communication.obj" \
"./myFiles/FaultDetect.obj" \
"./myFiles/GlobalDefine.obj" \
"./myFiles/PhaseLock.obj" \
"./myFiles/SampleAndControl.obj" \
"./myFiles/Scr.obj" \
"./myFiles/SystemTick.obj" \
"./myFiles/main.obj" \
"./sources/DSP2833x_ADC_cal.obj" \
"./sources/DSP2833x_Adc.obj" \
"./sources/DSP2833x_CodeStartBranch.obj" \
"./sources/DSP2833x_CpuTimers.obj" \
"./sources/DSP2833x_DefaultIsr.obj" \
"./sources/DSP2833x_ECan.obj" \
"./sources/DSP2833x_GlobalVariableDefs.obj" \
"./sources/DSP2833x_MemCopy.obj" \
"./sources/DSP2833x_PieCtrl.obj" \
"./sources/DSP2833x_PieVect.obj" \
"./sources/DSP2833x_SysCtrl.obj" \
"./sources/DSP2833x_usDelay.obj" \
"../cmd/DSP2833x_Headers_nonBIOS.cmd" \
"../cmd/F28335.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include FpgaDriver/subdir_vars.mk
-include cmd/subdir_vars.mk
-include communication/subdir_vars.mk
-include myFiles/subdir_vars.mk
-include sources/subdir_vars.mk
-include FpgaDriver/subdir_rules.mk
-include cmd/subdir_rules.mk
-include communication/subdir_rules.mk
-include myFiles/subdir_rules.mk
-include sources/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
UPSSCR2.0.out \

EXE_OUTPUTS__QUOTED += \
"UPSSCR2.0.out" \

BIN_OUTPUTS += \
UPSSCR2.0.hex \

BIN_OUTPUTS__QUOTED += \
"UPSSCR2.0.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "UPSSCR2.0.out"

# Tool invocations
UPSSCR2.0.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --diag_warning=225 --diag_wrap=off --display_error_number -z -m"UPSSCR2.0.map" --stack_size=0x300 --warn_sections -i"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/lib" -i"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="UPSSCR2.0_linkInfo.xml" --rom_model -o "UPSSCR2.0.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

UPSSCR2.0.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C2000 Hex Utility'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/hex2000"  -o "UPSSCR2.0.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "FpgaDriver\FpgaDriver.obj" "communication\Can.obj" "communication\RS232.obj" "communication\RS485.obj" "communication\communication.obj" "myFiles\FaultDetect.obj" "myFiles\GlobalDefine.obj" "myFiles\PhaseLock.obj" "myFiles\SampleAndControl.obj" "myFiles\Scr.obj" "myFiles\SystemTick.obj" "myFiles\main.obj" "sources\DSP2833x_ADC_cal.obj" "sources\DSP2833x_Adc.obj" "sources\DSP2833x_CodeStartBranch.obj" "sources\DSP2833x_CpuTimers.obj" "sources\DSP2833x_DefaultIsr.obj" "sources\DSP2833x_ECan.obj" "sources\DSP2833x_GlobalVariableDefs.obj" "sources\DSP2833x_MemCopy.obj" "sources\DSP2833x_PieCtrl.obj" "sources\DSP2833x_PieVect.obj" "sources\DSP2833x_SysCtrl.obj" "sources\DSP2833x_usDelay.obj" 
	-$(RM) "FpgaDriver\FpgaDriver.d" "communication\Can.d" "communication\RS232.d" "communication\RS485.d" "communication\communication.d" "myFiles\FaultDetect.d" "myFiles\GlobalDefine.d" "myFiles\PhaseLock.d" "myFiles\SampleAndControl.d" "myFiles\Scr.d" "myFiles\SystemTick.d" "myFiles\main.d" "sources\DSP2833x_Adc.d" "sources\DSP2833x_CpuTimers.d" "sources\DSP2833x_DefaultIsr.d" "sources\DSP2833x_ECan.d" "sources\DSP2833x_GlobalVariableDefs.d" "sources\DSP2833x_MemCopy.d" "sources\DSP2833x_PieCtrl.d" "sources\DSP2833x_PieVect.d" "sources\DSP2833x_SysCtrl.d" 
	-$(RM) "sources\DSP2833x_ADC_cal.d" "sources\DSP2833x_CodeStartBranch.d" "sources\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

