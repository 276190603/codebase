################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
sources/DSP2833x_ADC_cal.obj: ../sources/DSP2833x_ADC_cal.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_ADC_cal.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_Adc.obj: ../sources/DSP2833x_Adc.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_Adc.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_CodeStartBranch.obj: ../sources/DSP2833x_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_CodeStartBranch.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_CpuTimers.obj: ../sources/DSP2833x_CpuTimers.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_CpuTimers.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_DefaultIsr.obj: ../sources/DSP2833x_DefaultIsr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_DefaultIsr.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_ECan.obj: ../sources/DSP2833x_ECan.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_ECan.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_GlobalVariableDefs.obj: ../sources/DSP2833x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_GlobalVariableDefs.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_MemCopy.obj: ../sources/DSP2833x_MemCopy.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_MemCopy.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_PieCtrl.obj: ../sources/DSP2833x_PieCtrl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_PieCtrl.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_PieVect.obj: ../sources/DSP2833x_PieVect.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_PieVect.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_SysCtrl.obj: ../sources/DSP2833x_SysCtrl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_SysCtrl.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sources/DSP2833x_usDelay.obj: ../sources/DSP2833x_usDelay.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/communication" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/includes" --include_path="E:/Development/ccs7/work1/UPSINV2.0.170104/myFiles" -g --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sources/DSP2833x_usDelay.d" --obj_directory="sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


