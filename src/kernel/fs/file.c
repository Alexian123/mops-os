#include <fs/file.h>
#include <fs/fat/fat16.h>
#include <common.h>
#include <utils.h>
#include <libk/string.h>
#include <memory/kheap.h>

struct file_descriptor
{
    int index;
    struct filesystem *fs;
    void *private; // private data for internal fd
    struct disk *disk;
};

struct filesystem *fs_arr[MAX_FILESYSTEMS];
struct file_descriptor *fd_arr[MAX_FILE_DESCRIPTORS];

static struct filesystem** _get_free_fs() {
    for (int i = 0; i < MAX_FILESYSTEMS; ++i) {
        if (fs_arr[i] == NULL) {
            return &fs_arr[i];
        }
    }
    return NULL;
}

static void _fs_builtin_load() {
    fs_add(fat16_init());
}

static int _create_file_descriptor(struct file_descriptor **out_desc) {
    int res = -STATUS_NO_MEMORY_ERROR;
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; ++i) {
        if (fd_arr[i] == NULL) {
            struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
            if (!desc) break;

            // fd's start at 1 
            desc->index = i + 1;
            fd_arr[i] = desc;
            *out_desc = desc;
            res = 0;
            break;
        }
    }
    return res;
}

static struct file_descriptor* _get_file_descriptor(int index) {
    if (index <= 0 || index > MAX_FILE_DESCRIPTORS) return NULL;
    return fd_arr[index - 1];
}

void fs_init() {
    memset(fd_arr, 0, sizeof(fd_arr));
    memset(fs_arr, 0, sizeof(fs_arr));
    _fs_builtin_load();
}

void fs_add(struct filesystem *fs) {
    if (!fs) return;
    struct filesystem **free_fs = _get_free_fs();
    if (!free_fs) {
        panic("No free filesystem slots left");
    }
    *free_fs = fs;
}

struct filesystem* fs_resolve(struct disk *disk) {
    for (int i = 0; i < MAX_FILESYSTEMS; ++i) {
        if (fs_arr[i] != NULL && fs_arr[i]->resolve(disk) == 0) {
            return fs_arr[i];
        }
    }
    return NULL;
}

int kopen(const char *filename, const char *mode) {
    return -1;
}