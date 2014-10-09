################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../SW_RS_ELM_CHAN/suart.S 

OBJS += \
./SW_RS_ELM_CHAN/suart.o 

S_UPPER_DEPS += \
./SW_RS_ELM_CHAN/suart.d 


# Each subdirectory must supply rules for building sources it contributes
SW_RS_ELM_CHAN/%.o: ../SW_RS_ELM_CHAN/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -gstabs -mmcu=attiny26 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


