#include <libk/string.h>

size_t strnlen(const char *s, size_t n) {
    size_t len;
    for (len = 0; len < n; ++len) {
        if (s[len] == 0) break;
    }
    return len;
}