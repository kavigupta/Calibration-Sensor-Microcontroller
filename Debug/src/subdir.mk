################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Analysis.c \
../src/Controller.c \
../src/DataSet.c \
../src/IO.c \
../src/Matrix.c \
../src/Test.c \
../src/Utils.c \
../src/Vector.c 

OBJS += \
./src/Analysis.o \
./src/Controller.o \
./src/DataSet.o \
./src/IO.o \
./src/Matrix.o \
./src/Test.o \
./src/Utils.o \
./src/Vector.o 

C_DEPS += \
./src/Analysis.d \
./src/Controller.d \
./src/DataSet.d \
./src/IO.d \
./src/Matrix.d \
./src/Test.d \
./src/Utils.d \
./src/Vector.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


