#include <idt.h>
#include <common.h>
#include <libk/string.h>

struct idt_entry
{
    u16 offset1;    // offset bits 0-15
    u16 selector;   // selector in GDT
    u8 unused;      // set to 0
    u8 type_attr;   // descriptor type & attribs
    u16 offset2;    // offset bits 16-31  
} ATTR_PACKED;

struct idt_desc
{
    u16 limit;
    u32 base;
} ATTR_PACKED;

static struct idt_entry idt[MAX_INTERRUPTS];
static struct idt_desc idt_descriptor;

extern void ASMCALL idt_load(struct idt_desc *idt_desc);

#include <drivers/vga_text.h>
void idt_zero(void) {
    vga_puts("Oh no! Division by zero is not cool.\n");
    for (;;);
}

void idt_init(void) {
    memset(idt, 0, sizeof(idt));
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (u32)idt;

    idt_set(0, idt_zero);

    // lidt
    idt_load(&idt_descriptor);
}

void idt_set(int interrupt, void *addr) {
    struct idt_entry *desc = &idt[interrupt]; 
    desc->offset1 = (u32) addr & 0xFFFF;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->unused = 0;
    desc->type_attr = 0xEE;
    desc->offset2 = (u32) addr >> 16;
}