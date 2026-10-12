#include <memory/kheap.h>
#include <memory/heap.h>
#include <drivers/vga_text.h>
#include <utils.h>
#include <libk/string.h>

static struct heap kernel_heap;
static struct heap_table kernel_heap_table;

void kheap_init(void) {
    kernel_heap_table.entries = (hb_table_entry*)KERNEL_HEAP_TABLE_ADDRESS;
    kernel_heap_table.total = KERNEL_HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;

    void *end = (void*)(KERNEL_HEAP_ADDRESS + KERNEL_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)KERNEL_HEAP_ADDRESS, end, &kernel_heap_table);
    
    if (res < 0) {  // error
        panic("Failed to create kernel heap");
    }
}

void* kmalloc(szt size) {
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(szt size) {
    void *ptr = kmalloc(size);
    if (ptr) memset(ptr, 0, size);
    return ptr;
}

void kfree(void *ptr) {
    heap_free(&kernel_heap, ptr);
}