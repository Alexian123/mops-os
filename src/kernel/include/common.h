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
#define KERNEL_CODE_SELECTOR        0x08
#define KERNEL_DATA_SELECTOR        0x10

#define MAX_INTERRUPTS              512

#define HEAP_BLOCK_SIZE             4096

#define KERNEL_HEAP_SIZE_BYTES      104857600   // 100 MB
#define KERNEL_HEAP_ADDRESS         0x01000000
#define KERNEL_HEAP_TABLE_ADDRESS   0x00007E00

#define DISK_SECTOR_SIZE            512 // bytes

#define STATUS_OK                   0
#define STATUS_IO_ERROR             1
#define STATUS_INVALID_ARG_ERROR    2
#define STATUS_NO_MEMORY_ERROR      3
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