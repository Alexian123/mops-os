TARGET_CFLAGS += -ffreestanding -nostdlib -I$(KERNEL_INCLUDE_DIR) -falign-jumps -falign-functions -falign-labels -falign-loops \
	-fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label 	\
	-Wno-cpp -Wno-unused-parameter -nostartfiles -nodefaultlibs -Wall -O0
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T $(KERNEL_SRC_DIR)/arch/i686/linker.ld -nostdlib -ffreestanding -O0
TARGET_ASMFLAGS += -F dwarf

HEADERS_C=$(shell find . -type f -name "*.h")
SOURCES_C=$(shell find . -type f -name "*.c")
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/kernel/c/%.o, $(SOURCES_C))

HEADERS_ASM=$(shell find . -type f -name "*.inc")
SOURCES_ASM=$(shell find . -type f -name "*.asm")
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/kernel/asm/%.elf, $(SOURCES_ASM))

.PHONY: all kernel clean

all: kernel

kernel: $(BUILD_DIR)/kernel/kernel.bin

$(BUILD_DIR)/kernel/kernel.elf: $(OBJECTS_ASM) $(OBJECTS_C)
	@mkdir -p $(@D)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/kernel/kernel.map -o $@ $^ $(TARGET_LIBS)
	@echo "--> Linked  kernel.elf"

$(BUILD_DIR)/kernel/kernel.bin: $(BUILD_DIR)/kernel/kernel.elf
	$(TARGET)-objcopy -O binary $< $@
	@echo "--> Generated kernel.bin"

$(BUILD_DIR)/kernel/c/%.o: %.c $(HEADERS_C)
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<
	@echo "--> Compiled: " $<

$(BUILD_DIR)/kernel/asm/%.elf: %.asm $(HEADERS_ASM)
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo "--> Assembled: " $<

clean:
	@rm -f $(BUILD_DIR)/kernel.bin
	@rm -f $(BUILD_DIR)/kernel.map
	@rm -rf $(BUILD_DIR)/kernel