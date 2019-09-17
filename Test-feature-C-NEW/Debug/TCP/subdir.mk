################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TCP/1200.c \
../TCP/TCP.c 

OBJS += \
./TCP/1200.o \
./TCP/TCP.o 

C_DEPS += \
./TCP/1200.d \
./TCP/TCP.d 


# Each subdirectory must supply rules for building sources it contributes
TCP/%.o: ../TCP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


