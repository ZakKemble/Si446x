
PROJECT=libSi446x

MCU=atmega328
F_CPU=8000000UL
CPU_DIV=clock_div_1

COMPILER=avr-
AVRDUDE=avrdude

SRC_DIR=.
INC_DIR=.
OBJ_DIR=obj
BIN_DIR=bin

FILES= \
	examples/callbacks_irq.c \
	Si446x.c \
	Si446x_spi.c

CFLAGS= \
	-mmcu=$(MCU) \
	-c \
	-x c \
	-Wall \
	-Wextra \
	-Wstrict-prototypes \
	-Wunused-result \
	-std=gnu99 \
	-O1 \
	-mcall-prologues \
	-funsigned-char \
	-funsigned-bitfields \
	-fno-tree-scev-cprop \
	-ffreestanding \
	-fno-exceptions \
	-maccumulate-args \
	-ffunction-sections \
	-fdata-sections \
	-fpack-struct \
	-fshort-enums \
	-mrelax \
	-g2 \
	-I$(INC_DIR) \
	-fno-tree-loop-optimize \
	-fno-caller-saves

LDFLAGS= \
	-Wl,-Map="$(BIN_DIR)\$(PROJECT).map" \
	-Wl,--start-group \
	-Wl,--end-group \
	-Wl,--gc-sections \
	-Wa,-g \
	-mrelax \
	-mmcu=$(MCU)

DEFS= \
	-DF_CPU=$(F_CPU) \
	-DCPU_DIV=$(CPU_DIV)

LDLIBS=

DEPFLAGS= \
	-MD -MP -MT "$(@:%.o=%.d)" -MT "$@"

ELF=$(PROJECT).elf
HEX=$(PROJECT).hex
EEP=$(PROJECT).eep
LSS=$(PROJECT).lss

CC=$(COMPILER)gcc
LD=$(COMPILER)gcc
OBJCOPY=$(COMPILER)objcopy
OBJDUMP=$(COMPILER)objdump
SIZE=$(COMPILER)size

SOURCES=$(FILES:%.c=$(SRC_DIR)/%.c)
OBJECTS=$(FILES:%.c=$(OBJ_DIR)/%.o)

all: $(SOURCES) $(ELF)

$(ELF): $(OBJECTS)
	@echo Linking...
	@mkdir $(BIN_DIR) 2>nul || true
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $(BIN_DIR)/$@ $(LDLIBS)
	@$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $(BIN_DIR)/$(ELF) $(BIN_DIR)/$(HEX)
	@$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(BIN_DIR)/$(ELF) $(BIN_DIR)/$(EEP)
	@$(OBJDUMP) -h -S $(BIN_DIR)/$(ELF) > $(BIN_DIR)/$(LSS)
	@echo .
	@echo .
	@$(SIZE) $(BIN_DIR)/$(HEX) $(BIN_DIR)/$(ELF)
	@echo .
	@echo .

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)/%.d Makefile
	@echo Compiling $<...
	@mkdir "$(dir $@)" 2>nul || true
	@$(CC) $(DEPFLAGS) $(DEFS) $(CFLAGS) $< -o $@

clean:
	@rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* 2>nul || true

avrdude:
	$(AVRDUDE) -c arduino -b 250000 -P COM3 -p $(MCU) -U flash:w:$(BIN_DIR)/$(HEX)
	$(AVRDUDE) -c arduino -b 250000 -P COM14 -p $(MCU) -U flash:w:$(BIN_DIR)/$(HEX)

$(OBJ_DIR)/%.d: ;

.PHONY: clean all avrdude

-include $(patsubst %,$(OBJ_DIR)/%.d,$(basename $(FILES)))
