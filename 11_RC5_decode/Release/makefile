################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include LCD/subdir.mk
-include IR_DECODE/subdir.mk
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
11_RC5_decode.lss \

FLASH_IMAGE += \
11_RC5_decode.hex \

EEPROM_IMAGE += \
11_RC5_decode.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 11_RC5_decode.elf secondary-outputs

# Tool invocations
11_RC5_decode.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,11_RC5_decode.map -mmcu=atmega32 -o"11_RC5_decode.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

11_RC5_decode.lss: 11_RC5_decode.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 11_RC5_decode.elf  >"11_RC5_decode.lss"
	@echo 'Finished building: $@'
	@echo ' '

11_RC5_decode.hex: 11_RC5_decode.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 11_RC5_decode.elf  "11_RC5_decode.hex"
	@echo 'Finished building: $@'
	@echo ' '

11_RC5_decode.eep: 11_RC5_decode.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 11_RC5_decode.elf  "11_RC5_decode.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 11_RC5_decode.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 11_RC5_decode.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 11_RC5_decode.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
