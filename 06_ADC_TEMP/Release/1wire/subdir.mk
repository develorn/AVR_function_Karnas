################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../1wire/crc8.c \
../1wire/ds18x20.c \
../1wire/onewire.c 

OBJS += \
./1wire/crc8.o \
./1wire/ds18x20.o \
./1wire/onewire.o 

C_DEPS += \
./1wire/crc8.d \
./1wire/ds18x20.d \
./1wire/onewire.d 


# Each subdirectory must supply rules for building sources it contributes
1wire/%.o: ../1wire/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=11059200UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


