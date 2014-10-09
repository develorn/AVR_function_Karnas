################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include MKUART/subdir.mk
-include LCD/subdir.mk
-include IR_DECODE/subdir.mk
-include I2C_TWI/subdir.mk
-include 1Wire/subdir.mk
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
19_test.lss \

FLASH_IMAGE += \
19_test.hex \

EEPROM_IMAGE += \
19_test.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 19_test.elf secondary-outputs

# Tool invocations
19_test.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,19_test.map -mmcu=atmega32 -o"19_test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

19_test.lss: 19_test.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 19_test.elf  >"19_test.lss"
	@echo 'Finished building: $@'
	@echo ' '

19_test.hex: 19_test.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 19_test.elf  "19_test.hex"
	@echo 'Finished building: $@'
	@echo ' '

19_test.eep: 19_test.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 19_test.elf  "19_test.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 19_test.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 19_test.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 19_test.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
