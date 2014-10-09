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
20_ETHERNET.lss \

FLASH_IMAGE += \
20_ETHERNET.hex \

EEPROM_IMAGE += \
20_ETHERNET.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 20_ETHERNET.elf secondary-outputs

# Tool invocations
20_ETHERNET.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,20_ETHERNET.map -mmcu=atmega32 -o"20_ETHERNET.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

20_ETHERNET.lss: 20_ETHERNET.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 20_ETHERNET.elf  >"20_ETHERNET.lss"
	@echo 'Finished building: $@'
	@echo ' '

20_ETHERNET.hex: 20_ETHERNET.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 20_ETHERNET.elf  "20_ETHERNET.hex"
	@echo 'Finished building: $@'
	@echo ' '

20_ETHERNET.eep: 20_ETHERNET.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 20_ETHERNET.elf  "20_ETHERNET.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 20_ETHERNET.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 20_ETHERNET.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 20_ETHERNET.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
