include config/global.mk

.PHONY: all run always clean kernel bootloader

all: kernel bootloader

include config/toolchain.mk

run: all

kernel: always
	$(MAKE) -C $(KERNEL_SRC_DIR)

bootloader: always
	$(MAKE) -C $(BOOTLOADER_SRC_DIR)

always: clean

clean:
	$(MAKE) -C $(KERNEL_SRC_DIR) clean
	$(MAKE) -C $(BOOTLOADER_SRC_DIR) clean