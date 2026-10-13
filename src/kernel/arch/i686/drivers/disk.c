#include <drivers/disk.h>
#include <port_io.h>

// ATA

int disk_read_sectors(int lba, int total, void *dst) {
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