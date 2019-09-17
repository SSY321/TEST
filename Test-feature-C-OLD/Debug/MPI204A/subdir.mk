################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MPI204A/MPI204A.c 

OBJS += \
./MPI204A/MPI204A.o 

C_DEPS += \
./MPI204A/MPI204A.d 


# Each subdirectory must supply rules for building sources it contributes
MPI204A/%.o: ../MPI204A/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


