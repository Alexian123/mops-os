#include <libk/string.h>

bool strneq(const char *dst, const char *src, size_t n) {
    return strncmp(dst, src, n) == 0;
}