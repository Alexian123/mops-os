#include <libk/string.h>

int strncmp(const char *dst, const char *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        unsigned char a = (unsigned char)dst[i];
        unsigned char b = (unsigned char)src[i];
        if (a != b)
            return (int)a - (int)b;
        if (a == '\0')
            return 0;
    }
    return 0;
}