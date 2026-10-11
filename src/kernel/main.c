// gcc stuff
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// drivers
#include <drivers/vga_text.h>

void kmain(void) {
    vga_clearscr();
    vga_puts("Welcome!\n");

    // hang
    for (;;);
}