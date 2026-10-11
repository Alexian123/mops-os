#include <libk/string.h>

char* strnrchr(const char *s, char c, size_t n) {
    unsigned char target = (unsigned char)c;
    const char *last = NULL;
    for (size_t i = 0; i < n; ++i) {
        if ((unsigned char)s[i] == target)
            last = &s[i];
        if (s[i] == '\0')
            break;
    }
    return (char *)last;
}