#include <common.h>
#include <idt.h>
#include <port_io.h>

// drivers
#include <drivers/vga_text.h>

extern void bad(void);

void kmain(void) {
    vga_clearscr();
    vga_puts("Welcome!\n");

    idt_init();

    outb(0x60, 0xFF);

    // hang
    for (;;);
}