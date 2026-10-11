#ifndef __LIBK_STRING_H__
#define __LIBK_STRING_H__

#include <common.h>

void* memcpy(void *dst, const void *src, szt n);
void* memset(void *s, int c, szt n);

szt strlen(const char *s);
szt strnlen(const char *s, szt n);

char* strcat(char *dst, const char *src);
char* strncat(char *dst, const char *src, szt n);

char* strcpy(char *dst, const char *src);
char* strncpy(char *dst, const char *src, szt n);

int strcmp(const char *dst, const char *src);
int strncmp(const char *dst, const char *src, szt n);

int streq(const char *dst, const char *src);
int strneq(const char *dst, const char *src, szt n);

char* strchr(const char *s, char c);
char* strnchr(const char *s, char c, szt n);

char* strrchr(const char *s, char c);
char* strnrchr(const char *s, char c, szt n);

char* strstr(const char *s1, const char *s2);
char* strnstr(const char *s1, const char *s2, szt n);

char* strrstr(const char *s1, const char *s2);
char* strnrstr(const char *s1, const char *s2, szt n);

#endif