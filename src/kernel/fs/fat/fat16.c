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

struct fat_fd
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
    dir->end_sector_pos = root_dir_sector_pos + total_sectors;
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

static void _fat16_fat_filename_to_cstr(const char *src, int src_len, char **out) {
    for (int i = 0; i < src_len && src[i] != ' '; ++i)
        *(*out)++ = src[i];
}

static void _fat16_get_full_relative_filename(struct fat_directory_item *item, char *out_name, szt len) {
    memset(out_name, 0, len);
    char *out_tpm = out_name;
    _fat16_fat_filename_to_cstr((const char*)item->filename, 8, &out_tpm);
    if (item->ext[0] != ' ') { // check if file has extension
        *out_tpm++ = '.';
        _fat16_fat_filename_to_cstr((const char*)item->ext, 3, &out_tpm);
    }
}

static u32 _fat16_get_first_cluster_of(struct fat_directory_item *dir_item) {
    return (((u32)dir_item->first_cluster_hi) << 16) | (u32)dir_item->first_cluster_lo; 
}

static int _fat16_cluster_to_sector(struct fat_private *priv, int cluster) {
    // Why (cluster - 2) you ask? RTFM!
    return priv->root_dir.end_sector_pos + ((cluster - 2) * priv->header.primary.sectors_per_cluster);
}

static u32 _fat16_get_first_fat_sector(struct fat_private* private) {
    return private->header.primary.reserved_sectors;
}

static int _fat16_get_fat_entry(DISK *disk, int cluster) {
    struct fat_private *priv = (struct fat_private*)disk_get_fs_private(disk);
    DISK_STREAM *stream = priv->fat_read_stream;
    if (!stream) return -STATUS_IO_ERROR;

    u32 fat_table_pos = _fat16_get_first_fat_sector(priv) * disk_get_sector_size(disk);
    u32 entry_pos = fat_table_pos + cluster * FAT16_FAT_ENTRY_SIZE;

    int res = disk_stream_seek(stream, entry_pos);
    //int res = disk_stream_seek(stream, fat_table_pos * cluster * FAT16_FAT_ENTRY_SIZE);
    if (res < 0) return res;

    u16 entry = 0;
    res = disk_stream_read(stream, &entry, sizeof(entry));
    if (res < 0) return res;

    return entry;
}

static int _fat16_get_cluster_for_offset(DISK *disk, int start_cluster, int offset) {
    struct fat_private *priv = (struct fat_private*)disk_get_fs_private(disk);
    int disk_sector_size = disk_get_sector_size(disk);
    int size_of_cluster_bytes = priv->header.primary.sectors_per_cluster * disk_sector_size;
    int cluster_to_use = start_cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;

    for (int i = 0; i < clusters_ahead; ++i) {
        int entry = _fat16_get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF) { // last entry in file
            return -STATUS_IO_ERROR;
        }

        // check if sector is bad :'(
        if (entry == FAT16_BAD_SECTOR) {
            return -STATUS_IO_ERROR;
        }

        // check if sector is reserved
        if (entry == 0xFF0 || entry == 0xFF6) {
            return -STATUS_IO_ERROR;
        }

        if (entry == 0x00) { // FAT is corrupted
            return -STATUS_IO_ERROR;
        }

        cluster_to_use = entry;
    }

    return cluster_to_use;
}

static int _fat16_read_internal_stream(DISK *disk, DISK_STREAM *stream, int start_cluster, int offset, int total, void *out) {
    while (total > 0) {
        struct fat_private *priv = (struct fat_private*)disk_get_fs_private(disk);
        int disk_sector_size = disk_get_sector_size(disk);
        int size_of_cluster_bytes = priv->header.primary.sectors_per_cluster * disk_sector_size;
        int cluster_to_use = _fat16_get_cluster_for_offset(disk, start_cluster, offset);
        if (cluster_to_use < 0) return cluster_to_use;

        int offset_from_cluster = offset % size_of_cluster_bytes;
        int start_sector = _fat16_cluster_to_sector(priv, cluster_to_use);
        int start_pos = start_sector * disk_sector_size + offset_from_cluster;
        
        int bytes_remaining_in_cluster = size_of_cluster_bytes - offset_from_cluster;
        int total_to_read = total > bytes_remaining_in_cluster ? bytes_remaining_in_cluster : total;

        int res = disk_stream_seek(stream, start_pos);
        if (res < 0) return res;


        res = disk_stream_read(stream, out, total_to_read);
        if (res < 0) return res;

        total -= total_to_read;
        offset += total_to_read;
        out += total_to_read;
    }
    return 0;
}

