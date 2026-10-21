#ifndef __LIBK_STRING_H__
#define __LIBK_STRING_H__

#include <common.h>

void* memcpy(void *dst, const void *src, szt n);
void* memset(void *s, int c, szt n);

szt strlen(const char *s);
szt strnlen(const char *s, szt n);
szt strnlent(const char *s, szt n, char t); // check an extra terminator, on top of '\0'

char* strcat(char *dst, const char *src);
char* strncat(char *dst, const char *src, szt n);

char* strcpy(char *dst, const char *src);
char* strncpy(char *dst, const char *src, szt n);

int strcmp(const char *dst, const char *src);
int stricmp(const char *dst, const char *src); // case-insensitive strcmp
int strncmp(const char *dst, const char *src, szt n);
int strnicmp(const char *dst, const char *src, szt n); // case-insensitive strncmp

int streq(const char *dst, const char *src); // checks equality, strcmp wrapper
int strieq(const char *dst, const char *src); // case-insensitive streq
int strneq(const char *dst, const char *src, szt n); // checks equality, strncmp wrapper
int strnieq(const char *dst, const char *src, szt n); // case-insensitive strneq

char* strchr(const char *s, char c);
char* strnchr(const char *s, char c, szt n); // checks the first n bytes

char* strrchr(const char *s, char c); // reverse strchr
char* strnrchr(const char *s, char c, szt n); // reverse strnchr

char* strstr(const char *s1, const char *s2);
char* strnstr(const char *s1, const char *s2, szt n); // checks the first n bytes

char* strrstr(const char *s1, const char *s2); // reverse strstr
char* strnrstr(const char *s1, const char *s2, szt n); // reverse strnstr

#endif