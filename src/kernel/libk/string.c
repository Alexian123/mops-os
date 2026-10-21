#include <libk/string.h>
#include <libk/ctype.h>

/* ------------------------ Memory functions ------------------------ */
void* memcpy(void *dst, const void *src, szt n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    for (szt i = 0; i < n; ++i)
        d[i] = s[i];
    return dst;
}

void* memset(void *s, int c, szt n)
{
    unsigned char *p = (unsigned char *)s;
    unsigned char value = (unsigned char)c;
    for (szt i = 0; i < n; ++i)
        p[i] = value;
    return s;
}
/* ------------------------------------------------------------------ */

/* ------------------------ Length functions ------------------------ */
szt strlen(const char *s) {
    szt len = 0;
    while (*s) {
        ++len;
        ++s;
    }
    return len;
}

szt strnlen(const char *s, szt n) {
    szt len;
    for (len = 0; len < n; ++len) {
        if (s[len] == 0) break;
    }
    return len;
}

szt strnlent(const char *s, szt n, char t) {
    szt len;
    for (len = 0; len < n; ++len) {
        if (s[len] == t || s[len] == 0) 
            break;
    }
    return len;
}
/* ------------------------------------------------------------------ */


/* ------------------------- Copy functions ------------------------- */
char* strcpy(char *dst, const char *src) {
    char *ret = dst;
    while ((*dst++ = *src++) != '\0');
    return ret;
}

char* strncpy(char *dst, const char *src, szt n)
{
    char *ret = dst;
    szt i = 0;
    while (i < n && src[i] != '\0') {
        dst[i] = src[i];
        ++i;
    }
    while (i < n) {
        dst[i] = '\0';
        ++i;
    }
    return ret;
}
/* ------------------------------------------------------------------ */


/* ------------------------ Compare functions ----------------------- */
int strcmp(const char *dst, const char *src) {
    while (*dst && (unsigned char)*dst == (unsigned char)*src) {
        ++dst;
        ++src;
    }
    return (int)(unsigned char)*dst - (int)(unsigned char)*src;
}

int stricmp(const char *dst, const char *src) {
    while (*dst && (unsigned char)tolower(*dst) == (unsigned char)tolower(*src)) {
        ++dst;
        ++src;
    }
    return (int)(unsigned char)tolower(*dst) - (int)(unsigned char)tolower(*src);
}


int strncmp(const char *dst, const char *src, szt n) {
    for (szt i = 0; i < n; ++i) {
        unsigned char a = (unsigned char)dst[i];
        unsigned char b = (unsigned char)src[i];
        if (a != b)
            return (int)a - (int)b;
        if (a == '\0')
            return 0;
    }
    return 0;
}

int strnicmp(const char *dst, const char *src, szt n) {
    for (szt i = 0; i < n; ++i) {
        unsigned char a = (unsigned char)tolower(dst[i]);
        unsigned char b = (unsigned char)tolower(src[i]);
        if (a != b)
            return (int)a - (int)b;
        if (a == '\0')
            return 0;
    }
    return 0;
}

int streq(const char *dst, const char *src) {
    return strcmp(dst, src) == 0;
}

int strieq(const char *dst, const char *src) {
    return stricmp(dst, src) == 0;
}

int strneq(const char *dst, const char *src, szt n) {
    return strncmp(dst, src, n) == 0;
}

int strnieq(const char *dst, const char *src, szt n) {
    return strnicmp(dst, src, n) == 0;
}
/* ------------------------------------------------------------------ */


/* ------------------------ Append functions ------------------------ */
char* strcat(char *dst, const char *src) {
    szt len = strlen(dst);
    while (*src) {
        dst[len] = *src;
        ++src;
        ++len;
    }
    dst[len] = 0;
    return dst;
}

char* strncat(char *dst, const char *src, szt n)
{
    szt len = strlen(dst);
    szt i = 0;
    while (i < n && src[i] != 0) {
        dst[len + i] = src[i];
        ++i;
    }
    dst[len + i] = 0;
    return dst;
}
/* ------------------------------------------------------------------ */


/* ------------------------ Search functions ------------------------ */
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

char* strnchr(const char *s, char c, szt n) {
    unsigned char target = (unsigned char)c;
    for (szt i = 0; i < n; ++i) {
        if ((unsigned char)s[i] == target)
            return (char *)&s[i];
        if (s[i] == '\0')
            return NULL;
    }
    return NULL;
}

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

char* strnrchr(const char *s, char c, szt n) {
    unsigned char target = (unsigned char)c;
    const char *last = NULL;
    for (szt i = 0; i < n; ++i) {
        if ((unsigned char)s[i] == target)
            last = &s[i];
        if (s[i] == '\0')
            break;
    }
    return (char *)last;
}

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

char* strnstr(const char *s1, const char *s2, szt n) {
    if (*s2 == '\0')
        return (char *)s1;
    for (szt i = 0; i < n && s1[i] != '\0'; ++i) {
        szt j = 0;
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

char* strnrstr(const char *s1, const char *s2, szt n) {
    if (*s2 == '\0') {
        szt len = 0;
        while (len < n && s1[len] != '\0')
            ++len;
        return (char *)&s1[len];
    }
    const char *last = NULL;
    for (szt i = 0; i < n && s1[i] != '\0'; ++i) {
        szt j = 0;
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
/* ------------------------------------------------------------------ */