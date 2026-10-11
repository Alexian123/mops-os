section .asm

; void ASMCALL idt_load(struct idt_desc *idt_descriptor)
global idt_load
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]  ; first arg
    lidt [ebx]

    pop ebp
    ret