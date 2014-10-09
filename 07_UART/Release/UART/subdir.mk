################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UART/getchar.c \
../UART/getstr.c \
../UART/putchar.c \
../UART/putint.c \
../UART/putstr.c \
../UART/putstr_P.c \
../UART/rxlen.c \
../UART/uart_mk.c \
../UART/waitchar.c 

OBJS += \
./UART/getchar.o \
./UART/getstr.o \
./UART/putchar.o \
./UART/putint.o \
./UART/putstr.o \
./UART/putstr_P.o \
./UART/rxlen.o \
./UART/uart_mk.o \
./UART/waitchar.o 

C_DEPS += \
./UART/getchar.d \
./UART/getstr.d \
./UART/putchar.d \
./UART/putint.d \
./UART/putstr.d \
./UART/putstr_P.d \
./UART/rxlen.d \
./UART/uart_mk.d \
./UART/waitchar.d 


# Each subdirectory must supply rules for building sources it contributes
UART/%.o: ../UART/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


