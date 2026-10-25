#include <utils.h>
#include <drivers/vga_text.h>
#include <libk/ctype.h>

static const char HEX[] = "0123456789ABCDEF";

static void _print_hex(u64 val, int nibbles) {
    vga_puts("0x");
    for (int i = 0; i < nibbles; ++i) {
        vga_putc(toupper((char)HEX[(val >> 4*i) & 0xF]));
        val >>= 4;
    }
}

void print_hexb(u8 val) {
    _print_hex((u64)val, 2);
}

void print_hexw(u16 val) {
     _print_hex((u64)val, 4);
}

void print_hexdw(u32 val) {
     _print_hex((u64)val, 8);
}

void print_hexqw(u64 val) {
     _print_hex((u64)val, 16);
}