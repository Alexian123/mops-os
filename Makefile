include config/global.mk

.PHONY: all raw run run_debug debug always clean kernel bootloader

all: raw

include config/toolchain.mk

RAW_IMAGE=$(BUILD_DIR)/mops-os.bin

run: raw
	qemu-system-x86_64 -hda $(RAW_IMAGE)

run_debug: raw
	qemu-system-x86_64 -hda $(RAW_IMAGE) -S -gdb tcp::1234

debug: raw
	gdb -x $(SCRIPTS_DIR)/debug.gdb

raw: kernel bootloader
	dd if=$(BUILD_DIR)/bootloader/boot.bin >> $(RAW_IMAGE)
	dd if=$(BUILD_DIR)/kernel/kernel.bin >> $(RAW_IMAGE)
	dd if=/dev/zero bs=512 count=100 >> $(RAW_IMAGE)

kernel: always
	$(MAKE) -C $(KERNEL_SRC_DIR)

bootloader: always
	$(MAKE) -C $(BOOTLOADER_SRC_DIR)

always: clean

clean:
	$(MAKE) -C $(KERNEL_SRC_DIR) clean
	$(MAKE) -C $(BOOTLOADER_SRC_DIR) clean
	rm -f $(RAW_IMAGE)