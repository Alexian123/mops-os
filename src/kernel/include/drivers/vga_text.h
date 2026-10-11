#ifndef __VGA_TEXT_H___
#define __VGA_TEXT_H___

#define VGA_COLUMNS 80
#define VGA_ROWS    25

void vga_clearscr(void);
void vga_putc(char c);
void vga_puts(const char *str);

#endif