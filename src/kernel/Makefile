TARGET_CFLAGS += -Wall -ffreestanding -nostdlib -I$(KERNEL_INCLUDE_DIR) -I$(LIBK_INCLUDE_DIR)
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T $(KERNEL_SRC_DIR)/arch/i686/linker.ld -nostdlib -ffreestanding -O2

HEADERS_C=$(shell find . -type f -name "*.h")
SOURCES_C=$(shell find . -type f -name "*.c")
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/kernel/c/%.o, $(SOURCES_C))

HEADERS_ASM=$(shell find . -type f -name "*.inc")
SOURCES_ASM=$(shell find . -type f -name "*.asm")
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/kernel/asm/%.elf, $(SOURCES_ASM))

.PHONY: all kernel clean

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@echo "--> Linked  kernel.bin"

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