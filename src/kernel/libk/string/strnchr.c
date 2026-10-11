#include <libk/string.h>

char* strnchr(const char *s, char c, size_t n) {
    unsigned char target = (unsigned char)c;
    for (size_t i = 0; i < n; ++i) {
        if ((unsigned char)s[i] == target)
            return (char *)&s[i];
        if (s[i] == '\0')
            return NULL;
    }
    return NULL;
}