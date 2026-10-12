#include <memory/paging.h>
#include <memory/kheap.h>
#include <utils.h>

struct paging_4gb_chunk
{
    u32 *dir_entry;
};

static u32 *current_dir = NULL;

extern void ASMCALL paging_load_directory(u32 *dir); 

PAGING_4GB_CHUNK* paging_create_4gb_chunk(u8 flags) {
    u32 *dir = kzalloc(sizeof(u32) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    if (!dir) panic("Failed to allocate memory for page directory");

    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; ++i) {
        u32 *entry = kzalloc(sizeof(u32) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        if (!entry) panic("Failed to allocate memory for page table");

        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; ++j) {
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        dir[i] = (u32)entry | flags | PAGING_IS_WRITEABLE;
    }
    
    PAGING_4GB_CHUNK *chunk = kzalloc(sizeof(struct paging_4gb_chunk));
    if (!chunk) panic("Failed to allocate memory for 4GB chunk");

    chunk->dir_entry = dir;
    return chunk;
}

u32* paging_get_directory_from_4gb_chunk(PAGING_4GB_CHUNK *chunk) {
    return chunk->dir_entry;
}

void paging_switch(u32 *dir) {
    paging_load_directory(dir);
    current_dir = dir;
}