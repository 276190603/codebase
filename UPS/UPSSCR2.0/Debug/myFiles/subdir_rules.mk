################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
myFiles/FaultDetect.obj: ../myFiles/FaultDetect.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/FaultDetect.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myFiles/GlobalDefine.obj: ../myFiles/GlobalDefine.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/GlobalDefine.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myFiles/PhaseLock.obj: ../myFiles/PhaseLock.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/PhaseLock.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myFiles/SampleAndControl.obj: ../myFiles/SampleAndControl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/SampleAndControl.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myFiles/Scr.obj: ../myFiles/Scr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/Scr.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myFiles/SystemTick.obj: ../myFiles/SystemTick.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/SystemTick.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myFiles/main.obj: ../myFiles/main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/myFiles" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/communication" --include_path="E:/Development/ccs7/work1/UPSSCR2.0/includes" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="myFiles/main.d" --obj_directory="myFiles" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


