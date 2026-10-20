#include <memory/heap.h>
#include <libk/string.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN    0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE     0x00

#define HEAP_BLOCK_HAS_NEXT     0b10000000
#define HEAP_BLOCK_IS_FIRST     0b01000000

static int _heap_validate_alignment(void *ptr) {
    return ((u32)ptr % HEAP_BLOCK_SIZE) == 0;
}

static int _heap_validate_table(void *ptr, void *end, struct heap_table *table) {
    szt table_size = (szt)(end - ptr);
    szt total_blocks = table_size / HEAP_BLOCK_SIZE;
    if (table->total != total_blocks) return -STATUS_INVALID_ARG_ERROR;
    return 0;
}

static void* _heap_block_to_address(struct heap *heap, int block) {
    return heap->start_addr + (block * HEAP_BLOCK_SIZE);
}

static int _heap_address_to_block(struct heap *heap, void *ptr) {
    return ((int)(ptr - heap->start_addr)) / HEAP_BLOCK_SIZE;
}

static u32 _heap_align_val_to_upper(u32 val) {
    if (val % HEAP_BLOCK_SIZE == 0) return val; // already aligned
    val = (val - (val % HEAP_BLOCK_SIZE));
    val += HEAP_BLOCK_SIZE;
    return val;
}

static int _heap_get_entry_type(hb_table_entry_t entry) {
    return entry & 0x0F;
}

static int _heap_get_start_block(struct heap *heap, u32 blocks) {
    struct heap_table *table = heap->table;
    int bc = 0;     // current block
    int bs = -1;    // start block

    for (szt i = 0; i < table->total; ++i) {
        if (_heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }

        // if this is first block
        if (bs == -1) {
            bs = i;
        }
        
        ++bc;
        if (bc == blocks) {
            break;
        }
    }

    if (bs == -1) {
        return -STATUS_NO_MEMORY_ERROR;
    }

    return bs;
}

static void _heap_mark_blocks_taken(struct heap *heap, int start_block, u32 blocks) {
    int end_block = start_block + blocks - 1;

    hb_table_entry_t entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (blocks > 1) {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; ++i) {
        // set current entry
        heap->table->entries[i] = entry;

        // prepare next entry
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

static void _heap_mark_blocks_free(struct heap *heap, int block) {
    struct heap_table *table = heap->table;
    for (int i = block; i < (int)table->total; ++i) {
        hb_table_entry_t entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT)) break;
    }
}

static void* _heap_malloc_blocks(struct heap *heap, u32 blocks) {
    void *addr = 0;

    int start_block = _heap_get_start_block(heap, blocks);
    if (start_block < 0) return NULL;

    addr = _heap_block_to_address(heap, start_block);

    // mark blocks as used
    _heap_mark_blocks_taken(heap, start_block, blocks);

    return addr;
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table) {
    int res = 0;

    if (!_heap_validate_alignment(ptr) || !_heap_validate_alignment(end)) {
        return -STATUS_INVALID_ARG_ERROR;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->start_addr = ptr;
    heap->table = table;

    res = _heap_validate_table(ptr, end, table);
    if (res < 0) return res;

    szt table_size = sizeof(hb_table_entry_t) * table->total;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return res;
}

void* heap_malloc(struct heap *heap, szt size) {
    szt aligned_size = (szt)_heap_align_val_to_upper((u32)size);
    u32 total_blocks = aligned_size / HEAP_BLOCK_SIZE;
    return _heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap *heap, void *ptr) {
    int block = _heap_address_to_block(heap, ptr);
    _heap_mark_blocks_free(heap, block);
}