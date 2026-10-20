#ifndef __FILE_H__
#define __FILE_H__

#include <fs/path.h>

typedef unsigned int file_seek_mode_t;
enum file_seek_mode
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int file_mode_t;
enum file_mode
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;

typedef void* (*fs_open_func)(struct disk *disk, PATH_TOKEN *tok, file_mode_t mode);
typedef int (*fs_resolve_func)(struct disk *disk);

struct filesystem
{
    fs_resolve_func resolve;    // should return 0 if disk is using this fs
    fs_open_func open;
    char name[16];
};

void fs_init();
void fs_add(struct filesystem *fs);
struct filesystem* fs_resolve(struct disk *disk);

int kopen(const char *filename, const char *mode);

#endif