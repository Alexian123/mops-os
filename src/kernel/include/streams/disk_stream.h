#ifndef __DISK_STREAM_H__
#define __DISK_STREAM_H__

#include <drivers/disk.h>

typedef struct disk_stream DISK_STREAM;

DISK_STREAM* disk_stream_create(int disk_id);
void disk_stream_destroy(DISK_STREAM *stream);

int disk_stream_seek(DISK_STREAM *stream, int pos);
int disk_stream_read(DISK_STREAM *stream, void *out_buffer, int total);

#endif 