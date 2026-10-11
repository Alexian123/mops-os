#ifndef __VGA_TEXT_H___
#define __VGA_TEXT_H___

#define VGA_COLUMNS 80
#define VGA_ROWS    25

extern void vga_clearscr(void);
extern void vga_putc(char c);
extern void vga_puts(const char *str);

#endif