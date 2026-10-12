#include <common.h>
#include <interrupts/idt.h>
#include <io/port_io.h>
#include <memory/kheap.h>
#include <utils.h>

// drivers
#include <drivers/vga_text.h>

extern void bad(void);

void kmain(void) {
    vga_clearscr();
    vga_puts("Welcome to MOPS-OS!\n");

    kheap_init();
    vga_puts("Initialized kernel heap.\n");

    idt_init();
    vga_puts("Loaded IDT.\n");

    enable_interrupts();
    vga_puts("Interrupts are now active. Beware!\n");
    
    // kmain is eternal
    for (;;);
}