#ifndef __IDT_H__
#define __IDT_H__

void idt_init(void);
void idt_set(int interrupt, void *addr);

#endif