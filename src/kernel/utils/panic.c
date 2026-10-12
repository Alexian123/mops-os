#include <utils.h>
#include <drivers/vga_text.h>

void ASMCALL panic(const char *msg) {
    vga_clearscr();
    vga_puts("KERNEL PANIC!\n");
    vga_puts(msg);
    hang();
}