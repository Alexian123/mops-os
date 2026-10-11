#include <libk/string.h>

char* strrstr(const char *s1, const char *s2) {
    if (*s2 == '\0') {
        const char *end = s1;
        while (*end != '\0')
            ++end;
        return (char *)end;
    }
    const char *last = NULL;
    for (const char *p = s1; *p != '\0'; ++p) {
        const char *a = p;
        const char *b = s2;
        while (*a != '\0' && *b != '\0' && *a == *b) {
            ++a;
            ++b;
        }
        if (*b == '\0')
            last = p;
    }
    return (char *)last;
}