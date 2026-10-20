#ifndef __DISK_H__
#define __DISK_H__

#include <common.h>
#include <fs/file.h>

typedef struct disk DISK;

void disk_locate_and_init(void);
DISK* disk_get(int index);

int disk_read_blocks(DISK *disk, u32 lba, int total, void *dst);
int disk_get_id(DISK *disk);
int disk_get_sector_size(DISK *disk);

void disk_set_fs_private(DISK *disk, void *fs_private);
void* disk_get_fs_private(DISK *disk);

struct filesystem* disk_get_fs(DISK *disk);

#endif