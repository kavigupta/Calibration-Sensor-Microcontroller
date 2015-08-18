################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Analysis.c \
../src/Constants.c \
../src/Controller.c \
../src/DataSet.c \
../src/IO.c \
../src/Matrix.c \
../src/Test.c \
../src/Utils.c \
../src/Vector.c \
../src/analysis_peakfind.c \
../src/analysis_peaktools.c \
../src/analysis_preprocessing.c \
../src/analysis_trial_separation.c 

OBJS += \
./src/Analysis.o \
./src/Constants.o \
./src/Controller.o \
./src/DataSet.o \
./src/IO.o \
./src/Matrix.o \
./src/Test.o \
./src/Utils.o \
./src/Vector.o \
./src/analysis_peakfind.o \
./src/analysis_peaktools.o \
./src/analysis_preprocessing.o \
./src/analysis_trial_separation.o 

C_DEPS += \
./src/Analysis.d \
./src/Constants.d \
./src/Controller.d \
./src/DataSet.d \
./src/IO.d \
./src/Matrix.d \
./src/Test.d \
./src/Utils.d \
./src/Vector.d \
./src/analysis_peakfind.d \
./src/analysis_peaktools.d \
./src/analysis_preprocessing.d \
./src/analysis_trial_separation.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


