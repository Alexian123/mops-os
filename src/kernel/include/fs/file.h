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

typedef unsigned int file_stat_flags_t;
enum file_stat_flags
{
    FILE_STAT_RDONLY = 0x01,
};

struct filestat
{
    file_stat_flags_t flags;
    u32 size;
};

typedef struct disk DISK; // from drivers/disk.h

typedef int (*fs_resolve_func_t)(DISK *disk);
typedef void* (*fs_open_func_t)(DISK *disk, PATH_TOKEN *tok, file_mode_t mode);
typedef int (*fs_read_func_t)(DISK *disk, void *private, u32 size, u32 nmemb, char *out);
typedef int (*fs_seek_func_t)(void *private, u32 offset, file_seek_mode_t mode);
typedef int (*fs_stat_func_t)(DISK *disk, void *private, struct filestat *stat);

struct filesystem
{
    fs_resolve_func_t resolve;    // should return 0 if disk is using this fs
    fs_open_func_t open;
    fs_read_func_t read;
    fs_seek_func_t seek;
    fs_stat_func_t stat;
    char name[MAX_FILESYSTEM_NAME_LEN];
};

void fs_init();
void fs_add(struct filesystem *fs);
struct filesystem* fs_resolve(DISK *disk);

int kopen(const char *filename, const char *mode_str);
int kread(void *dest, u32 size, u32 nmemb, int fd);
int kseek(int fd, int offset, file_seek_mode_t from);
int kstat(int fd, struct filestat *stat);

#endif