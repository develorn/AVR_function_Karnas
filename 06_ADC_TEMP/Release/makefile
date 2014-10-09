################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include LCD/subdir.mk
-include 1wire/subdir.mk
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
06_ADC_TEMP.lss \

FLASH_IMAGE += \
06_ADC_TEMP.hex \

EEPROM_IMAGE += \
06_ADC_TEMP.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 06_ADC_TEMP.elf secondary-outputs

# Tool invocations
06_ADC_TEMP.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,06_ADC_TEMP.map -mmcu=atmega32 -o"06_ADC_TEMP.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

06_ADC_TEMP.lss: 06_ADC_TEMP.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 06_ADC_TEMP.elf  >"06_ADC_TEMP.lss"
	@echo 'Finished building: $@'
	@echo ' '

06_ADC_TEMP.hex: 06_ADC_TEMP.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 06_ADC_TEMP.elf  "06_ADC_TEMP.hex"
	@echo 'Finished building: $@'
	@echo ' '

06_ADC_TEMP.eep: 06_ADC_TEMP.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 06_ADC_TEMP.elf  "06_ADC_TEMP.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 06_ADC_TEMP.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 06_ADC_TEMP.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 06_ADC_TEMP.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
