################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include MKUART/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
07_UART_kalibracja.lss \

FLASH_IMAGE += \
07_UART_kalibracja.hex \

EEPROM_IMAGE += \
07_UART_kalibracja.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 07_UART_kalibracja.elf secondary-outputs

# Tool invocations
07_UART_kalibracja.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,07_UART_kalibracja.map -mmcu=atmega32 -o"07_UART_kalibracja.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

07_UART_kalibracja.lss: 07_UART_kalibracja.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 07_UART_kalibracja.elf  >"07_UART_kalibracja.lss"
	@echo 'Finished building: $@'
	@echo ' '

07_UART_kalibracja.hex: 07_UART_kalibracja.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 07_UART_kalibracja.elf  "07_UART_kalibracja.hex"
	@echo 'Finished building: $@'
	@echo ' '

07_UART_kalibracja.eep: 07_UART_kalibracja.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 07_UART_kalibracja.elf  "07_UART_kalibracja.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 07_UART_kalibracja.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 07_UART_kalibracja.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 07_UART_kalibracja.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
