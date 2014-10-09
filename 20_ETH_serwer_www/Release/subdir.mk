################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../basic_web_server_example.c \
../enc28j60.c \
../ip_arp_udp_tcp.c 

OBJS += \
./basic_web_server_example.o \
./enc28j60.o \
./ip_arp_udp_tcp.o 

C_DEPS += \
./basic_web_server_example.d \
./enc28j60.d \
./ip_arp_udp_tcp.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=11059200UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


