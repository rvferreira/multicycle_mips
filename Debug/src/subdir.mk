################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/commonVariables.cc \
../src/control_signals.cc \
../src/cpu_resources.cc \
../src/uc.cc \
../src/utils.cc 

CPP_SRCS += \
../src/main.cpp 

CC_DEPS += \
./src/commonVariables.d \
./src/control_signals.d \
./src/cpu_resources.d \
./src/uc.d \
./src/utils.d 

OBJS += \
./src/commonVariables.o \
./src/control_signals.o \
./src/cpu_resources.o \
./src/main.o \
./src/uc.o \
./src/utils.o 

CPP_DEPS += \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


