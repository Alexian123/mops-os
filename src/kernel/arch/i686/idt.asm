section .asm

extern int21h_handler
extern no_int_handler

; void ASMCALL idt_load(struct idt_desc *idt_descriptor)
global idt_load
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]  ; first arg
    lidt [ebx]

    pop ebp
    ret

global int21h
int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

global no_int
no_int:
    cli
    pushad
    call no_int_handler
    popad
    sti
    iret