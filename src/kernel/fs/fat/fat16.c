#include <fs/fat/fat16.h>
#include <streams/disk_stream.h>
#include <memory/kheap.h>
#include <utils.h>
#include <libk/string.h>

#define FAT16_SIGNATURE                 0x29
#define FAT16_FAT_ENTRY_SIZE            2
#define FAT16_BAD_SECTOR                0xFF7
#define FAT16_UNUSED                    0

typedef unsigned int fat_item_type_t;
enum fat_item_type
{
    FAT_ITEM_TYPE_DIRECTORY = 0,
    FAT_ITEM_TYPE_FILE      = 1
};

// FAT directory entry attribs mask
#define FAT_FILE_RD_ONLY                0x01
#define FAT_FILE_HIDDEN                 0x02
#define FAT_FILE_SYSTEM                 0x04
#define FAT_FILE_VOLUME_LABEL           0x08
#define FAT_FILE_SUB_DIRECTORY          0x10
#define FAT_FILE_ARCHIVE                0x20
#define FAT_FILE_DEVICE                 0x40
#define FAT_FILE_RESERVED               0x80

struct fat_header
{
    u8 jmp_short_and_nop[3];
    u8 oem_identifier[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors;
    u8 fat_copies;
    u16 root_dir_entries;
    u16 number_of_sectors;
    u8 media_type;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 number_of_heads;
    u32 hidden_sectors;
    u32 sectors_big;
} ATTR_PACKED;

struct extended_bpb
{
    u8 drive_number;
    u8 winnt_bit;
    u8 signature;
    u32 volume_id;
    u8 volume_id_string[11];
    u8 system_id_string[8];
} ATTR_PACKED;

struct fat_header_full
{
    struct fat_header primary;
    union 
    {
        struct extended_bpb extended;
    };
};

struct fat_directory_item
{
    u8 filename[8];
    u8 ext[3];
    u8 attribute;
    u8 reserved;
    u8 creation_time_tenths_of_a_sec;
    u16 creation_time;
    u16 creation_date;
    u16 last_access;
    u16 first_cluster_hi;
    u16 last_mod_time;
    u16 last_mod_date;
    u16 first_cluster_lo;
    u32 file_size;
} ATTR_PACKED;

struct fat_directory
{
    struct fat_directory_item *item;
    int total;
    int begin_sector_pos;
    int end_sector_pos;
};

struct fat_item
{
    union
    {
        struct fat_directory_item *item;    // regular file
        struct fat_directory      *dir;     // directory
    };
    fat_item_type_t type;
};

struct fat_item_descriptor
{
    struct fat_item *item;
    u32 pos;
};

struct fat_private
{
    struct fat_header_full header;
    struct fat_directory root_dir;
    DISK_STREAM *cluster_read_stream;
    DISK_STREAM *fat_read_stream;
    DISK_STREAM *directory_stream;
};

static struct filesystem fat16_fs;

static int _fat16_init_private(DISK *disk, struct fat_private *private) {
    int disk_id = disk_get_id(disk);
    private->cluster_read_stream = disk_stream_create(disk_id);
    private->fat_read_stream = disk_stream_create(disk_id);
    private->directory_stream = disk_stream_create(disk_id);

    if (!private->cluster_read_stream || !private->fat_read_stream || !private->directory_stream) {
        return -STATUS_NO_MEMORY_ERROR;
    }

    return 0;
}

static void _fat16_cleanup_private(struct fat_private *private) {
    disk_stream_destroy(private->cluster_read_stream);
    disk_stream_destroy(private->fat_read_stream);
    disk_stream_destroy(private->directory_stream);
    kfree(private);
}

static int _fat16_sector_to_absolute(DISK *disk, int sector) {
    return sector * disk_get_sector_size(disk);
}

static int _fat16_get_total_items_in_dir(DISK *disk, struct fat_private *private, int dir_start_sector) {
    struct fat_directory_item item, empty_item;
    memset(&item, 0, sizeof(item));
    memset(&empty_item, 0, sizeof(empty_item));

    int res = 0, i = 0;
    int dir_start_pos = _fat16_sector_to_absolute(disk, dir_start_sector);
    DISK_STREAM *stream = private->directory_stream;

    res = disk_stream_seek(stream, dir_start_pos);
    if (res < 0) return res;

    for (;;) {
        res = disk_stream_read(stream, &item, sizeof(item));
        if (res < 0) return res;

        if (item.filename[0] == 0) { // blank record, marks end
            break;
        }

        if (item.filename[0] == 0xE5) { // item unused
            continue;
        }

        ++i;
    }

    return i;
}

static int _fat16_load_root_dir(DISK *disk, struct fat_private *private) {
    struct fat_directory *dir = &(private->root_dir);
    struct fat_header *primary = &(private->header.primary);
    int root_dir_sector_pos = primary->fat_copies * primary->sectors_per_fat + primary->reserved_sectors;
    int root_dir_entries = primary->root_dir_entries;
    int root_dir_size = root_dir_entries * sizeof(struct fat_directory_item);
    int disk_sector_size = disk_get_sector_size(disk);
    int total_sectors = root_dir_size / disk_sector_size;

    if (root_dir_size % disk_sector_size) {
        ++total_sectors;
    }

    int total_items = _fat16_get_total_items_in_dir(disk, private, root_dir_sector_pos);

    struct fat_directory_item *dir_item = kzalloc(root_dir_size);
    if (!dir_item) return -STATUS_NO_MEMORY_ERROR;

    DISK_STREAM *stream = private->directory_stream;
    int res = disk_stream_seek(stream, _fat16_sector_to_absolute(disk, root_dir_sector_pos));
    if (res < 0) {
        kfree(dir_item);
        return res;
    }

    res = disk_stream_read(stream, dir_item, root_dir_size);
    if (res < 0) {
        kfree(dir_item);
        return res;
    }

    dir->item = dir_item;
    dir->total = total_items;
    dir->begin_sector_pos = root_dir_sector_pos;
    dir->end_sector_pos = root_dir_sector_pos + root_dir_size / disk_sector_size;
    return 0;
}

static int fat16_resolve(DISK *disk) {
    if (!disk) return -STATUS_INVALID_ARG_ERROR;

    struct fat_private *private = kzalloc(sizeof(struct fat_private));
    if (!private) return -STATUS_NO_MEMORY_ERROR;

    int res = _fat16_init_private(disk, private);
    if (res < 0) return res;

    DISK_STREAM *stream = disk_stream_create(disk_get_id(disk));
    if (!stream) {
        _fat16_cleanup_private(private);
        return -STATUS_NO_MEMORY_ERROR;
    }

    res = disk_stream_read(stream, &private->header, sizeof(private->header));
    if (res < 0) {
        _fat16_cleanup_private(private);
        disk_stream_destroy(stream);
        return res;
    }

    // check if FAT16 signature is present
    if (private->header.extended.signature != 0x29) {
        _fat16_cleanup_private(private);
        disk_stream_destroy(stream);
        return -STATUS_FS_NOT_PRESENT_ERROR;
    }

    res = _fat16_load_root_dir(disk, private);
    if (res < 0) {
        _fat16_cleanup_private(private);
        disk_stream_destroy(stream);
        return res;
    }

    disk_set_fs_private(disk, private);
    disk_stream_destroy(stream);
    return 0;
}

static void* fat16_open(DISK *disk, PATH_TOKEN *tok, file_mode_t mode) {
    if (!disk || !tok) return NULL;
    return NULL;
}

struct filesystem* fat16_init() {
    memset(&fat16_fs, 0, sizeof(fat16_fs));
    fat16_fs.open = fat16_open;
    fat16_fs.resolve = fat16_resolve;
    strncpy(fat16_fs.name, "FAT16", MAX_FILESYSTEM_NAME_LEN);
    return &fat16_fs;
}