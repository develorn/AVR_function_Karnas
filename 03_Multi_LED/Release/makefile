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
03_Multi_LED.lss \

FLASH_IMAGE += \
03_Multi_LED.hex \

EEPROM_IMAGE += \
03_Multi_LED.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 03_Multi_LED.elf secondary-outputs

# Tool invocations
03_Multi_LED.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,03_Multi_LED.map -mmcu=atmega32 -o"03_Multi_LED.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

03_Multi_LED.lss: 03_Multi_LED.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 03_Multi_LED.elf  >"03_Multi_LED.lss"
	@echo 'Finished building: $@'
	@echo ' '

03_Multi_LED.hex: 03_Multi_LED.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 03_Multi_LED.elf  "03_Multi_LED.hex"
	@echo 'Finished building: $@'
	@echo ' '

03_Multi_LED.eep: 03_Multi_LED.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 03_Multi_LED.elf  "03_Multi_LED.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 03_Multi_LED.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 03_Multi_LED.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 03_Multi_LED.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
