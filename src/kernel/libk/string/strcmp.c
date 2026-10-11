#include <libk/string.h>

int strcmp(const char *dst, const char *src) {
    while (*dst && (unsigned char)*dst == (unsigned char)*src) {
        ++dst;
        ++src;
    }
    return (int)(unsigned char)*dst - (int)(unsigned char)*src;
}