static int _fat16_read_internal(DISK *disk, int start_cluster, int offset, int total, void *out) {
    struct fat_private *priv = (struct fat_private*)disk_get_fs_private(disk);
    DISK_STREAM *stream = priv->cluster_read_stream;
    return _fat16_read_internal_stream(disk, stream, start_cluster, offset, total, out);
}

static void _fat16_free_directory(struct fat_directory *dir) {
    if (!dir) return;
    if (dir->item) {
        kfree(dir->item);
    }
    kfree(dir);
}

static struct fat_directory*  _fat16_load_fat_directory(DISK *disk, struct fat_directory_item *dir_item) {
    if (!(dir_item->attribute & FAT_FILE_SUB_DIRECTORY)) return NULL;

    struct fat_directory *dir = kzalloc(sizeof(struct fat_directory));
    if (!dir) return NULL;

    struct fat_private *priv = disk_get_fs_private(disk);
    if (!priv) {
        _fat16_free_directory(dir);
        return NULL;
    }

    int cluster = _fat16_get_first_cluster_of(dir_item);
    int sector = _fat16_cluster_to_sector(priv, cluster);
    int total_items = _fat16_get_total_items_in_dir(disk, priv, sector);
    dir->total = total_items;

    int dir_size = total_items * sizeof(struct fat_directory_item);
    dir->item = kzalloc(dir_size);
    if (!dir->item) {
        _fat16_free_directory(dir);
        return NULL;
    }

    int res = _fat16_read_internal(disk, cluster, 0, dir_size, dir->item);
    if (res < 0) {
        _fat16_free_directory(dir);
        return NULL;
    }

    return dir;
}

static struct fat_directory_item* _fat16_clone_dir_item(struct fat_directory_item * dir_item, szt size) {
    if (size < sizeof(struct fat_directory_item)) return NULL;
    struct fat_directory_item *item_clone = kzalloc(size);
    if (!item_clone) return NULL;
    memcpy(item_clone, dir_item, size);
    return item_clone;
}

static void _fat16_free_fat_item(struct fat_item *item) {
    if (!item) return;
    if (item->type == FAT_ITEM_TYPE_DIRECTORY && item->dir) {
        _fat16_free_directory(item->dir);
    } else if (item->type == FAT_ITEM_TYPE_FILE && item->item) {
        kfree(item->item);
    }
    kfree(item);
}

static struct fat_item* _fat16_create_fat_item_for_dir_item(DISK *disk, struct fat_directory_item *dir_item) {
    struct fat_item *item = kzalloc(sizeof(struct fat_item));
    if (!item) return NULL;

    // check if item is a sub directory
    if (dir_item->attribute & FAT_FILE_SUB_DIRECTORY) {
        item->dir = _fat16_load_fat_directory(disk, dir_item);
        if (!item->dir) {
            _fat16_free_fat_item(item);
            return NULL;
        }
        item->type = FAT_ITEM_TYPE_DIRECTORY;
    } else {
        item->type = FAT_ITEM_TYPE_FILE;
        item->item = _fat16_clone_dir_item(dir_item, sizeof(struct fat_directory_item));
        if (!item->item) {
            _fat16_free_fat_item(item);
            return NULL;
        }
    }
    return item;
}

static struct fat_item* _fat16_find_item_in_dir(DISK *disk, struct fat_directory *dir, const char *name) {
    struct fat_item *item = NULL;
    char filename[PATH_MAX_LEN];
    for (int i = 0; i < dir->total; ++i) {
        _fat16_get_full_relative_filename(&(dir->item[i]), filename, sizeof(filename));
        if (strnieq(filename, name, sizeof(filename))) {
            // found
            item = _fat16_create_fat_item_for_dir_item(disk, &(dir->item[i]));
            break;
        }
    }
    return item;
}

