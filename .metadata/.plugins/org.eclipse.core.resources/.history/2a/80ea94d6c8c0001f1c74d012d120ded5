################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include SDCARD/option/subdir.mk
-include SDCARD/subdir.mk
-include LCD/subdir.mk
-include IR_DECODE/subdir.mk
-include I2C_TWI/subdir.mk
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
15_Karta_SD.lss \

FLASH_IMAGE += \
15_Karta_SD.hex \

EEPROM_IMAGE += \
15_Karta_SD.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: 15_Karta_SD.elf secondary-outputs

# Tool invocations
15_Karta_SD.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,15_Karta_SD.map -mmcu=atmega32 -o"15_Karta_SD.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

15_Karta_SD.lss: 15_Karta_SD.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S 15_Karta_SD.elf  >"15_Karta_SD.lss"
	@echo 'Finished building: $@'
	@echo ' '

15_Karta_SD.hex: 15_Karta_SD.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex 15_Karta_SD.elf  "15_Karta_SD.hex"
	@echo 'Finished building: $@'
	@echo ' '

15_Karta_SD.eep: 15_Karta_SD.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex 15_Karta_SD.elf  "15_Karta_SD.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: 15_Karta_SD.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 15_Karta_SD.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) 15_Karta_SD.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
