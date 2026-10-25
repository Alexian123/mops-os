#ifndef __LIBK_STDLIB_H__
#define __LIBK_STDLIB_H__

#include <common.h>

// converts int to string
// dest capacity must be at least 3 bytes (1 byte per digit + 1 byte for sign + null terminator)
int itoa(char *dest, int num);

#endif