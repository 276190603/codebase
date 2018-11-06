################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
communication/Can.obj: ../communication/Can.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/work1/UPSBYPASS/communication" --include_path="E:/Development/ccs7/work1/UPSBYPASS/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSBYPASS/includes" --include_path="E:/Development/ccs7/work1/UPSBYPASS/myFiles" --include_path="E:/Development/ccs7/work1/UPSBYPASS/sources" --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="communication/Can.d" --obj_directory="communication" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

communication/RS232.obj: ../communication/RS232.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/work1/UPSBYPASS/communication" --include_path="E:/Development/ccs7/work1/UPSBYPASS/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSBYPASS/includes" --include_path="E:/Development/ccs7/work1/UPSBYPASS/myFiles" --include_path="E:/Development/ccs7/work1/UPSBYPASS/sources" --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="communication/RS232.d" --obj_directory="communication" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

communication/RS485.obj: ../communication/RS485.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/work1/UPSBYPASS/communication" --include_path="E:/Development/ccs7/work1/UPSBYPASS/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSBYPASS/includes" --include_path="E:/Development/ccs7/work1/UPSBYPASS/myFiles" --include_path="E:/Development/ccs7/work1/UPSBYPASS/sources" --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="communication/RS485.d" --obj_directory="communication" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

communication/communication.obj: ../communication/communication.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Development/ccs7/work1/UPSBYPASS/communication" --include_path="E:/Development/ccs7/work1/UPSBYPASS/FpgaDriver" --include_path="E:/Development/ccs7/work1/UPSBYPASS/includes" --include_path="E:/Development/ccs7/work1/UPSBYPASS/myFiles" --include_path="E:/Development/ccs7/work1/UPSBYPASS/sources" --include_path="E:/Development/ccs7/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="communication/communication.d" --obj_directory="communication" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


