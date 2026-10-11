#include <libk/string.h>

char* strnstr(const char *s1, const char *s2, size_t n) {
    if (*s2 == '\0')
        return (char *)s1;
    for (size_t i = 0; i < n && s1[i] != '\0'; ++i) {
        size_t j = 0;
        while (j < n - i &&
               s1[i + j] != '\0' &&
               s2[j] != '\0' &&
               s1[i + j] == s2[j]) {
            ++j;
        }
        if (s2[j] == '\0')
            return (char *)&s1[i];
    }
    return NULL;
}