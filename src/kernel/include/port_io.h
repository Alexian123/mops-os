#ifndef __PORT_IO_H__
#define __PORT_IO_H__

#include <common.h>

extern u8 ASMCALL inb(u16 port);
extern u16 ASMCALL inw(u16 port);

extern void ASMCALL outb(u16 port, u8 data);
extern void ASMCALL outw(u16 port, u16 data);

#endif