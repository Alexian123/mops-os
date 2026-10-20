#include <fs/fat/fat16.h>
#include <streams/disk_stream.h>
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
    struct fat_directory_item *items;
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

static void* _fat16_open(struct disk *dist, PATH_TOKEN *tok, file_mode_t mode) {
    return NULL;
}

static int _fat16_resolve(struct disk *disk) {
    return -1;
}

static struct filesystem fat16_fs = 
{
    .open = _fat16_open,
    .resolve = _fat16_resolve
};

struct filesystem* fat16_init() {
    strncpy(fat16_fs.name, "FAT16", MAX_FILESYSTEM_NAME_LEN);
    return &fat16_fs;
}