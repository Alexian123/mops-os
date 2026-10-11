#include <libk/string.h>

char* strnrstr(const char *s1, const char *s2, size_t n) {
    if (*s2 == '\0') {
        size_t len = 0;
        while (len < n && s1[len] != '\0')
            ++len;
        return (char *)&s1[len];
    }
    const char *last = NULL;
    for (size_t i = 0; i < n && s1[i] != '\0'; ++i) {
        size_t j = 0;
        while (j < n - i &&
               s1[i + j] != '\0' &&
               s2[j] != '\0' &&
               s1[i + j] == s2[j]) {
            ++j;
        }
        if (s2[j] == '\0')
            last = &s1[i];
    }
    return (char *)last;
}