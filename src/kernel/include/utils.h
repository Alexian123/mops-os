#ifndef __UTILS_H__
#define __UTILS_H__

#include <common.h>

extern void ASMCALL disable_interrupts(void);
extern void ASMCALL enable_interrupts(void);
extern void ASMCALL hang(void);

void ASMCALL panic(const char *msg);

#endif