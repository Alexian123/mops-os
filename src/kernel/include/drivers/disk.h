#ifndef __DISK_H__
#define __DISK_H__

#include <common.h>

// read from Primary Disk
int disk_read_sectors(int lba, int total, void *dst);

#endif