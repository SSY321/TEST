################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PGV150/PGV150.c 

OBJS += \
./PGV150/PGV150.o 

C_DEPS += \
./PGV150/PGV150.d 


# Each subdirectory must supply rules for building sources it contributes
PGV150/%.o: ../PGV150/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


