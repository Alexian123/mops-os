TARGET_ASMFLAGS=-f bin

BOOTLOADER_SRC=./boot.asm

.PHONY: all bootloader clean

all: bootloader

bootloader: $(BUILD_DIR)/bootloader/boot.bin

$(BUILD_DIR)/bootloader/boot.bin: $(BOOTLOADER_SRC)
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo "--> Assembled: " $<

clean:
	@rm -rf $(BUILD_DIR)/bootloader