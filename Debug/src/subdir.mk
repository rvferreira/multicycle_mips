################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/uc.cc 

C_SRCS += \
../src/commonVariables.c \
../src/control_signals.c \
../src/cpu_resources.c \
../src/main.c \
../src/uc.c \
../src/utils.c 

CC_DEPS += \
./src/uc.d 

OBJS += \
./src/commonVariables.o \
./src/control_signals.o \
./src/cpu_resources.o \
./src/main.o \
./src/uc.o \
./src/utils.o 

C_DEPS += \
./src/commonVariables.d \
./src/control_signals.d \
./src/cpu_resources.d \
./src/main.d \
./src/uc.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


