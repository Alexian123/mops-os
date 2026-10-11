#include <libk/string.h>

char* strrchr(const char *s, char c) {
    unsigned char target = (unsigned char)c;
    const char *last = NULL;
    for (;;) {
        if ((unsigned char)*s == target)
            last = s;
        if (*s == '\0')
            break;
        ++s;
    }
    return (char *)last;
}