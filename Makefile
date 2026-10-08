include config/global.mk

.PHONY: all raw run debug always clean kernel bootloader

all: raw

include config/toolchain.mk

RAW_IMAGE=$(BUILD_DIR)/mops-os.bin

run: raw
	qemu-system-x86_64 -hda $(RAW_IMAGE)

debug: raw
	$(SCRIPTS_DIR)/debug.sh $(SCRIPTS_DIR)/debug.gdb $(RAW_IMAGE)

raw: kernel bootloader
	cp $(BUILD_DIR)/bootloader/boot.bin $(RAW_IMAGE)
	dd if=/dev/zero bs=512 count=1 >> $(RAW_IMAGE)

kernel: always
	$(MAKE) -C $(KERNEL_SRC_DIR)

bootloader: always
	$(MAKE) -C $(BOOTLOADER_SRC_DIR)

always: clean

clean:
	$(MAKE) -C $(KERNEL_SRC_DIR) clean
	$(MAKE) -C $(BOOTLOADER_SRC_DIR) clean
	rm -f $(RAW_IMAGE)