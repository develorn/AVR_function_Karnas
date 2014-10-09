###############################################################################
# Makefile for the project SD_Card
###############################################################################

## General Flags
PROJECT = SD_Card
MCU = atmega32
TARGET = SD_Card.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                 -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=SD_Card.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = FAT32.o SD_main.o SD_routines.o SPI_routines.o UART_routines.o RTC_routines.o i2c_routines.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) SD_Card.hex SD_Card.eep SD_Card.lss size

## Compile
FAT32.o: ../FAT32.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

SD_main.o: ../SD_main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

SD_routines.o: ../SD_routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

SPI_routines.o: ../SPI_routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

UART_routines.o: ../UART_routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

RTC_routines.o: ../RTC_routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c_routines.o: ../i2c_routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) SD_Card.elf dep/* SD_Card.hex SD_Card.eep SD_Card.lss SD_Card.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

