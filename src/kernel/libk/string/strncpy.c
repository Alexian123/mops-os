#include <libk/string.h>

char* strncpy(char *dst, const char *src, size_t n)
{
    char *ret = dst;
    size_t i = 0;
    while (i < n && src[i] != '\0') {
        dst[i] = src[i];
        ++i;
    }
    while (i < n) {
        dst[i] = '\0';
        ++i;
    }
    return ret;
}