################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Odom_Calib/Odom_Calib.c 

OBJS += \
./Odom_Calib/Odom_Calib.o 

C_DEPS += \
./Odom_Calib/Odom_Calib.d 


# Each subdirectory must supply rules for building sources it contributes
Odom_Calib/%.o: ../Odom_Calib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


