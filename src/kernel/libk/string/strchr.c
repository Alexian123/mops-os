#include <libk/string.h>

char* strchr(const char *s, char c) {
    unsigned char target = (unsigned char)c;
    for (;;) {
        if ((unsigned char)*s == target)
            return (char *)s;
        if (*s == '\0')
            return NULL;
        ++s;
    }
}