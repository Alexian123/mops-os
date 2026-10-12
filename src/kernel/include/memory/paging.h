#ifndef __PAGING_H__
#define __PAGING_H__

#define PAGING_CACHE_DISABLED       0b00010000
#define PAGING_WRITE_THROUGH        0b00001000
#define PAGING_ACCESS_FROM_ALL      0b00000100
#define PAGING_IS_WRITEABLE         0b00000010
#define PAGING_IS_PRESENT           0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE      1024
#define PAGING_PAGE_SIZE                    4096

#include <common.h>

typedef struct paging_4gb_chunk PAGING_4GB_CHUNK;

PAGING_4GB_CHUNK* paging_create_4gb_chunk(u8 flags);
u32* paging_get_directory_from_4gb_chunk(PAGING_4GB_CHUNK *chunk);
void paging_switch(u32 *dir);

// must be called after loading a page dir
extern void ASMCALL paging_enable(void);

#endif