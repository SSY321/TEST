################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Interpolation.c \
../App/Movement.c 

OBJS += \
./App/Interpolation.o \
./App/Movement.o 

C_DEPS += \
./App/Interpolation.d \
./App/Movement.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o: ../App/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


