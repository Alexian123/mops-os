#ifndef __KHEAP_H__
#define __KHEAP_H__

#include <common.h>

void kheap_init(void);
void* kmalloc(szt size);
void* kzalloc(szt size);
void kfree(void *ptr);

#endif