#include <common.h>
#include <idt.h>
#include <port_io.h>
#include <memory/kheap.h>

// drivers
#include <drivers/vga_text.h>

extern void bad(void);

void kmain(void) {
    vga_clearscr();
    vga_puts("Welcome!\n");

    kheap_init();

    idt_init();
    
    // hang
    for (;;);
}