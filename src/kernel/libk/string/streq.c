#include <libk/string.h>

bool streq(const char *dst, const char *src) {
    return strcmp(dst, src) == 0;
}