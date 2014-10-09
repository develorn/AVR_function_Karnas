################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FatFS/ff.c \
../FatFS/mmc.c 

OBJS += \
./FatFS/ff.o \
./FatFS/mmc.o 

C_DEPS += \
./FatFS/ff.d \
./FatFS/mmc.d 


# Each subdirectory must supply rules for building sources it contributes
FatFS/%.o: ../FatFS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=11059200UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


