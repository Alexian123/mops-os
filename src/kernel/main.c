#include <common.h>
#include <interrupts/idt.h>
#include <port_io.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <utils.h>

// drivers
#include <drivers/vga_text.h>
#include <drivers/disk.h>

// libk
#include <libk/string.h>
#include <libk/ctype.h>

static PAGING_4GB_CHUNK *kchunk = NULL;

void kmain(void) {
    vga_clearscr();
    vga_puts("Welcome to MOPS-OS!\n");

    kheap_init();
    vga_puts("Initialized kernel heap.\n");

    idt_init();
    vga_puts("Loaded IDT.\n");

    // create kernel page dir and load it
    kchunk = paging_create_4gb_chunk(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);    // should remove PAGING_ACCESS_FROM_ALL
    u32 *kernel_page_dir = paging_get_directory_from_4gb_chunk(kchunk);
    paging_switch(kernel_page_dir);
    paging_enable();
    vga_puts("Loaded kernel page directory. Paging is now active.\n");

    char buf[512];
    memset(buf, 0, 512);
    disk_read_sectors(0, 1, buf);   // read boot sector

    enable_interrupts();
    vga_puts("Interrupts are now active. Beware!\n");
    
    // kmain is eternal
    for (;;);
}