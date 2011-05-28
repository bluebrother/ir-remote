# Makefile for ir-remote.
# Based on the mfile template, heavily reduced.
#
SILENT = @
MCU = attiny2313
FORMAT = ihex
TARGET = ir-remote
SRC = main.c rc5.c

CFLAGS = -gstabs -Os -Wall -Wstrict-prototypes -std=gnu99 \
	 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
	 -mmcu=$(MCU) -I.

# Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = dragon_isp
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)

# Default target.
all: build

build: elf hex eep

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym


# Program the device.  
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	@echo OBJCOPY $@
	$(SILENT)$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	@echo OBJCOPY $@
	-$(SILENT)$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	@echo OBJDUMP $@
	$(SILENT)$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	@echo NM $@
	$(SILENT)$(NM) -n $< > $@

# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	@echo LD $@
	$(SILENT)$(CC) $(CFLAGS) $(OBJ) --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
.c.o:
	@echo CC $<
	$(SILENT)$(CC) -c $(CFLAGS) $< -o $@ 

# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d)


.PHONY:	all build elf hex eep lss sym program coff extcoff clean
