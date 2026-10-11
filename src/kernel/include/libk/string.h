#ifndef __LIBK_STRING_H__
#define __LIBK_STRING_H__

#include <stddef.h>
#include <stdbool.h>

extern size_t strlen(const char *s);
extern size_t strnlen(const char *s, size_t n);

extern char* strcat(char *dst, const char *src);
extern char* strncat(char *dst, const char *src, size_t n);

extern char* strcpy(char *dst, const char *src);
extern char* strncpy(char *dst, const char *src, size_t n);

extern int strcmp(const char *dst, const char *src);
extern int strncmp(const char *dst, const char *src, size_t n);

extern bool streq(const char *dst, const char *src);
extern bool strneq(const char *dst, const char *src, size_t n);

extern char* strchr(const char *s, char c);
extern char* strnchr(const char *s, char c, size_t n);

extern char* strrchr(const char *s, char c);
extern char* strnrchr(const char *s, char c, size_t n);

extern char* strstr(const char *s1, const char *s2);
extern char* strnstr(const char *s1, const char *s2, size_t n);

extern char* strrstr(const char *s1, const char *s2);
extern char* strnrstr(const char *s1, const char *s2, size_t n);

#endif