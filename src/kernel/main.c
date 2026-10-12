#include <common.h>
#include <interrupts/idt.h>
#include <io/port_io.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <utils.h>

// drivers
#include <drivers/vga_text.h>

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

    // allocate some physical memory and map vaddr 0x1000 to it
    char *ptr = kzalloc(4096);
    paging_set(kernel_page_dir, (void*)0x1000, (u32)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    paging_enable();

    vga_puts("Loaded kernel page directory. Paging is now active.\n");

    enable_interrupts();
    vga_puts("Interrupts are now active. Beware!\n");
    
    // kmain is eternal
    for (;;);
}