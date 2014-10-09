################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDCARD/org_mk/ff.c \
../SDCARD/org_mk/mmc.c 

OBJS += \
./SDCARD/org_mk/ff.o \
./SDCARD/org_mk/mmc.o 

C_DEPS += \
./SDCARD/org_mk/ff.d \
./SDCARD/org_mk/mmc.d 


# Each subdirectory must supply rules for building sources it contributes
SDCARD/org_mk/%.o: ../SDCARD/org_mk/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=11059200UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


