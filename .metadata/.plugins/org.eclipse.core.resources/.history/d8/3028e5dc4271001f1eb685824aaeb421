################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include LCD/subdir.mk
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
tiny26_VA.lss \

FLASH_IMAGE += \
tiny26_VA.hex \

EEPROM_IMAGE += \
tiny26_VA.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: tiny26_VA.elf secondary-outputs

# Tool invocations
tiny26_VA.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,tiny26_VA.map -mmcu=attiny26 -o"tiny26_VA.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

tiny26_VA.lss: tiny26_VA.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S tiny26_VA.elf  >"tiny26_VA.lss"
	@echo 'Finished building: $@'
	@echo ' '

tiny26_VA.hex: tiny26_VA.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex tiny26_VA.elf  "tiny26_VA.hex"
	@echo 'Finished building: $@'
	@echo ' '

tiny26_VA.eep: tiny26_VA.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex tiny26_VA.elf  "tiny26_VA.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: tiny26_VA.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny26 tiny26_VA.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) tiny26_VA.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
