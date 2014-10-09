################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../I2C_SOFT/i2c_soft.c 

OBJS += \
./I2C_SOFT/i2c_soft.o 

C_DEPS += \
./I2C_SOFT/i2c_soft.d 


# Each subdirectory must supply rules for building sources it contributes
I2C_SOFT/%.o: ../I2C_SOFT/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=11059200UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


