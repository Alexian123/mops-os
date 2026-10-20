#include <drivers/disk.h>
#include <port_io.h>
#include <libk/string.h>

#define DISK_TYPE_PHYS  0

typedef unsigned int disk_type_t;

struct disk
{
    disk_type_t type;
    int sector_size;
    int id;
    struct filesystem *fs;
    void *fs_private;
};

static struct disk saved_disk;

// ATA
// read from Primary Disk
int disk_read_sectors(u32 lba, int total, void *dst) {
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (u8)(lba & 0xFF));
    outb(0x1F4, (u8)(lba >> 8));
    outb(0x1F5, (u8)(lba >> 16));
    outb(0x1F7, 0x20);

    u16 *dstw = (u16*) dst;
    for (int i = 0; i < total; ++i) {
        // wait for data to become ready
        char c = inb(0x1F7);
        while (!(c & 0x08)) {
            c = inb(0x1F7);
        }

        // read from disk to mem
        for (int j = 0; j < 256; ++j) {
            *dstw = inw(0x1F0);
            ++dstw;
        }
    }

    return 0;
}

void disk_locate_and_init(void) {
    memset(&saved_disk, 0, sizeof(struct disk));
    saved_disk.type = DISK_TYPE_PHYS;
    saved_disk.sector_size = DISK_SECTOR_SIZE;
    saved_disk.fs = fs_resolve(&saved_disk);
    saved_disk.id = 0;
}

DISK* disk_get(int index) {
    if (index != 0) return NULL;
    return &saved_disk;
}

int disk_read_blocks(DISK *disk, u32 lba, int total, void *dst) {
    if (disk != &saved_disk) return -STATUS_IO_ERROR;
    return disk_read_sectors(lba, total, dst);
}

int disk_get_id(DISK *disk) {
    if (!disk) return -STATUS_INVALID_ARG_ERROR;
    return disk->id;
}

int disk_get_sector_size(DISK *disk) {
    if (!disk) return -STATUS_INVALID_ARG_ERROR;
    return disk->sector_size;
}

void* disk_get_fs_private(DISK *disk) {
    if (!disk) return NULL;
    return disk->fs_private;
}

void disk_set_fs_private(DISK *disk, void *fs_private) {
    if (disk) {
        disk->fs_private = fs_private;
    }
}

struct filesystem* disk_get_fs(DISK *disk) {
    if (!disk) return NULL;
    return disk->fs;
}