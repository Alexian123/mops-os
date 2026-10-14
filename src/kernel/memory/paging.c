#include <memory/paging.h>
#include <memory/kheap.h>
#include <utils.h>

struct paging_4gb_chunk
{
    u32 *dir_entry;
};

static u32 *current_dir = NULL;

extern void ASMCALL paging_load_directory(u32 *dir); 

static int _paging_get_indices(void *vaddr, u32 *out_dir_idx, u32 *out_table_idx) {
    if (!paging_is_aligned(vaddr))  return -STATUS_INVALID_ARG_ERROR;

    if (out_dir_idx) {
        *out_dir_idx = ((u32)vaddr / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    }

    if (out_table_idx) {
        *out_table_idx = ((u32)vaddr % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
    }

    return 0;
}

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

bool paging_is_aligned(void *addr) {
    return ((u32)addr % PAGING_PAGE_SIZE) == 0;
}

int paging_set(u32 *dir, void *vaddr, u32 val) {
    if (!paging_is_aligned(vaddr))  return -STATUS_INVALID_ARG_ERROR;
    
    u32 dir_idx = 0, table_idx = 0;
    int res = _paging_get_indices(vaddr, &dir_idx, &table_idx);
    if (res < 0) return res;

    u32 entry = dir[dir_idx];
    u32 *table = (u32*)(entry & 0xFFFFF000);
    table[table_idx] = val;

    return 0;
}