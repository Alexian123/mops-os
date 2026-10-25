#ifndef __UTILS_H__
#define __UTILS_H__

#include <common.h>

extern void ASMCALL disable_interrupts(void);
extern void ASMCALL enable_interrupts(void);
extern void ASMCALL hang(void);

void panic(const char *msg);

void print_hexb(u8 val);
void print_hexw(u16 val);
void print_hexdw(u32 val);
void print_hexqw(u64 val);

#endif