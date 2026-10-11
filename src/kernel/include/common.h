#ifndef __COMMON_H__
#define __COMMON_H__

/* ------------------------- Compiler Headers ------------------------ */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/* ------------------------------------------------------------------- */


/* ----------------------- Compiler Attributes ----------------------- */
#define ATTR_PACKED     __attribute__((packed))
#define ASMCALL         __attribute__((cdecl))
/* ------------------------------------------------------------------- */


/* ---------------------------- Constants ---------------------------- */
#define MAX_INTERRUPTS          512
#define KERNEL_CODE_SELECTOR    0x08
#define KERNEL_DATA_SELECTOR    0x10
/* ------------------------------------------------------------------- */


/* -------------------------- Type Aliases --------------------------- */
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef size_t      szt;
/* ------------------------------------------------------------------- */

#endif