static struct fat_item* _fat16_get_dir_entry(DISK *disk, PATH_TOKEN *tok) {
    struct fat_private *private = (struct fat_private*)disk_get_fs_private(disk);
    if (!private) return NULL;

    struct fat_item *crt_item = NULL;
    struct fat_item *root_item = _fat16_find_item_in_dir(disk, &private->root_dir, path_get_token_text(tok));
    if (!root_item) return NULL;

    PATH_TOKEN *next_tok = path_get_next_token(tok);
    crt_item = root_item;
    while (next_tok) {
        if (crt_item->type != FAT_ITEM_TYPE_DIRECTORY) {
            crt_item = 0;
            break;
        }

        struct fat_item* item_tmp = _fat16_find_item_in_dir(disk, crt_item->dir, path_get_token_text(next_tok));
        _fat16_free_fat_item(crt_item);
        crt_item = item_tmp;
        next_tok = path_get_next_token(next_tok);
    }

    return crt_item;
}

static void* fat16_open(DISK *disk, PATH_TOKEN *tok, file_mode_t mode) {
    if (!disk || !tok) return ERR(-STATUS_INVALID_ARG_ERROR);

    // TODO: remove the READ ONLY restriction after fat16_write is implemented
    if (mode != FILE_MODE_READ) return ERR(-STATUS_UNSUPPORTED_ERROR);

    struct fat_fd *fd = kzalloc(sizeof(struct fat_fd));
    if (!fd) return ERR(-STATUS_NO_MEMORY_ERROR);

    fd->pos = 0;

    fd->item = _fat16_get_dir_entry(disk, tok);
    if (!fd->item) {
        kfree(fd);
        return ERR(-STATUS_IO_ERROR);
    }

    return fd;
}

static int fat16_read(DISK *disk, void *private, u32 size, u32 nmemb, char *out) {
    struct fat_fd *desc = (struct fat_fd*)private;
    struct fat_directory_item *dir_item = desc->item->item;
    int offset = desc->pos;
    int read = 0;
    for (u32 i = 0; i < nmemb; ++i) {
        int res = _fat16_read_internal(
            disk,
            _fat16_get_first_cluster_of(dir_item),
            offset,
            size,
            out
        );
        if (res < 0) break;
        out += size;
        offset += size;
        ++read;
    }   
    return read;
}

static int fat16_seek(void *private, u32 offset, file_seek_mode_t mode) {
    struct fat_fd *desc = (struct fat_fd*)private;
    struct fat_item *item = desc->item;
    if (item->type != FAT_ITEM_TYPE_FILE) return -STATUS_INVALID_ARG_ERROR;

    struct fat_directory_item *dir_item = item->item;
    if (offset >= dir_item->file_size) return -STATUS_INVALID_ARG_ERROR;

    switch (mode) {
    case SEEK_CUR:
        desc->pos += offset;
        break;
    case SEEK_SET:
        desc->pos = offset;
        break;
    case SEEK_END:
        return -STATUS_UNSUPPORTED_ERROR;
    default:
        return -STATUS_INVALID_ARG_ERROR;
    }

    return STATUS_OK;
}

int fat16_stat(DISK *disk, void *private, struct filestat *stat) {
    struct fat_fd *desc = (struct fat_fd*)private;
    struct fat_item *item = desc->item;
    if (item->type != FAT_ITEM_TYPE_FILE) return -STATUS_UNSUPPORTED_ERROR;

    struct fat_directory_item *dir_item = item->item;
    stat->size = dir_item->file_size;
    stat->flags = 0x0;

    if (dir_item->attribute & FAT_FILE_RD_ONLY) {
        stat->flags |= FILE_STAT_RDONLY;
    }

    return STATUS_OK;
}

struct filesystem* fat16_init() {
    memset(&fat16_fs, 0, sizeof(fat16_fs));
    fat16_fs.resolve = fat16_resolve;
    fat16_fs.open = fat16_open;
    fat16_fs.read = fat16_read;
    fat16_fs.seek = fat16_seek;
    fat16_fs.stat = fat16_stat;
    strncpy(fat16_fs.name, "FAT16", MAX_FILESYSTEM_NAME_LEN);
    return &fat16_fs;
}