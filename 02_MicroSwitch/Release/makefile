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
02_MicroSwitch.lss \

FLASH_IMAGE += \
02_MicroSwitch.hex \

EEPROM_IMAGE += \
02_MicroSwitch.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 02_MicroSwitch.elf secondary-outputs

# Tool invocations
02_MicroSwitch.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,02_MicroSwitch.map -mmcu=atmega32 -o"02_MicroSwitch.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

02_MicroSwitch.lss: 02_MicroSwitch.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 02_MicroSwitch.elf  >"02_MicroSwitch.lss"
	@echo 'Finished building: $@'
	@echo ' '

02_MicroSwitch.hex: 02_MicroSwitch.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 02_MicroSwitch.elf  "02_MicroSwitch.hex"
	@echo 'Finished building: $@'
	@echo ' '

02_MicroSwitch.eep: 02_MicroSwitch.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 02_MicroSwitch.elf  "02_MicroSwitch.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 02_MicroSwitch.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 02_MicroSwitch.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 02_MicroSwitch.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
