#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VGA_ADDRESS 0xB8000
#define VGA_COLUMNS 80
#define VGA_ROWS    25

void print_char_vga(char c, char color, int col, int row) {
    volatile char* vga = (volatile char*) VGA_ADDRESS;
    int index = (row * VGA_COLUMNS + col) * 2;
    vga[index] = c;
    vga[index + 1] = color; // 0x0F white text on black bg
}

void kmain(void) {
    // clear screen
    for (int i = 0; i < VGA_COLUMNS; ++i) {
        for (int j = 0; j < VGA_ROWS; ++j) {
            print_char_vga(' ', 0x0F, i, j);
        }
    }

    // print welcome msg
    print_char_vga('W', 0x0F, 0, 0);
    print_char_vga('e', 0x0F, 1, 0);
    print_char_vga('l', 0x0F, 2, 0);
    print_char_vga('c', 0x0F, 3, 0);
    print_char_vga('o', 0x0F, 4, 0);
    print_char_vga('m', 0x0F, 5, 0);
    print_char_vga('e', 0x0F, 6, 0);
    print_char_vga('!', 0x0F, 7, 0);

    // hang
    for (;;);
}