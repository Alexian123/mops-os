#include <common.h>
#include <interrupts/idt.h>
#include <port_io.h>
#include <utils.h>
#include <fs/file.h>

// memory
#include <memory/kheap.h>
#include <memory/paging.h>

// drivers
#include <drivers/vga_text.h>
#include <drivers/disk.h>

// libk
#include <libk/string.h>
#include <libk/ctype.h>
#include <libk/stdlib.h>

static PAGING_4GB_CHUNK *kchunk = NULL;

void ASMCALL kmain(void) {
    vga_clearscr();
    vga_puts("Welcome to MOPS-OS!\n");

    // init kernel heap
    kheap_init();
    vga_puts("Initialized kernel heap.\n");

    // init file systems
    fs_init();
    vga_puts("Initialized file systems.\n");

    // init disks
    disk_locate_and_init();
    vga_puts("Located and initialized all disks (lie).\n");

    // load IDT
    idt_init();
    vga_puts("Loaded IDT.\n");

    // create kernel page dir and load it
    kchunk = paging_create_4gb_chunk(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);    // should remove PAGING_ACCESS_FROM_ALL
    u32 *kernel_page_dir = paging_get_directory_from_4gb_chunk(kchunk);
    paging_switch(kernel_page_dir);
    paging_enable();
    vga_puts("Loaded kernel page directory. Paging is now active.\n");

    enable_interrupts();
    vga_puts("Interrupts are now active. Beware!\n");

    int fd = kopen("0:/hello.txt", "r");
    if (fd < 1) {
        int error_no = -fd;
        char c = DIGIT_NUM_TO_CHAR(error_no);
        vga_puts("Error reading file. CODE: ");
        vga_putc(c);
        vga_putc('\n');
    } else {
        vga_puts("Successfully opened file.\n");

        struct filestat stat;
        memset(&stat, 0, sizeof(stat));
        kstat(fd, &stat);
        char size_str[16];
        memset(size_str, 0, sizeof(size_str));
        itoa(size_str, (int)stat.size);
        vga_puts("File size: ");
        vga_puts(size_str);
        vga_puts(" bytes\n");

        char buf[64];
        memset(buf, 0, sizeof(buf));
        kseek(fd, 28, SEEK_SET);
        kseek(fd, 29, SEEK_CUR);
        kread(buf, sizeof(buf) - 1, 1, fd);
        vga_puts(buf);
    }

    // kmain is eternal
    for (;;);
}