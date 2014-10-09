################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
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
16_Pilot_IR.lss \

FLASH_IMAGE += \
16_Pilot_IR.hex \

EEPROM_IMAGE += \
16_Pilot_IR.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 16_Pilot_IR.elf secondary-outputs

# Tool invocations
16_Pilot_IR.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,16_Pilot_IR.map -mmcu=attiny2313 -o"16_Pilot_IR.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

16_Pilot_IR.lss: 16_Pilot_IR.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 16_Pilot_IR.elf  >"16_Pilot_IR.lss"
	@echo 'Finished building: $@'
	@echo ' '

16_Pilot_IR.hex: 16_Pilot_IR.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 16_Pilot_IR.elf  "16_Pilot_IR.hex"
	@echo 'Finished building: $@'
	@echo ' '

16_Pilot_IR.eep: 16_Pilot_IR.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 16_Pilot_IR.elf  "16_Pilot_IR.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 16_Pilot_IR.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny2313 16_Pilot_IR.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 16_Pilot_IR.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
