#include <fs/file.h>
#include <fs/fat/fat16.h>
#include <common.h>
#include <utils.h>
#include <libk/string.h>
#include <memory/kheap.h>
#include <drivers/disk.h>

struct file_descriptor
{
    int index;
    struct filesystem *fs;
    void *private; // private data for internal fd
    DISK *disk;
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

static file_mode_t _get_file_mode_from_str(const char *s) {
    file_mode_t mode = FILE_MODE_INVALID;
    if (!s) return mode;
    if (strnieq(s, "r", 1)) {
        mode = FILE_MODE_READ;
    } else if (strnieq(s, "w", 1)) {
        mode = FILE_MODE_WRITE;
    } else if (strnieq(s, "a", 1)) {
        mode = FILE_MODE_APPEND;
    }
    return mode;
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

struct filesystem* fs_resolve(DISK *disk) {
    for (int i = 0; i < MAX_FILESYSTEMS; ++i) {
        if (fs_arr[i] != NULL && fs_arr[i]->resolve(disk) == 0) {
            return fs_arr[i];
        }
    }
    return NULL;
}

int kopen(const char *filename, const char *mode_str) {
    PATH *root = path_parse(filename, NULL);
    if (!root) return -STATUS_UNKNOWN_ERROR;

    PATH_TOKEN *first = path_get_first_token(root);
    if (!first) {
        path_destroy(root);
        return -STATUS_INVALID_ARG_ERROR;
    }

    int drive_no = path_get_drive_number(root);
    DISK *disk = disk_get(drive_no);
    if (!disk) {
        path_destroy(root);
        return -STATUS_IO_ERROR;
    }

    struct filesystem *fs = disk_get_fs(disk);
    if (!fs) {
        path_destroy(root);
        return -STATUS_IO_ERROR;
    }

    file_mode_t mode = _get_file_mode_from_str(mode_str);
    if (mode == FILE_MODE_INVALID) {
        path_destroy(root);
        return -STATUS_INVALID_ARG_ERROR;
    }
    
    void *fd_private_data = fs->open(disk, first, mode);
    if (IS_ERR(fd_private_data)) {
        path_destroy(root);
        return ERR_I(fd_private_data);
    }

    struct file_descriptor *fd = NULL;
    int res = _create_file_descriptor(&fd);
    if (res < 0) {
        path_destroy(root);
        return -STATUS_INVALID_ARG_ERROR;
    }

    fd->fs = fs;
    fd->private = fd_private_data;
    fd->disk = disk;

    path_destroy(root);   
    return fd->index;
}

int kread(void *dest, u32 size, u32 nmemb, int fd) {
    if (!dest || size == 0 || nmemb == 0 || fd < 1) return -STATUS_INVALID_ARG_ERROR;

    struct file_descriptor *desc = _get_file_descriptor(fd);
    if (!desc) return -STATUS_INVALID_ARG_ERROR;

    return desc->fs->read(desc->disk, desc->private, size, nmemb, (char*)dest);
}