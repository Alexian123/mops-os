#include <libk/string.h>

char* strstr(const char *s1, const char *s2) {
    if (*s2 == '\0')
        return (char *)s1;
    for (const char *p = s1; *p != '\0'; ++p) {
        const char *a = p;
        const char *b = s2;
        while (*a != '\0' && *b != '\0' && *a == *b) {
            ++a;
            ++b;
        }
        if (*b == '\0')
            return (char *)p;
    }
    return NULL;
}