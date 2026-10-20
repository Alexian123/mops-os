#include <fs/fat/fat16.h>
#include <libk/string.h>

static void* _fat16_open(struct disk *dist, PATH_TOKEN *tok, file_mode_t mode) {
    return NULL;
}

static int _fat16_resolve(struct disk *disk) {
    return -1;
}

static struct filesystem fat16_fs = {
    .open = _fat16_open,
    .resolve = _fat16_resolve
};

struct filesystem* fat16_init() {
    strncpy(fat16_fs.name, "FAT16", MAX_FILESYSTEM_NAME_LEN);
    return &fat16_fs;
}