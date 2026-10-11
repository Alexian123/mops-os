#include <libk/string.h>

char* strcat(char *dst, const char *src) {
    size_t len = strlen(dst);
    while (*src) {
        dst[len] = *src;
        ++src;
        ++len;
    }
    dst[len] = 0;
    return dst;
}