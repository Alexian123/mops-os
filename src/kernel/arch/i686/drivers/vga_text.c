#include <drivers/vga_text.h>
#include <common.h>

#define VGA_ADDRESS 0xB8000

static volatile u8* const vga = (volatile u8*)VGA_ADDRESS;

static u8 color = 0x0F;
static u8 tab_size = 4;
static u8 col = 0;
static u8 row = 0;

static void _vga_putc_at(char c, u8 row, u8 col)
{
    szt index = (row * VGA_COLUMNS + col) * 2;
    vga[index] = (unsigned char)c;
    vga[index + 1] = color;
}

static void _vga_scroll(void){
    for (szt i = 1; i< VGA_ROWS; ++i) {
        for (szt j = 0; j < VGA_COLUMNS; ++j) {
            szt src = (i * VGA_COLUMNS + j) * 2;
            szt dst = ((i - 1) * VGA_COLUMNS + j) * 2;
            vga[dst] = vga[src];
            vga[dst + 1] = vga[src + 1];
        }
    }

    for (szt i = 0; i < VGA_COLUMNS; ++i) {
        szt index = ((VGA_ROWS - 1) * VGA_COLUMNS + i) * 2;
        vga[index] = ' ';
        vga[index + 1] = color;
    }
}

void vga_clearscr(void) {
    for (szt i = 0; i < VGA_COLUMNS * VGA_ROWS; ++i) {
        vga[i*2] = ' ';
        vga[i*2 + 1] = color;
    }
    row = 0;
    col = 0;
}

void vga_putc(char c)
{
    if (c == '\n') {
        col = 0;
        ++row;
    } else if (c == '\r') {
        col = 0;
    } else if (c == '\t') {
        int spaces = tab_size - (col % tab_size);
        while (spaces-- > 0) {
            vga_putc(' ');
        }
    } else {
        _vga_putc_at(c, row, col);
        ++col;
        if (col >= VGA_COLUMNS) {
            col = 0;
            ++row;
        }
    }

    if (row >= VGA_ROWS) {
        _vga_scroll();
        row = VGA_ROWS - 1;
    }
}

void vga_puts(const char *str) {
    while (*str) {
        vga_putc(*str);
        ++str;
    }
}