#include <drivers/vga_text.h>
#include <stdint.h>
#include <stddef.h>

#define VGA_ADDRESS 0xB8000

static volatile uint8_t* const vga = (volatile uint8_t*)VGA_ADDRESS;

static uint8_t color = 0x0F;
static uint8_t tab_size = 4;
static uint8_t col = 0;
static uint8_t row = 0;

static void _vga_putc_at(char c, uint8_t row, uint8_t col);
static void _vga_scroll(void);

void vga_clearscr(void) {
    for (size_t i = 0; i < VGA_COLUMNS * VGA_ROWS; ++i) {
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

void _vga_putc_at(char c, uint8_t row, uint8_t col)
{
    size_t index = (row * VGA_COLUMNS + col) * 2;
    vga[index] = (unsigned char)c;
    vga[index + 1] = color;
}

void _vga_scroll(void){
    for (size_t i = 1; i< VGA_ROWS; ++i) {
        for (size_t j = 0; j < VGA_COLUMNS; ++j) {
            size_t src = (i * VGA_COLUMNS + j) * 2;
            size_t dst = ((i - 1) * VGA_COLUMNS + j) * 2;
            vga[dst] = vga[src];
            vga[dst + 1] = vga[src + 1];
        }
    }

    for (size_t i = 0; i < VGA_COLUMNS; ++i) {
        size_t index = ((VGA_ROWS - 1) * VGA_COLUMNS + i) * 2;
        vga[index] = ' ';
        vga[index + 1] = color;
    }
}