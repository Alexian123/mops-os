#include <streams/disk_stream.h>
#include <memory/kheap.h>
#include <libk/string.h>

struct disk_stream
{
    int byte_pos;
    DISK *disk;
};

DISK_STREAM* disk_stream_create(int disk_id) {
    DISK *disk = disk_get(disk_id);
    if (!disk)  return NULL;

    struct disk_stream *stream = kzalloc(sizeof(struct disk_stream));
    if (!stream) return NULL;

    stream->byte_pos = 0;
    stream->disk = disk;
    return stream;
}

void disk_stream_destroy(DISK_STREAM *stream) {
    if (stream) kfree(stream);
}

int disk_stream_seek(DISK_STREAM *stream, int pos) {
    if (!stream) return -STATUS_INVALID_ARG_ERROR;
    stream->byte_pos = pos;
    return 0;
}

int disk_stream_read(DISK_STREAM *stream, void *out_buffer, int total) {
    if (!stream || !out_buffer || total <= 0)
        return -STATUS_INVALID_ARG_ERROR;

    char *out = (char*)out_buffer;
    int remaining = total;

    while (remaining > 0) {
        int sector = stream->byte_pos / DISK_SECTOR_SIZE;
        int offset = stream->byte_pos % DISK_SECTOR_SIZE;
        char buf[DISK_SECTOR_SIZE];

        // read a whole sector
        int res = disk_read_blocks(stream->disk, sector, 1, buf);
        if (res < 0) return -STATUS_IO_ERROR;

        // bytes remaining in this sector
        int available = DISK_SECTOR_SIZE - offset;

        // read only what fits in this sector and the request
        int to_read = remaining < available ? remaining : available;
        memcpy(out, buf + offset, to_read);

        // advance both positions
        out += to_read;
        stream->byte_pos += to_read;
        remaining -= to_read;
    }

    return 0;
}

