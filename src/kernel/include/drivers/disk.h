#ifndef __DISK_H__
#define __DISK_H__

#include <common.h>

#define DISK_TYPE_PHYS  0

typedef unsigned int disk_type_t;

typedef struct disk DISK;

void disk_locate_and_init(void);
DISK* disk_get(int index);
int disk_read_blocks(DISK* disk, u32 lba, int total, void *dst);

#endif