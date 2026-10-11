#include <libk/string.h>

char* strncat(char *dst, const char *src, size_t n)
{
    size_t len = strlen(dst);
    size_t i = 0;
    while (i < n && src[i] != 0) {
        dst[len + i] = src[i];
        ++i;
    }
    dst[len + i] = 0;
    return dst;
}